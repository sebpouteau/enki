//
//  factory.cpp
//  enki
//
//  Created by Sébastien POUTEAU on 09/03/2017.
//
//

#include "Factory.h" 
#include "Robots/Thymio2/Thymio2.h"
#include "Robots/s-bot/Sbot.h"
#include "Robots/e-puck/EPuck.h"
#include "Robots/khepera/Khepera.h"
#include "Robots/marxbot/Marxbot.h"


Enki::PhysicalObject* Factory::initObject(const std::vector<std::string> tabObj) const{
		
		int pos = 0;
		int type_object = stod(tabObj[pos++]);
		switch (type_object) {
			case (int)TypeObject::THYMIO2:
			{
                return new Enki::Thymio2();
			}
			case (int)TypeObject::SBOT:
			{
				return new Enki::Sbot();
			}
			case (int)TypeObject::MARXBOT:
			{
				return new Enki::Marxbot();
			}
			case (int)TypeObject::KHEPERA:
			{
				return new Enki::Khepera();
			}
			case (int)TypeObject::EPUCK:
			{
				return new Enki::EPuck;
			}
			case (int)TypeObject::PHYS_OBJ:
				return new Enki::PhysicalObject();
				
			default:
				return new Enki::PhysicalObject();
		}
	}
