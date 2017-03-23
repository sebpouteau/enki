/*
 Enki - a fast 2D robot simulator
 Copyright © 2017 Sébastien Pouteau <sebastien.pouteau@etu.u-bordeaux.fr>

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Factory.h"


Enki::PhysicalObject* Factory::initObject(int type) const
{
	switch (type)
	{
		case (int)ROBOT_TYPES::THYMIO2:
		{
			return new Enki::Thymio2();
		}
		case (int)ROBOT_TYPES::SBOT:
		{
			return new Enki::Sbot();
		}
		case (int)ROBOT_TYPES::MARXBOT:
		{
			return new Enki::Marxbot();
		}
		case (int)ROBOT_TYPES::KHEPERA:
		{
			return new Enki::Khepera();
		}
		case (int)ROBOT_TYPES::EPUCK:
		{
			return new Enki::EPuck();
		}
		case (int)ROBOT_TYPES::PHYSICAL_OBJECT:
		{
			return new Enki::PhysicalObject();
		}
		default:
		{
			return new Enki::PhysicalObject();
		}
	}
}
