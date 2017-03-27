/*
  Enki - a fast 2D robot simulator
  Copyright © 2017 Jimmy Gouraud <jimmy.gouraud@etu.u-bordeaux.fr>
  Copyright © 2017 Mathieu Lirzin <mathieu.lirzin@etu.u-bordeaux.fr>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Serialize.h"

namespace Enki
{
	using namespace std;

	inline char operator*(const Separator sep)
	{
		return static_cast<char>(sep);
	}

	Separator sep_next(const Separator sep)
	{
		if (sep == Separator::a)
			return Separator::b;
		else if (sep == Separator::b)
			return Separator::c;
		else if (sep == Separator::c)
			return Separator::d;
		else if (sep == Separator::d)
			return Separator::e;
		else if (sep == Separator::e)
			return Separator::f;
		else if (sep == Separator::f)
			return Separator::g;
		else if (sep == Separator::g)
			return Separator::h;
		else if (sep == Separator::i)
			return Separator::j;
		else
			throw runtime_error("separator is too big");
	}

	// Split STR into a vector of string where DELIM is used as the separator.
	// DELIM is not present in the strings returned.
	vector<string>& split(const string &str, char delim)
	{
		stringstream ss(str);
		string item;
		vector<string>* elems = new vector<string>;
		while (getline(ss, item, delim))
		{
			elems->push_back(move(item));
		}

		return *elems;
	}

	string serialize(World* world)
	{
		ostringstream oss;

		serialize(world, oss);

		return oss.str();
	}

	void deserializeUdpate(World* world, const string& str)
	{
		World* remoteWorld = deserialize<World*>(str);

		vector<Thymio2*> th;
		for (auto& object : remoteWorld->objects)
		{
			th.push_back(dynamic_cast<Thymio2*>(object));
		}

		int i = 0;
		for (auto& object : world->objects)
		{
			Thymio2* thymio = dynamic_cast<Thymio2*>(object);
			if (i < th.size())
			{
				thymio->pos = th.at(i)->pos;
				thymio->angle = th.at(i)->angle;
				i++;
			}
		}
	}

	void serialize(set<PhysicalObject*> objects, ostream& os, Separator sep)
	{
		Separator next = sep_next(sep);
		for (auto& object : objects)
		{
			if (Thymio2* thymio = dynamic_cast<Thymio2*>(object))
			{
				os << *sep;
				serialize(thymio, os, next);
			}
			else if (PhysicalObject* po = dynamic_cast<PhysicalObject*>(object))
			{
				os << *sep;
				serialize(po, os, next);
			}
			else
			{
				cerr << " ===  serialize - NOT DEFINED === " << endl;
			}
		}
	}

	void serialize(World* world, ostream& os, Separator sep)
	{
		Separator next = sep_next(sep);
		os << world->wallsType << *next;

		switch (world->wallsType)
		{
		case World::WALLS_SQUARE:
			os << world->w << *next
			   << world->h << *next;
			break;
		case World::WALLS_CIRCULAR:
			os << world->r << *next;
			break;
		default:
			break;
		}

		serialize(world->color, os, sep_next(next));
		os << *next;
		serialize(world->groundTexture, os, sep_next(next));
		serialize(world->objects, os, sep);
	}

	void serialize(const World::GroundTexture& texture, ostream& os, Separator sep)
	{
		os << texture.width << *sep << texture.height;

		for (auto& datum : texture.data)
		{
			os << *sep << datum;
		}
	}

	void serialize(Thymio2* thymio, ostream& os, Separator sep)
	{
		os << THYMIO2 << *sep;
		os << thymio->getId() << *sep;		// id
		serialize(thymio->pos, os, sep_next(sep));
		os << *sep << thymio->angle;

		for (int i = 0; i < Thymio2::LED_COUNT; i++)
		{
			os << *sep;
			serialize(thymio->getColorLed((Thymio2::LedIndex)i), os, sep_next(sep));
		}
	}

	void serialize(PhysicalObject* po, ostream& os, Separator sep)
	{
		os << PHYSICAL_OBJECT << *sep;
		os << po->getId() << *sep;
		serialize(po->pos, os, sep_next(sep));
		os << *sep;
		os << po->angle << *sep;
		serialize(po->getColor(), os, sep_next(sep));
		os << *sep;

		os << po->isCylindric() << *sep;
		if (po->isCylindric())
		{
			os << po->getRadius() << *sep
			   << po->getHeight() << *sep
			   << po->getMass() << *sep;
		}
		else
		{
			serialize(po->getHull(), os, sep_next(sep));
			os << po->getMass() << *sep;
		}
	}

	void serialize(const Color& color, ostream& os, Separator sep)
	{
		os << color.r() << *sep
		   << color.g() << *sep
		   << color.b() << *sep
		   << color.a();
	}

	void serialize(const Point& pos, ostream& os, Separator sep)
	{
		os << pos.x << *sep << pos.y;
	}

	void serialize(const PhysicalObject::Hull& hull, ostream& os, Separator sep)
	{
		// a Hull is a vector of Part which are composed of a Polygone, a
		// height, and optionally of textures;.
		Separator next = sep_next(sep);
		for (auto& poly: hull)
		{
			serialize(poly.getShape(), os, sep_next(next));

			os << poly.getHeight() << *next;
			os << poly.isTextured() << *next;

			if (poly.isTextured())
			{
				os << *next;
				serialize(poly.getTextures(), os, sep_next(next));
			}
			os << *sep;
		}
	}

	void serialize(const Polygone& shape, ostream& os, Separator sep)
	{
		for (auto& point : shape)
		{
			serialize(point, os, sep_next(sep));
			os << *sep;
		}
	}

	void serialize(const Textures& textures, ostream& os, Separator sep)
	{
		for (auto& texture : textures)
		{
			serialize(texture, os, sep_next(sep));
			os << *sep;
		}
	}

	void serialize(const Texture& texture, ostream& os, Separator sep)
	{
		for (auto& color : texture)
		{
			serialize(color, os, sep_next(sep));
			os << *sep;
		}
	}

	template<>
	World* deserialize(const string& str, Separator sep)
	{
		vector<string>& objects = split(str, *sep);
		World* world;

		Separator next = sep_next(sep);
		vector<string>& attrs = split(objects[0], *next);
		int wallsType = stoi(attrs[0]);

		switch (wallsType)
		{
		case World::WALLS_SQUARE:
		{
			double w = stod(attrs[1]);
			double h = stod(attrs[2]);
			Color color = deserialize<Color>(attrs[3], sep_next(next));
			auto gt = deserialize<World::GroundTexture>(attrs[4], sep_next(next));
			world = new World(w, h, color, gt);
			break;
		}
		case World::WALLS_CIRCULAR:
		{
			double r = stod(attrs[1]);
			Color color = deserialize<Color>(attrs[2], sep_next(next));
			auto gt = deserialize<World::GroundTexture>(attrs[3], sep_next(next));
			world = new World(r, color, gt);
			break;
		}
		default:
			world = new World();
		}
		
		// delete world data
		objects.erase(objects.begin());
		// Deserialize objects inside world.
		for (auto& obj : objects)
		{
			size_t sepIndex = obj.find_first_of(*sep_next(sep));
			if (sepIndex == string::npos) // type not present
				continue;

			int type = stoi(obj.substr(0, sepIndex));
			switch(type)
			{
				case THYMIO2:
				{
					Thymio2* thymio = new Thymio2();
					deserialize(thymio, obj, sep_next(sep));
					world->addObject(thymio);
					break;
				}
				case PHYSICAL_OBJECT:
				{
					PhysicalObject* po = new PhysicalObject();
					deserialize(po, obj	, sep_next(sep));
					world->addObject(po);
					break;
				}
				default:
				{
					cerr << " ===  Error Deserialize - objects not defined ! === " << endl;
					cout << type << endl;
					break;
				}
			}
		}
		delete &objects;
		delete &attrs;
		return world;
	}

	void deserialize(Thymio2* thymio,const string& str, Separator sep)
	{
		vector<string>& attrs = split(str, *sep);
		int id = stoi(attrs[1]);
		thymio->pos = deserialize<Point>(attrs[2], sep_next(sep));
		thymio->angle = stod(attrs[3]);

		for (int i = 0; i < Thymio2::LED_COUNT; i++)
		{
			Color led = deserialize<Color>(attrs[i + 4], sep_next(sep));
			thymio->setLedColor((Thymio2::LedIndex)i, led);
		}
		delete &attrs;
	}

	void deserialize(PhysicalObject* po, const string& str, Separator sep)
	{
		// TODO: Deserialize STR for real.  For now return an arbitrary
		// PhysicalObject.
		vector<string> attrs = split(str, *sep);
		Separator next = sep_next(sep);

		po->setId(stoi(attrs[0]));
		po->pos = deserialize<Point>(attrs[1], next);
		po->angle = stod(attrs[2]);
		po->setColor(deserialize<Color>(attrs[3], next));

		{
			int isCylindric = stoi(attrs[4]);

			if (isCylindric)
			{
				double radius = stod(attrs[5]);
				double height = stod(attrs[6]);
				double mass = stod(attrs[7]);
				po->setCylindric(radius, height, mass);
			}
			else
			{
				PhysicalObject::Hull hull = deserialize<PhysicalObject::Hull>(attrs[5], next);
				double mass = stod(attrs[6]);
				po->setCustomHull(hull, mass);
			}
		}
	}

	template<>
	World::GroundTexture deserialize(const string& str, Separator sep)
	{
		vector<string>& attrs = split(str, *sep);
		double height = stod(attrs[0]);
		double width = stod(attrs[1]);

		std::vector<uint32_t> data;
		for (auto it = attrs.begin() + 2; it != attrs.end(); ++it)
		{
			data.push_back(stoi(*it));
		}
		delete &attrs;
		return World::GroundTexture(width, height, data.data());
	}

	template<>
	Color deserialize(const string& str, Separator sep)
	{
		vector<string>& attrs = split(str, *sep);
		double r = stod(attrs[0]);
		double g = stod(attrs[1]);
		double b = stod(attrs[2]);
		double a = stod(attrs[3]);
		delete &attrs;
		return Color(r, g, b, a);
	}

	template<>
	Point deserialize(const string& str, Separator sep)
	{
		vector<string>& attrs = split(str, *sep);
		double x = stod(attrs[0]);
		double y = stod(attrs[1]);
		delete &attrs;
		return Point(x, y);
	}

	template<>
	PhysicalObject::Hull deserialize(const string& str, Separator sep)
	{
		vector<std::string> parts = split(str, *sep);
		parts.pop_back();		// Remove last empty string;
		PhysicalObject::Hull hull;
		Separator next = sep_next(sep);

		for (auto& part : parts)
		{
			vector<std::string> attrs = split(str, *next);
			Polygone shape = deserialize<Polygone>(attrs[0], sep_next(next));
			int height = stoi(attrs[1]);
			int isTextured = stoi(attrs[2]);

			if (isTextured)
			{
				Textures textures = deserialize<Textures>(attrs[3], sep_next(next));
				hull += PhysicalObject::Hull(PhysicalObject::Part(shape, height, textures));
			}
			else
			{
				hull += PhysicalObject::Hull(PhysicalObject::Part(shape, height));
			}
		}

		return hull;
	}

	template<>
	Polygone deserialize(const string& str, Separator sep)
	{
		vector<string> points = split(str, *sep);
		points.pop_back();		// Remove last empty string;
		Polygone shape;

		for (auto& point : points)
		{
			shape << deserialize<Point>(point, sep_next(sep));
		}

		return shape;
	}

	template<>
	Textures deserialize(const string& str, Separator sep)
	{
		vector<string> attrs = split(str, *sep);
		attrs.pop_back();		// Remove last empty string;
		Textures textures;

		for (auto& texture : attrs)
		{
			textures.push_back(deserialize<Texture>(texture, sep_next(sep)));
		}

		return textures;
	}

	template<>
	Texture deserialize(const string& str, Separator sep)
	{
		vector<string> attrs = split(str, *sep);
		attrs.pop_back();		// Remove last empty string;
		Texture texture;

		for (auto& color : attrs)
		{
			texture.push_back(deserialize<Color>(color, sep_next(sep)));
		}

		return texture;
	}

}
