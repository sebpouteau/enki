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

#include <enki/Factory.h>
#include "robots/thymio2/Thymio2.h"
#include "robots/s-bot/Sbot.h"
#include "robots/e-puck/EPuck.h"
#include "robots/khepera/Khepera.h"
#include "robots/marxbot/Marxbot.h"

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

namespace Enki
{
	// Number of digits after the decimal point for double.
	static const int serialize_precision = 2;// Precision of a double (2 digits after the decimal point).
	
	const char OBJECT_SEPARATOR = ':';
	const char TYPE_SEPARATOR = ';';

	std::vector<std::string> split(const std::string& str, const char separator);

	//! Return the string serialization of world
	std::string serialize(World *world, bool first = false);
	//! Update the world with the string serialization.
	void deserializeUdpate(World* world, const std::string& str, bool first = false);

	//! Serialize the world and add the string serialization to the os
	void serialize(World* world, std::ostream& os, bool first = false);
	//! Serialize the texture and add the string serialization to the os
	void serialize(const World::GroundTexture& texture, std::ostream& os, bool first = false);
	//! Serialize objects and add the string serialization to the os
	void serialize(std::set<PhysicalObject*> objects, std::ostream& os, bool first = false);
	//! Serialize the robot and add the the string serialization to the os
	void serialize(Robot* r, std::ostream& os);
	//! Serialize the Thymio and add the string serialization to the os
	void serialize(Thymio2* thymio, std::ostream& os, bool first = false);
	//! Serialize the Epuck and add the string serialization to the os
	void serialize(EPuck* thymio, std::ostream& os, bool first = false);
	//! Serialize the Maxrbot and add the string serialization to the os
	void serialize(Marxbot* thymio, std::ostream& os, bool first = false);
	//! Serialize the Sbot and add the string serialization to the os
	void serialize(Sbot* thymio, std::ostream& os, bool first = false);
	//! Serialize the Khepra and add the string serialization to the os
	void serialize(Khepera* thymio, std::ostream& os, bool first = false);
	//! Serialize the PhysicalObject and add the string serialization to the os
	void serialize(PhysicalObject* po, std::ostream& os, bool first = false);
	//! Serialize the Color and add the string serialization to the os
	void serialize(const Color& color, std::ostream& os);
	//! Serialize the Point and add the string serialization to the os
	void serialize(const Point& pos, std::ostream& os);
	//! Serialize the Point and add the string serialization to the os
	void serialize(const PhysicalObject::Hull& hull, std::ostream& os);
	//! Serialize the Polygone and add the string serialization to the os
	void serialize(const Polygone& poly, std::ostream& os);
	//! Serialize the textures and add the string serialization to the os
	void serialize(const Textures& textures, std::ostream& os);
	//! Serialize the texture and add the string serialization to the os
	void serialize(const Texture& texture, std::ostream& os);

	//! Generic template for deserialization.
	template<typename T>
	T deserialize(const std::string& str, int* pos);

	//! Return a World without objects with the string serialization.
	World* initWorld(const std::string& str);
	//! Deserialize information of world create objects if first == true, update objects if first == false
	void deserialize(World* world, const std::string& str, bool first);
	//! Return a GroundTexture with the string serialization.
	template<>
	World::GroundTexture deserialize(const std::string& str, int* pos);
	//! Deserialize the Robot R from string STR with separator SEP.
	void deserialize(Robot* r, const std::string& str);
	//! Update a Thymio with the string serialization.
	void deserialize(Thymio2* thymio, const std::string& str, bool first = false);
	//! Update a marxbot with the string serialization.
	void deserialize(Marxbot* marxbot, const std::string& str, bool first = false);
	//! Update a EPuck with the string serialization.
	void deserialize(EPuck* epuck, const std::string& str, bool first = false);
	//! Update a Khepera with the string serialization.
	void deserialize(Khepera* khepera, const std::string& str, bool first = false);
	//! Update a Sbot with the string serialization.
	void deserialize(Sbot* sbot, const std::string& str, bool first = false);
	//! Update a PhysicalObject with the string serialization.
	void deserialize(PhysicalObject* po, const std::string& str, bool fisrt = false);
	//! Return a Color with the string serialization.
	template<>
	Color deserialize(const std::string& str, int* pos);
	//! Return a Point with the string serialization.
	template<>
	Point deserialize(const std::string& str, int* pos);
	//! Return a Hull with the string serialization.
	template<>
	PhysicalObject::Hull deserialize(const std::string& str, int* pos);
	//! Return a Polygone with the string serialization.
	template<>
	Polygone deserialize(const std::string& str, int* pos);
	//! Return a vector of Texture with the string serialization.
	template<>
	Textures deserialize(const std::string& str, int* pos);
	//! Return a Texture with the string serialization.
	template<>
	Texture deserialize(const std::string& str, int* pos);
}

#endif // SERIALIZE_H
