/*
  Enki - a fast 2D robot simulator
  Copyright © 2017 Jimmy Gouraud <jimmy.gouraud@etu.u-bordeaux.fr>

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

#include "catch.hpp"

#include <enki/PhysicalEngine.h>
#include <enki/robots/thymio2/Thymio2.h>
#include <enki/robots/e-puck/EPuck.h>
#include <enki/robots/s-bot/Sbot.h>
#include <enki/robots/khepera/Khepera.h>
#include <enki/robots/marxbot/Marxbot.h>

#include <enki/Serialize.h>
#include <enki/worldgenerator/WorldGenerator.h>

#define NB_ITERATIONS 20
#define NB_OBJECTS 5
#define NB_HULL 10

using namespace Enki;
using namespace std;

const double EPSILON = pow(10, -PRECISION);

static Robot* createRobot(int type)
{	
	switch (type)
	{
	case Randomizer::THYMIO2_:
		return new Thymio2();
	case Randomizer::EPUCK_:
		return new EPuck();
	case Randomizer::SBOT_:
		return new Sbot();
	case Randomizer::MARXBOT_:
		return new Marxbot();
	case Randomizer::KHEPERA_:
		return new Khepera();
	default:
		return new Thymio2();
	}
}

static bool equalsFloat(float f1, float f2) {
	return fabs(f1 - f2) < EPSILON;
}

static bool equalsPoint(const Point& p1, const Point& p2)
{
	return (fabs(p1.x - p2.x) < EPSILON &&
			fabs(p1.y - p2.y) < EPSILON);
}

static bool equalsColor(const Color& c1, const Color& c2)
{
	return (fabs(c1.r() - c2.r()) < EPSILON &&
			fabs(c1.g() - c2.g()) < EPSILON &&
			fabs(c1.b() - c2.b()) < EPSILON &&
			fabs(c1.a() - c2.a()) < EPSILON);
}

static bool equalsRobots(const Robot& r1, const Robot& r2)
{
	if (!equalsPoint(r1.pos, r2.pos)) {
		cerr << "[Robot] Not the same position : "
			 << "pos1 = " << r1.pos << " / "
			 << "pos2 = " << r2.pos << endl;
		return false;
	}

	if (!equalsFloat(r1.angle, r2.angle)) {
		cerr << "[Robot] Not the same angle : "
			 << "angle1 = " << r1.angle << " / "
			 << "angle2 = " << r2.angle << endl;
		return false;
	}
	
	return true;
}

static bool equalsThymio(const Thymio2& t1, const Thymio2& t2)
{
	equalsRobots(t1, t2);
	
	for (unsigned int i = 0; i < Thymio2::LED_COUNT; ++i) {
		if (!equalsColor(t1.getColorLed((Thymio2::LedIndex)i),
						 t2.getColorLed((Thymio2::LedIndex)i))) {			
			cerr << "[Thymio2] Not the same color : "
				 << "color1 = " << t1.getColorLed((Thymio2::LedIndex)i) << " / "
				 << "color2 = " << t2.getColorLed((Thymio2::LedIndex)i) << endl;
			return false;
		}
	}
	
	return true;
}

static bool equalsPhysObjBase(const PhysicalObject& po1, const PhysicalObject& po2)
{	
	if (!equalsPoint(po1.pos, po2.pos)) {
		cerr << "[PO] Not the same position : "
			 << "pos1 = " << po1.pos << " / "
			 << "pos2 = " << po2.pos << endl;
		return false;
	}

	if (fabs(po1.angle - po2.angle) > EPSILON) {
		cerr << "[PO] Not the same angle : "
			 << "angle1 = " << po1.angle << " / "
			 << "angle2 = " << po2.angle << endl;
		return false;
	}
	
	return true;
}

static bool equalsPhysObj(const PhysicalObject& po1, const PhysicalObject& po2)
{
	if (!equalsPhysObjBase(po1, po2)) {
		return false;
	}
	
	if (!equalsColor(po1.getColor(), po2.getColor())) {
		cerr << "[PO] Not the same color : "
			 << "color1 = " << po1.getColor() << " / "
			 << "color2 = " << po2.getColor() << endl;
		return false;
	}
	
	if (po1.isCylindric() != po2.isCylindric()) {
		cerr << "[PO] Not the same shape : "
			 << "po1.isCylindric() = " << po1.isCylindric() << " "
			 << "po2.isCylindric() = " << po2.isCylindric() << endl;
		return false;
	}

	if (po1.isCylindric() && po2.isCylindric()) {
		if (fabs(po1.getRadius() - po2.getRadius()) > EPSILON) {
			cerr << "[PO] Not the same radius : "
				 << "radius1 = " << po1.getRadius() << " "
				 << "radius2 = " << po2.getRadius() << endl;
			return false;
		}
		
		if (fabs(po1.getHeight() - po2.getHeight()) > EPSILON) {
			cerr << "[PO] Not the same height "
			 << "height1 = " << po1.getHeight() << " "
			 << "height2 = " << po2.getHeight() << endl;
			return false;
		}
	}

	if (!po1.isCylindric() && !po2.isCylindric()) {
		const PhysicalObject::Hull& hull1 = po1.getHull();
		const PhysicalObject::Hull& hull2 = po2.getHull();
		if (hull1.size() != hull2.size()) {
			cerr << "[PO] Not the same  hull size "
				 << "size1 = " << hull1.size() << " "
				 << "size2 = " << hull2.size() << endl;
			return false;
		}

		for (int i = 0; i < hull1.size(); ++i) {
			const Polygone& shape1 = hull1[i].getShape();
			const Polygone& shape2 = hull2[i].getShape();

			if (shape1.size() != shape2.size()){
				cerr << "[PO] Not the same shape size "
					 << "size1 = " << shape1.size() << " "
					 << "size2 = " << shape2.size() << endl;
				return false;
			}
			
			for (size_t j = 0; j < shape1.size(); ++j) {
				if (!equalsPoint(shape1[j], shape2[j])) {
					cerr << "[PO] Not the same position shape: "
						 << "pos1 = " << shape1[j] << " / "
						 << "pos2 = " << shape2[j] << endl;
					return false;
				}
			}
			
			if (fabs(hull1[i].getHeight() - hull2[i].getHeight()) > EPSILON) {
				cerr << "[PO] Not the same height size: "
					 << "pos1 = " << hull1[i].getHeight() << " / "
					 << "pos2 = " << hull2[i].getHeight() << endl;
				return false;
			}

			if (hull1[i].isTextured() != hull2[i].isTextured()){
				cerr << "[PO] Not the same texture of hull :"
					 << "hull1.isTextured() = " << hull1[i].isTextured() << " / "
					 << "hull2.isTextured() = " << hull2[i].isTextured() << endl;
				return false;
			}

			if (hull1[i].isTextured()) {
				const Textures& textures1 = hull1[i].getTextures();
				const Textures& textures2 = hull2[i].getTextures();
				
				if (textures1.size() != textures2.size()){
					cerr << "[PO] Not the same textures size : "
						 << "size1 = " << textures2.size() << " / "
						 << "size2 = " << textures1.size() << endl;
					return false;
				}
				
				for (size_t j = 0; j < textures1.size(); ++j) {
					Texture texture1 = textures1[j];
					Texture texture2 = textures2[j];
					
					if (texture1.size() != texture2.size()){
						cerr << "[PO] Not the same texture size : "
							 << "size1 = " << texture2.size() << " / "
							 << "size2 = " << texture1.size() << endl;
						return false;
					}

					for (size_t k = 0; k < texture1.size(); ++k) {
						if (!equalsColor(texture1[k], texture2[k])){
							cerr << "[PO] Not the same texture color : "
								 << "color1 = " << texture1[k] << " / "
								 << "color2 = " << texture2[k] << endl;
							return false;
						}
					}
				}
			}
		}
	}
	
	if (fabs(po1.getMass() - po2.getMass()) > EPSILON) {
		cerr << "[PO] Not the same mass" << endl;
		return false;
	}
	
	return true;
}

static bool equalsWorld(const World& w1, const World& w2)
{
	if (w1.wallsType != w2.wallsType)	{
		cerr << "[World] Not the same wallsType : "
			 << "wallsType1 : " << w1.wallsType << " / "
			 << "wallsType2 : " << w2.wallsType << endl;
		return false;
	}
	
	if ( (w1.h != w2.h) || (w1.w != w2.w) ) {
		cerr << "[World] Not the same width : "
			 << "width1 : "  << w1.w << " / "
			 << "width2 : "  << w2.w << " | "
			 << "height1 : " << w1.h << " / "
			 << "height2 : " << w2.h << endl;
		return false;
	}
	
	if (w1.objects.size() != w2.objects.size()) {
		cerr << "[World] Not the same number of objects : "
			 << "nbObject1 : " << w1.objects.size() << " / "
			 << "nbObject2 : " << w2.objects.size() << endl;
		return false;
	}
	
	for (auto& obj1 : w1.objects) {
		for (auto& obj2 : w2.objects) {
			if (obj1->getId() == obj2->getId()) {
				if (Thymio2* thymio1 = dynamic_cast<Thymio2*>(obj1)) {
					if (Thymio2* thymio2 = dynamic_cast<Thymio2*>(obj2)) {
						if (!equalsThymio(*thymio1, *thymio2)) {
							cerr << "[World] Not same Thymio in both Worlds" << endl;
							return false;
						}
						continue;
					} else {
						cerr << "[World] Not same Object in both Worlds" << endl;
						return false;
					}
				} else if (EPuck* epuck1 = dynamic_cast<EPuck*>(obj1)) {
					if (EPuck* epuck2 = dynamic_cast<EPuck*>(obj2)) {
						if (!equalsRobots(*epuck1, *epuck2)) {
							cerr << "[World] Not same EPuck in both Worlds" << endl;
							return false;
						}
						continue;
					} else {
						cerr << "[World] Not same Object in both Worlds" << endl;
						return false;
					}
				} else if (Sbot* sbot1 = dynamic_cast<Sbot*>(obj1)) {
					if (Sbot* sbot2 = dynamic_cast<Sbot*>(obj2)) {
						if (!equalsRobots(*sbot1, *sbot2)) {
							cerr << "[World] Not same Sbot in both Worlds" << endl;
							return false;
						}
						continue;
					} else {
						cerr << "[World] Not same Object in both Worlds" << endl;
						return false;
					}
				} else if (Khepera* khepera1 = dynamic_cast<Khepera*>(obj1)) {
					if (Khepera* khepera2 = dynamic_cast<Khepera*>(obj2)){
						if (!equalsRobots(*khepera1, *khepera2)) {
							cerr << "[World] Not same Khepera in both Worlds" << endl;
							return false;
						}
						continue;
					} else {
						cerr << "[World] Not same Object in both Worlds" << endl;
						return false;
					}
				} else if (Marxbot* marxbot1 = dynamic_cast<Marxbot*>(obj1)) {
					if (Marxbot* marxbot2 = dynamic_cast<Marxbot*>(obj2)) {
						if (!equalsRobots(*marxbot1, *marxbot2)) {
							cerr << "[World] Not same Marxbot in both Worlds" << endl;
							return false;
						}
						continue;
					} else {
						cerr << "[World] Not same Object in both Worlds" << endl;
						return false;
					}
				} else if (PhysicalObject* po1 = dynamic_cast<PhysicalObject*>(obj1)) {
					if (PhysicalObject* po2 = dynamic_cast<PhysicalObject*>(obj2)) {
						if (!equalsPhysObj(*po1, *po2)) {
							cerr << "[World] Not same PhysicalObject in both Worlds" << endl;
							return false;
						}
						continue;
					} else {
						cerr << "[World] Not same Object in both Worlds" << endl;
						return false;
					}
				} else {
					cerr << "[World] Object not defined " << endl;
					return false;
				}
			}
		}
	}

	return true;
}


TEST_CASE( "Robustness Test Serialization", "[RT Serialization/Deserialization]" ) {
	Randomizer random;

	SECTION("RT Color") {
		Color color = random.randColor();

		for (int i = 0; i < NB_ITERATIONS; ++i) {
            std::ostringstream oss;

            serialize(color, oss, Separator::a);
            Color color1 = deserialize<Color>(oss.str(), Separator::a);
			REQUIRE(equalsColor(color, color1));

            Color color2 = deserialize<Color>(oss.str(), Separator::a);
			REQUIRE(equalsColor(color, color2));

			color = color1;
		}
	}

	SECTION("RT Thymio Init") {
		Thymio2* thymio = random.randThymio();

		for (int i = 0; i < NB_ITERATIONS; ++i) {
            std::ostringstream oss;

            serialize(thymio, oss, Separator::a, true);
			
			Thymio2* thymio1 = new Thymio2();
			deserialize(thymio1, oss.str(),Separator::a, true);
			REQUIRE(equalsThymio(*thymio, *thymio1));
			
			Thymio2* thymio2 = new Thymio2();
            deserialize(thymio2, oss.str(),Separator::a, true);
			REQUIRE(equalsThymio(*thymio, *thymio2));
			
			delete thymio;
			delete thymio2;
			thymio = thymio1;
		}
		
		delete thymio;
	}

	SECTION("RT Thymio Update") {
		Thymio2* thymio = random.randThymio();

		for (int i = 0; i < NB_ITERATIONS; ++i) {
            std::ostringstream oss;

            serialize(thymio, oss, Separator::a, false);

			Thymio2* thymio1 = new Thymio2();
            deserialize(thymio1, oss.str(),Separator::a, false);
			REQUIRE(equalsThymio(*thymio, *thymio1));
			
			Thymio2* thymio2 = new Thymio2();
            deserialize(thymio2, oss.str(),Separator::a, false);
			REQUIRE(equalsThymio(*thymio, *thymio2));
			
			delete thymio;
			delete thymio2;
			thymio = thymio1;
		}
		
		delete thymio;
	}
	
	SECTION("RT Robot Init") {
		int robot_type = random.randInt(1, 4); // without Thymio2
		Robot* robot = random.randRobot(robot_type);

		for (int i = 0; i < NB_ITERATIONS; ++i) {
            std::ostringstream oss;

            serialize(robot, oss, Separator::a, true);
			
			Robot* robot1 = createRobot(robot_type);
            deserialize(robot1, oss.str(), Separator::a, true);
			REQUIRE(equalsRobots(*robot, *robot1));
			
			Robot* robot2 = createRobot(robot_type);
            deserialize(robot2, oss.str(), Separator::a, true);
			REQUIRE(equalsRobots(*robot, *robot2));
			
			delete robot;
			delete robot2;
			robot = robot1;
		}
		
		delete robot;
	}

	SECTION("RT Robot Update") {
		int robot_type = random.randInt(1, 4); // without Thymio2
		Robot* robot = random.randRobot(robot_type);

		for (int i = 0; i < NB_ITERATIONS; ++i) {
            std::ostringstream oss;

            serialize(robot, oss, Separator::a, false);

			Robot* robot1 = createRobot(robot_type);
            deserialize(robot1, oss.str(), Separator::a, false);
			REQUIRE(equalsRobots(*robot, *robot1));
			
			Robot* robot2 = createRobot(robot_type);
            deserialize(robot2, oss.str(), Separator::a, false);
			REQUIRE(equalsRobots(*robot, *robot2));
			
			delete robot;
			delete robot2;
			robot = robot1;
		}
		
		delete robot;
	}
	
	SECTION("RT Physical Object Init") {
		for (int i = 0; i < NB_HULL + 1; i++) {
			PhysicalObject* po = random.randPhysicalObject(i);

			for (int i = 0; i < NB_ITERATIONS; ++i) {
                std::ostringstream oss;

                serialize(po, oss, Separator::a, true);

				PhysicalObject* po1 = new PhysicalObject();
                deserialize(po1, oss.str(), Separator::a, true);
				REQUIRE(equalsPhysObj(*po, *po1));
				
				PhysicalObject* po2 = new PhysicalObject();
                deserialize(po2, oss.str(), Separator::a, true);
				REQUIRE(equalsPhysObj(*po, *po2));
				
				delete po;
				delete po2;
				po = po1;
			}
   
			delete po;
		}
	}

	SECTION("RT Physical Object Update") {
		PhysicalObject* po = random.randPhysicalObject();

		for (int i = 0; i < NB_ITERATIONS; ++i) {
            std::ostringstream oss;

            serialize(po, oss, Separator::a, false);

			PhysicalObject* po1 = new PhysicalObject();
            deserialize(po1, oss.str(), Separator::a, false);
			REQUIRE(equalsPhysObjBase(*po, *po1));
				
			PhysicalObject* po2 = new PhysicalObject();
            deserialize(po2, oss.str(), Separator::a, false);
			REQUIRE(equalsPhysObjBase(*po, *po2));
				
			delete po;
			delete po2;
			po = po1;
		}
   
		delete po;
	}

	SECTION("RT World Init") {
		WorldGenerator* gen = new WorldGenerator();
		gen->add(ANYTHING_, NB_OBJECTS);
        World* world = initWorld(serialize(gen->getWorld(), true));
        delete gen;

		for (int i = 0; i < NB_ITERATIONS; ++i) {
			string str = serialize(world, true);

			World* world1 = initWorld(str);
			REQUIRE(equalsWorld(*world, *world1));

			World* world2 = initWorld(str);
			REQUIRE(equalsWorld(*world, *world2));

			delete world;
			delete world2;
			world = world1;
		}
		delete world;
	}
	
	SECTION("RT World Update") {
		WorldGenerator* gen = new WorldGenerator();
		gen->add(ANYTHING_, NB_OBJECTS);
		World* world = gen->getWorld();
		
		string strInit = serialize(world, true);
		World* world1 = initWorld(strInit);
		World* world2 = initWorld(strInit);
		
		for (int i = 0; i < NB_ITERATIONS; ++i) {
			gen->genStep();

			string strUpdate = serialize(world, false);
            deserialize(world1, strUpdate, Separator::a, false);
			REQUIRE(equalsWorld(*world, *world1));

            deserialize(world2, strUpdate, Separator::a, false);
			REQUIRE(equalsWorld(*world, *world2));
		}

		delete gen;
		delete world1;
		delete world2;
	}
}
