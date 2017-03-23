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

#ifndef __FACTORY_H
#define __FACTORY_H

#include <stdio.h>
#include "PhysicalEngine.h"
#include "robots/thymio2/Thymio2.h"
#include "robots/s-bot/Sbot.h"
#include "robots/e-puck/EPuck.h"
#include "robots/khepera/Khepera.h"
#include "robots/marxbot/Marxbot.h"


class Factory
{
public:
	enum ROBOT_TYPES { PHYSICAL_OBJECT, THYMIO2, EPUCK, SBOT, MARXBOT, KHEPERA  };

	Enki::PhysicalObject* initObject(int type) const;
};

#endif /* Factory.h */
