/*
 Enki - a fast 2D robot simulator
 Copyright © 2017 Jimmy Gouraud <jimmy.gouraud@etu.u-bordeaux.fr>
 
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

//#include <iomanip>      // std::setprecision
#include "Geometry.h"
#include <sstream>
#include <iostream>

/*! \file Serialize.h
 \brief The serialization & deserialization
 
 For the serialization:
 - complex objects ends with OBJECT_SEPARATOR = ':'
 eg: World:Thymio:PhysicalObject:...:
 - basic types (int, float, double, etc.) ends with TYPE_SEPARATOR = ';'
 eg: int;float;double;
 */

#define PRECISION 2 // Precision of a double (3 digits after the decimal point).


const char OBJECT_SEPARATOR = ':';
const char TYPE_SEPARATOR = ';';

namespace Enki
{

    // ==== Point ====
    void serializePoint(const Point &pos, std::ostringstream* oss);

    // ===== Practical function =====
    // ==============================
    std::vector<std::string> findSeparator(const std::string& str, const char& separator);

}






#endif // SERIALIZE_H
