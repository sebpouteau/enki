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
	
	std::vector<std::string> split(const string& str, const char separator)
	{
		std::vector<std::string> tab;
		int pos = -1;
		do
		{
			int begin = pos + 1;
			pos = str.find_first_of(separator, begin);
			tab.push_back(str.substr(begin, pos - begin));
		} while (pos != std::string::npos);
		// delete blank end
		tab.pop_back();
		return tab;
	}


	string serialize(World* world, bool first)
	{
		ostringstream oss;
		serialize(world, oss, first);

		return oss.str();
	}

	void serialize(World* world, ostream& os, bool first)
	{
		os.precision(serialize_precision);
		os.setf(std::ios::fixed);
		if (first)
		{
			os << world->wallsType << TYPE_SEPARATOR;

			switch (world->wallsType)
			{
				case World::WALLS_SQUARE:
					os << world->w << TYPE_SEPARATOR
					<< world->h << TYPE_SEPARATOR;
					break;
				case World::WALLS_CIRCULAR:
					os << world->r << TYPE_SEPARATOR;
					break;
				default:
					break;
			}

			serialize(world->color, os);
			serialize(world->groundTexture, os);
			os << OBJECT_SEPARATOR;

		}
		serialize(world->objects, os, first);
	}

	void serialize(set<PhysicalObject*> objects, ostream& os, bool first)
	{
		for (auto& object : objects)
		{
			if (Thymio2* thymio = dynamic_cast<Thymio2*>(object))
			{
				serialize(thymio, os, first);
			}
			else if (EPuck* epuck = dynamic_cast<EPuck*>(object))
			{
				serialize(epuck, os, first);
			}
			else if (Sbot* sbot = dynamic_cast<Sbot*>(object))
			{
				serialize(sbot, os, first);
			}
			else if (Khepera* khepera = dynamic_cast<Khepera*>(object))
			{
				serialize(khepera, os, first);
			}
			else if (Marxbot* marxbot = dynamic_cast<Marxbot*>(object))
			{
				serialize(marxbot, os, first);
			}
			else if (PhysicalObject* po = dynamic_cast<PhysicalObject*>(object))
			{
				serialize(po, os, first);
			}
			else
			{
				cerr << " ===  serialize - NOT DEFINED === " << endl;
			}
		}
	}

	void serialize(const World::GroundTexture& texture, ostream& os, bool first)
	{
		os << texture.width << TYPE_SEPARATOR
		<< texture.height << TYPE_SEPARATOR;
		
		os << texture.data.size() << TYPE_SEPARATOR;
		
		for (int i = 0; i < texture.data.size(); ++i)
		{
			os << texture.data[i] << TYPE_SEPARATOR;
		}
	}

	void serialize(Robot* r, ostream& os)
	{
		os.precision(serialize_precision);
		os.setf(std::ios::fixed);
		serialize(r->pos, os);
		os << r->angle << TYPE_SEPARATOR;
	}

	void serialize(Thymio2* thymio, ostream& os, bool first)
	{
		os.precision(serialize_precision);
		os.setf(std::ios::fixed);
		
		os << Factory::TypeObject::THYMIO2 << TYPE_SEPARATOR
		<< thymio->getId() << TYPE_SEPARATOR;
		
		serialize(static_cast<Robot*>(thymio),os);
		
		os << Thymio2::LED_COUNT << TYPE_SEPARATOR;
		for (int i = 0; i < Thymio2::LED_COUNT; ++i)
		{
			serialize(thymio->getColorLed((Thymio2::LedIndex)i), os);
		}
		
		os << OBJECT_SEPARATOR;
	}

	void serialize(EPuck* epuck, ostream& os, bool first)
	{
		os << Factory::TypeObject::EPUCK << TYPE_SEPARATOR << epuck->getId() << TYPE_SEPARATOR;
		serialize(static_cast<Robot*>(epuck), os);
		os << OBJECT_SEPARATOR;	}

	void serialize(Khepera* khepera, ostream& os, bool first)
	{
		os << Factory::TypeObject::KHEPERA << TYPE_SEPARATOR << khepera->getId() << TYPE_SEPARATOR;
		serialize(static_cast<Robot*>(khepera), os);
		os << OBJECT_SEPARATOR;
	}

	void serialize(Marxbot* marxbot, ostream& os, bool first)
	{
		os << Factory::TypeObject::MARXBOT << TYPE_SEPARATOR << marxbot->getId() << TYPE_SEPARATOR;
		serialize(static_cast<Robot*>(marxbot), os);
		os << OBJECT_SEPARATOR;
	}

	void serialize(Sbot* sbot, ostream& os, bool first)
	{
		os << Factory::TypeObject::SBOT << TYPE_SEPARATOR << sbot->getId() << TYPE_SEPARATOR;
		serialize(static_cast<Robot*>(sbot), os);
		if (first)
			serialize(sbot->getColor(),os);
		os << OBJECT_SEPARATOR;
	}

	void serialize(PhysicalObject* po, ostream& os, bool first)
	{
		os << Factory::TypeObject::PHYS_OBJ << TYPE_SEPARATOR;
		os << po->getId() << TYPE_SEPARATOR;		// id

		serialize(po->pos, os);
		os << po->angle << TYPE_SEPARATOR;
		serialize(po->getColor(), os);
		if(first)
		{
			os << po->isCylindric() << TYPE_SEPARATOR;
			if (po->isCylindric())
			{
				os << po->getRadius() << TYPE_SEPARATOR
				<< po->getHeight() << TYPE_SEPARATOR
				<< po->getMass() << TYPE_SEPARATOR;
			}
			else
			{
				serialize(po->getHull(), os);
				os << po->getMass() << TYPE_SEPARATOR;
			}
		}
		
		os << OBJECT_SEPARATOR;
	}

	void serialize(const Color& color, ostream& os)
	{
		os << color.r() << TYPE_SEPARATOR
		<< color.g() << TYPE_SEPARATOR
		<< color.b() << TYPE_SEPARATOR
		<< color.a() << TYPE_SEPARATOR;
	}

	void serialize(const Point& pos, ostream& os)
	{
		os << pos.x << TYPE_SEPARATOR << pos.y << TYPE_SEPARATOR;
	}

	void serialize(const PhysicalObject::Hull& hull, ostream& os)
	{
		os << hull.size() << TYPE_SEPARATOR;

		// a Hull is a vector of Part which are composed of a Polygone, a
		// height, and optionally of textures;.
		for (auto& poly: hull)
		{
			serialize(poly.getShape(), os);
			os << poly.getHeight() << TYPE_SEPARATOR;
			os << poly.isTextured()<< TYPE_SEPARATOR;

			if (poly.isTextured())
			{
				serialize(poly.getTextures(), os);
			}
		}
	}

	void serialize(const Polygone& shape, ostream& os)
	{
		os << shape.size() << TYPE_SEPARATOR;

		for (auto& point : shape)
		{
			serialize(point, os);
		}
	}

	void serialize(const Textures& textures, ostream& os)
	{
		os << textures.size() << TYPE_SEPARATOR;

		for (auto& texture : textures)
		{
			serialize(texture, os);
		}
	}

	void serialize(const Texture& texture, ostream& os)
	{
		os << texture.size() << TYPE_SEPARATOR;

		for (auto& color : texture)
		{
			serialize(color, os);
		}
	}

	World* initWorld(const string& str){
		vector<string> objects = split(str, OBJECT_SEPARATOR);
		World* world;
	
		std::vector<std::string> attrs = split(objects[0], TYPE_SEPARATOR);
		
		int pos = 0;
		int wallsType = stoi(attrs[pos++]);

		switch (wallsType)
		{
			case World::WALLS_SQUARE:
			{
				double w = stod(attrs[pos++]);
				double h = stod(attrs[pos++]);
				Color color = deserialize<Color>(str, &pos);
				auto gt = deserialize<World::GroundTexture>(str, &pos);
				world = new World(w, h, color, gt);
				break;
			}
			case World::WALLS_CIRCULAR:
			{
				double r = stod(attrs[pos++]);
				Color color = deserialize<Color>(str, &pos);
				auto gt = deserialize<World::GroundTexture>(str,&pos);
				world = new World(r, color, gt);
				break;
			}
			default:
				world = new World();
		}
		deserialize(world, str, true);
		return world;
	}

	static void updateObject(World* world, const std::string str, bool first)
	{
		std::vector<std::string> tabObj = split(str, TYPE_SEPARATOR);
		int type_object = stoi(tabObj[0]);
		int id = stoi(tabObj[1]);
		for (auto& obj : world->objects)
		{
			if (obj->getId() == id)
			{
				switch(type_object)
				{
					case Factory::TypeObject::THYMIO2:
					{
						Thymio2* thymio = dynamic_cast<Thymio2*>(obj);
						if (thymio != NULL)
							deserialize(thymio, str, first);
						break;
					}
					case Factory::TypeObject::MARXBOT:
					{
						Marxbot* marxbot = dynamic_cast<Marxbot*>(obj);
						if (marxbot != NULL)
							deserialize(marxbot, str, first);
						break;
					}
					case Factory::TypeObject::KHEPERA:
					{
						Khepera* khepera = dynamic_cast<Khepera*>(obj);
						if (khepera != NULL)
							deserialize(khepera, str, first);
						break;
					}
					case Factory::TypeObject::EPUCK:
					{
						EPuck* epuck = dynamic_cast<EPuck*>(obj);
						if (epuck != NULL)
							deserialize(epuck, str, first);
						break;
					}
					case Factory::TypeObject::SBOT:
					{
						Sbot* sbot = dynamic_cast<Sbot*>(obj);
						if (sbot != NULL)
							deserialize(sbot, str, first);
						break;
					}
					case Factory::TypeObject::PHYS_OBJ:
					{
						deserialize(obj, str, first);
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

	void deserialize(World* world, const string& str, bool first)
	{
		vector<string> objects = split(str, OBJECT_SEPARATOR);
		// delete world data
		if(first)
			objects.erase(objects.begin());

		for (auto& obj : objects)
		{
			if (first)
			{
				std::vector<std::string> tmpObj = split(obj, TYPE_SEPARATOR);
				int pos = 0;
				int type = stoi(tmpObj[pos++]);
				int id = stoi(tmpObj[pos++]);
				Factory factory;
				PhysicalObject* o = factory.initObject(type);
				o->setId(id);
				world->addObject(o);
			}

			updateObject(world, obj, first);
		}
	}

	void deserialize(Robot *r, const string& str, int* pos)
	{
		std::vector<std::string> attrs = split(str, TYPE_SEPARATOR);
		r->pos = deserialize<Point>(str,pos);
		r->angle = stod(attrs[*pos]); *pos += 1;
	}

	void deserialize(Thymio2* thymio, const string& str, bool first)
	{
		vector<string> attrs = split(str, TYPE_SEPARATOR);
		int position = 2;
		deserialize(static_cast<Robot*>(thymio), str, &position);
		
		int nb_led = stod(attrs[position++]);

		for (int i = 0; i < nb_led; i++)
		{
			Color led = deserialize<Color>(str, &position);
			thymio->setLedColor((Thymio2::LedIndex)i, led);
		}
	}

	void deserialize(Marxbot* marxbot, const string& str, bool first)
	{
		int position = 2;
		deserialize(static_cast<Robot*>(marxbot), str, &position);
	}

	void deserialize(Khepera* khepera, const string& str, bool first)
	{
		int position = 2;
		deserialize(static_cast<Robot*>(khepera), str, &position);
	}

	void deserialize(Sbot* sbot, const string& str, bool first)
	{
		int position = 2;
		deserialize(static_cast<Robot*>(sbot), str, &position);
		if (first)
			sbot->setColor(deserialize<Color>(str, &position));
	}

	void deserialize(EPuck* epuck, const string& str, bool first)
	{
		int position = 2;
		deserialize(static_cast<Robot*>(epuck), str, &position);	}

	void deserialize(PhysicalObject* po, const string& str, bool first)
	{
		vector<string> attrs = split(str, TYPE_SEPARATOR);
		int position = 1;
		po->setId(stoi(attrs[position++]));
		po->pos = deserialize<Point>(str, &position);
		po->angle = stod(attrs[position++]);

		if(first)
		{
			po->setColor(deserialize<Color>(str, &position));

			int isCylindric = stoi(attrs[position++]);

			if (isCylindric)
			{
				double radius = stod(attrs[position++]);
				double height = stod(attrs[position++]);
				double mass = stod(attrs[position++]);
				po->setCylindric(radius, height, mass);
			}
			else
			{
				PhysicalObject::Hull hull = deserialize<PhysicalObject::Hull>(str, &position);
				double mass = stod(attrs[position++]);
				po->setCustomHull(hull, mass);
			}
		}
	}

	template<>
	PhysicalObject::Hull deserialize(const string& str, int* pos)
	{
		vector<std::string> parts = split(str, TYPE_SEPARATOR);
		PhysicalObject::Hull hull;
		
		int nb_hull = stoi(parts[*pos]); *pos += 1;
		
		for (int i = 0; i < nb_hull; ++i)
		{
			Polygone shape = deserialize<Polygone>(str, pos);
			
			double height = stod(parts[*pos]); *pos += 1;
			int isTextured = stoi(parts[*pos]); *pos += 1;

			if (isTextured)
			{
				Textures textures = deserialize<Textures>(str, pos);
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
	World::GroundTexture deserialize(const string& str, int* pos)
	{
		vector<string> attrs = split(str, TYPE_SEPARATOR);

		double height = stod(attrs[*pos]); *pos += 1;
		double width = stod(attrs[*pos]); *pos += 1;

		int nb_data = stoi(attrs[*pos]); *pos += 1;

		std::vector<uint32_t> data;
		for (int i = 0; i < nb_data; ++i)
		{
			data.push_back(stod(attrs[*pos])); *pos += 1;
		}
		return World::GroundTexture(width, height, data.data());
	}

	template<>
	Polygone deserialize(const string& str, int *pos)
	{
		vector<string> points = split(str, TYPE_SEPARATOR);
		Polygone shape;
		
		int nb_polygone = stoi(points[*pos]); *pos += 1;
		
		for (int j = 0; j < nb_polygone; ++j)
		{
			shape << deserialize<Point>(str, pos);
		}
		return shape;
	}

	template<>
	Textures deserialize(const string& str, int *pos)
	{
		vector<string> attrs = split(str, TYPE_SEPARATOR);
		Textures textures;
		
		int nb_texture = stoi(attrs[*pos]); *pos += 1;
		
		for (int j = 0; j < nb_texture; ++j)
		{
			textures.push_back(deserialize<Texture>(str, pos));
		}
		return textures;
	}

	template<>
	Texture deserialize(const string& str, int *pos)
	{
		vector<string> attrs = split(str, TYPE_SEPARATOR);
		Texture texture;
		
		int nb_color = stoi(attrs[*pos]); *pos += 1;

		for (int k = 0; k < nb_color; ++k)
		{
			texture.push_back(deserialize<Color>(str, pos));
		}
		return texture;
	}

	template<>
	Color deserialize(const string& str, int *pos)
	{
		vector<string> attrs = split(str, TYPE_SEPARATOR);
		double r = stod(attrs[*pos]); *pos += 1;
		double g = stod(attrs[*pos]); *pos += 1;
		double b = stod(attrs[*pos]); *pos += 1;
		double a = stod(attrs[*pos]); *pos += 1;
		return Color(r, g, b, a);
	}

	template<>
	Point deserialize(const string& str, int* pos)
	{
		vector<string> attrs = split(str, TYPE_SEPARATOR);
		double x = stod(attrs[*pos]); *pos += 1;
		double y = stod(attrs[*pos]); *pos += 1;
		return Point(x, y);
	}

}
