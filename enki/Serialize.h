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

#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <enki/robots/thymio2/Thymio2.h>

/*!	\file Serialize.h
	\brief The serialization & deserialization

	For the serialization the data is organized in a tree where the separator is
	used to indicate which level a node beyond.  This technique allows parsing
	the strings recursively easily.  The current limitation is that there is a
	limited deepness for the tree.

	At first level we found the world characterics and the objects it contains.
	At second level we find data members sub members and it continues like this
	with an arbitrary limit of Separator::size levels.
*/

enum ROBOT_TYPES { WORLD, THYMIO2, PHYSICAL_OBJECT };

namespace Enki
{
	// Number of digits after the decimal point for double.
	const int PRECISION = 2;

	enum class Separator : char
	{
		a = ':',
		b = ';',
		c = '&',
		d = '#',
		e = '~',
		f = '|',
		g = '-',
		h = '_',
		i = '^',
		j = '@',
		size
	};

	//! Return the actual char associated with SEP.
	inline char operator*(const Separator sep);
	//! Return the next separator after SEP.
	Separator next(const Separator sep);

	//! Return the string serialization of world
	std::string serialize(World *world);
	//! Update the world with the string serialization.
	void deserializeUdpate(World* world, const std::string& str);

	//! Serialize the world and add the string serialization to the os
	void serialize(World* world, std::ostream& os,
				   Separator sep = Separator::a);
	//! Serialize the texture and add the string serialization to the os
	void serialize(const World::GroundTexture& texture, std::ostream& os,
				   Separator sep = Separator::a);
	//! Serialize objects and add the string serialization to the os
	void serialize(std::set<PhysicalObject*> objects, std::ostream& os,
				   Separator sep = Separator::a);
	//! Serialize the Thymio and add the string serialization to the os
	void serialize(Thymio2* thymio, std::ostream& os,
				   Separator sep = Separator::a);
	//! Serialize the PhysicalObject and add the string serialization to the os
	void serialize(PhysicalObject* po, std::ostream& os,
				   Separator sep = Separator::a);
	//! Serialize the Color and add the string serialization to the os
	void serialize(const Color& color, std::ostream& os,
				   Separator sep = Separator::a);
	//! Serialize the Point and add the string serialization to the os
	void serialize(const Point& pos, std::ostream& os,
				   Separator sep = Separator::a);
	//! Serialize the Point and add the string serialization to the os
	void serialize(const PhysicalObject::Hull& hull, std::ostream& os,
				   Separator sep = Separator::a);
	//! Serialize the Polygone and add the string serialization to the os
	void serialize(const Polygone& poly, std::ostream& os,
				   Separator sep = Separator::a);
	//! Serialize the textures and add the string serialization to the os
	void serialize(const Textures& textures, std::ostream& os,
				   Separator sep = Separator::a);
	//! Serialize the texture and add the string serialization to the os
	void serialize(const Texture& texture, std::ostream& os,
				   Separator sep = Separator::a);

	//! Generic template for deserialization.
	template<typename T>
	T deserialize(const std::string& str, Separator sep = Separator::a);

	//! Return a World without objects with the string serialization.
	template<>
	World* deserialize(const std::string& str, Separator sep);
	//! Return a GroundTexture with the string serialization.
	template<>
	World::GroundTexture deserialize(const std::string& str, Separator sep);
	//! Return a Thymio with the string serialization.
	void deserialize(Thymio2* thymio, const std::string& str, Separator sep = Separator::b);
	//! Return a PhysicalObject with the string serialization.
	void deserialize(PhysicalObject* po, const std::string& str, Separator sep = Separator::b);
	//! Return a Color with the string serialization.
	template<>
	Color deserialize(const std::string& str, Separator sep);
	//! Return a Point with the string serialization.
	template<>
	Point deserialize(const std::string& str, Separator sep);
	//! Return a Hull with the string serialization.
	template<>
	PhysicalObject::Hull deserialize(const std::string& str, Separator sep);
	//! Return a Polygone with the string serialization.
	template<>
	Polygone deserialize(const std::string& str, Separator sep);
	//! Return a vector of Texture with the string serialization.
	template<>
	Textures deserialize(const std::string& str, Separator sep);
	//! Return a Texture with the string serialization.
	template<>
	Texture deserialize(const std::string& str, Separator sep);
}

#endif // SERIALIZE_H
