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
#include "Factory.h"

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
		else if (sep == Separator::h)
			return Separator::i;
		else if (sep == Separator::i)
			return Separator::j;
		else if (sep == Separator::j)
			return Separator::size;
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

	string serialize(World* world, bool first)
	{
		ostringstream oss;
		serialize(world, oss, Separator::a, first);

		return oss.str();
	}

	void serialize(World* world, ostream& os, Separator sep, bool first)
	{
		if (first)
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
		}
		serialize(world->objects, os, sep, first);
	}

	void serialize(set<PhysicalObject*> objects, ostream& os, Separator sep, bool first)
	{
		Separator next = sep_next(sep);
		for (auto& object : objects)
		{
			if (Thymio2* thymio = dynamic_cast<Thymio2*>(object))
			{
				os << *sep;
				serialize(thymio, os, next, first);
			}
			else if (EPuck* epuck = dynamic_cast<EPuck*>(object))
			{
				os << *sep;
				serialize(epuck, os, next, first);
			}
			else if (Sbot* sbot = dynamic_cast<Sbot*>(object))
			{
				os << *sep;
				serialize(sbot, os, next, first);
			}
			else if (Khepera* khepera = dynamic_cast<Khepera*>(object))
			{
				os << *sep;
				serialize(khepera, os, next, first);
			}
			else if (Marxbot* marxbot = dynamic_cast<Marxbot*>(object))
			{
				os << *sep;
				serialize(marxbot, os, next, first);
			}
			else if (PhysicalObject* po = dynamic_cast<PhysicalObject*>(object))
			{
				os << *sep;
				serialize(po, os, next, first);
			}
			else
			{
				cerr << " ===  serialize - NOT DEFINED === " << endl;
			}
		}
	}

	void serialize(const World::GroundTexture& texture, ostream& os, Separator sep, bool first)
	{
		os << texture.width << *sep << texture.height;

		for (auto& datum : texture.data)
		{
			os << *sep << datum;
		}
	}

	void serialize(Robot* r, ostream& os, Separator sep)
	{
		serialize(r->pos, os, sep_next(sep));
		os << *sep << r->angle;
	}

	void serialize(Thymio2* thymio, ostream& os, Separator sep, bool first)
	{
		os << Factory::ROBOT_TYPES::THYMIO2 << *sep;
		os << thymio->getId() << *sep;		// id
		serialize(static_cast<Robot*>(thymio), os, sep_next(sep));

		for (int i = 0; i < Thymio2::LED_COUNT; i++)
		{
			os << *sep;
			serialize(thymio->getColorLed((Thymio2::LedIndex)i), os, sep_next(sep));
		}
	}

	void serialize(EPuck* epuck, ostream& os, Separator sep, bool first)
	{
		os << Factory::ROBOT_TYPES::EPUCK << *sep;
		os << epuck->getId() << *sep;		// id
		serialize(static_cast<Robot*>(epuck), os, sep_next(sep));
	}

	void serialize(Khepera* khepera, ostream& os, Separator sep, bool first)
	{
		os << Factory::ROBOT_TYPES::KHEPERA << *sep;
		os << khepera->getId() << *sep;		// id
		serialize(static_cast<Robot*>(khepera), os, sep_next(sep));
	}

	void serialize(Marxbot* Marxbot, ostream& os, Separator sep, bool first)
	{
		os << Factory::ROBOT_TYPES::MARXBOT << *sep;
		os << Marxbot->getId() << *sep;		// id
		serialize(static_cast<Robot*>(Marxbot), os, sep_next(sep));
	}

	void serialize(Sbot* sbot, ostream& os, Separator sep, bool first)
	{
		os << Factory::ROBOT_TYPES::SBOT << *sep;
		os << sbot->getId() << *sep;		// id
		serialize(static_cast<Robot*>(sbot), os, sep_next(sep));
		os << *sep;
		if (first)
			serialize(sbot->getColor(), os, sep_next(sep));
	}

	void serialize(PhysicalObject* po, ostream& os, Separator sep, bool first)
	{
		os << Factory::ROBOT_TYPES::PHYSICAL_OBJECT << *sep;
		os << po->getId() << *sep;		// id

		serialize(po->pos, os, sep_next(sep));
		os << *sep;
		os << po->angle << *sep;
		serialize(po->getColor(), os, sep_next(sep));
		if(first)
		{
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
				os << *sep;
				os << po->getMass() << *sep;
			}
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
		Separator next = sep_next(sep);
		// a Hull is a vector of Part which are composed of a Polygone, a
		// height, and optionally of textures;.
		for (auto& poly: hull)
		{
			serialize(poly.getShape(), os, sep_next(next));
			os << *next;
			os << poly.getHeight() << *next;
			os << poly.isTextured()<< *next;

			if (poly.isTextured())
			{
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

	World* initWorld(const string& str){
		Separator sep = Separator::a;
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
		deserialize(world, str, sep, true);
		return world;
	}

	static void updateObject(World* world, const std::string str, Separator sep, bool first)
	{
		std::vector<std::string> tabObj = split(str, *sep);
		int type_object = stod(tabObj[0]);
		int id = stod(tabObj[1]);
		for (auto& obj : world->objects)
		{
			if (obj->getId() == id)
			{
				switch(type_object)
				{
					case Factory::ROBOT_TYPES::THYMIO2:
					{
						Thymio2* thymio = dynamic_cast<Thymio2*>(obj);
						if (thymio != NULL)
							deserialize(thymio, str, sep, first);
						break;
					}
					case Factory::ROBOT_TYPES::MARXBOT:
					{
						Marxbot* marxbot = dynamic_cast<Marxbot*>(obj);
						if (marxbot != NULL)
							deserialize(marxbot, str, sep, first);
						break;
					}
					case Factory::ROBOT_TYPES::KHEPERA:
					{
						Khepera* khepera = dynamic_cast<Khepera*>(obj);
						if (khepera != NULL)
							deserialize(khepera, str, sep, first);
						break;
					}
					case Factory::ROBOT_TYPES::EPUCK:
					{
						EPuck* epuck = dynamic_cast<EPuck*>(obj);
						if (epuck != NULL)
							deserialize(epuck, str, sep, first);
						break;
					}
					case Factory::ROBOT_TYPES::SBOT:
					{
						Sbot* sbot = dynamic_cast<Sbot*>(obj);
						if (sbot != NULL)
							deserialize(sbot, str, sep, first);
						break;
					}
					case Factory::ROBOT_TYPES::PHYSICAL_OBJECT:
					{
						deserialize(obj, str, sep, first);
						break;
					}
					default:
					{
						cerr << " ===  Error Deserialize - objects not defined ! === " << endl;
						cerr << type_object << endl;
						break;
					}
				}
				break;
			}
		}
	}

	void deserialize(World* world, const string& str, Separator sep, bool first)
	{
		vector<string>& objects = split(str, *sep);
		// delete world data
		if(first)
			objects.erase(objects.begin());

		Separator next = sep_next(sep);

		for (auto& obj : objects)
		{
			size_t sepIndex = obj.find_first_of(*sep_next(sep));
			if (sepIndex == string::npos) // type not present
				continue;
			std::vector<std::string>& attrs = split(obj, *next);

			int type = stoi(attrs[0]);
			int id = stoi(attrs[1]);

			if (first)
			{
				Factory factory;
				PhysicalObject* o = factory.initObject(type);
				o->setId(id);
				world->addObject(o);
			}

			updateObject(world, obj, next, first);
			delete &attrs;
		}
		delete &objects;
	}

	void deserialize(const string& str, Robot *r, Separator sep)
	{
		std::vector<std::string>& attrs = split(str, *sep);
		r->pos = deserialize<Point>(attrs[0], sep_next(sep));
		r->angle = stod(attrs[1]);
		delete &attrs;
	}

	void deserialize(Thymio2* thymio, const string& str, Separator sep, bool first)
	{
		vector<string>& attrs = split(str, *sep);

		deserialize(attrs[2], static_cast<Robot*>(thymio), sep_next(sep));

		for (int i = 0; i < Thymio2::LED_COUNT; i++)
		{
			Color led = deserialize<Color>(attrs[i + 3], sep_next(sep));
			thymio->setLedColor((Thymio2::LedIndex)i, led);
		}
		delete &attrs;
	}

	void deserialize(Marxbot* marxbot, const string& str, Separator sep, bool first)
	{
		vector<string>& attrs = split(str, *sep);
		deserialize(attrs[2], static_cast<Robot*>(marxbot), sep_next(sep));
		delete &attrs;
	}

	void deserialize(Khepera* khepera, const string& str, Separator sep, bool first)
	{
		vector<string>& attrs = split(str, *sep);
		deserialize(attrs[2], static_cast<Robot*>(khepera), sep_next(sep));
		delete &attrs;
	}

	void deserialize(Sbot* sbot, const string& str, Separator sep, bool first)
	{
		vector<string>& attrs = split(str, *sep);
		deserialize(attrs[2], static_cast<Robot*>(sbot), sep_next(sep));
		if (first)
			sbot->setColor(deserialize<Color>(attrs[3],sep_next(sep)));
		delete &attrs;
	}

	void deserialize(EPuck* epuck, const string& str, Separator sep, bool first)
	{
		vector<string>& attrs = split(str, *sep);
		deserialize(attrs[2], static_cast<Robot*>(epuck), sep_next(sep));
		delete &attrs;
	}

	void deserialize(PhysicalObject* po, const string& str, Separator sep, bool first)
	{
		vector<string>& attrs = split(str, *sep);
		Separator next = sep_next(sep);

		po->setId(stoi(attrs[1]));
		po->pos = deserialize<Point>(attrs[2], next);
		po->angle = stod(attrs[3]);

		if(first)
		{
			po->setColor(deserialize<Color>(attrs[4], next));

			int isCylindric = stoi(attrs[5]);

			if (isCylindric)
			{
				double radius = stod(attrs[6]);
				double height = stod(attrs[7]);
				double mass = stod(attrs[8]);
				po->setCylindric(radius, height, mass);
			}
			else
			{
				PhysicalObject::Hull hull = deserialize<PhysicalObject::Hull>(attrs[6], next);
				double mass = stod(attrs[7]);
				po->setCustomHull(hull, mass);
			}
		}
		delete &attrs;
	}

	template<>
	PhysicalObject::Hull deserialize(const string& str, Separator sep)
	{
		vector<std::string>& parts = split(str, *sep);
		PhysicalObject::Hull hull;
		Separator next = sep_next(sep);

		for (auto& part : parts)
		{
			vector<std::string>& attrs = split(part, *next);

			Polygone shape = deserialize<Polygone>(attrs[0], sep_next(next));
			double height = stod(attrs[1]);
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
			delete &attrs;
		}
		delete &parts;
		return hull;
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
			data.push_back(stod(*it));
		}
		delete &attrs;
		return World::GroundTexture(width, height, data.data());
	}

	template<>
	Polygone deserialize(const string& str, Separator sep)
	{
		vector<string>& points = split(str, *sep);
		Polygone shape;

		for (auto& point : points)
		{
			shape << deserialize<Point>(point, sep_next(sep));
		}
		delete &points;
		return shape;
	}

	template<>
	Textures deserialize(const string& str, Separator sep)
	{
		vector<string>& attrs = split(str, *sep);
		Textures textures;

		for (auto& texture : attrs)
		{
			textures.push_back(deserialize<Texture>(texture, sep_next(sep)));
		}
		delete &attrs;
		return textures;
	}

	template<>
	Texture deserialize(const string& str, Separator sep)
	{
		vector<string>& attrs = split(str, *sep);
		Texture texture;

		for (auto& color : attrs)
		{
			texture.push_back(deserialize<Color>(color, sep_next(sep)));
		}
		delete &attrs;
		return texture;
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

}
