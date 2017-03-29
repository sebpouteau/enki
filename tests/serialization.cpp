/*
Enki - a fast 2D robot simulator
Copyright © 2017 Nicolas Palard <nicolas.palard@etu.u-bordeaux.fr>

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
#include <enki/robots/marxbot/Marxbot.h>
#include <enki/robots/khepera/Khepera.h>
#include <enki/Serialize.h>
#include <enki/worldgenerator/WorldGenerator.h>

using namespace Enki;

#define NB_ITERATIONS 10

static const double EPSILON = pow(10, -PRECISION);

std::vector<std::string>& splitAll(const std::string& str){
	std::vector<std::string>* s = new std::vector<std::string>;
	int pos = -1;
	bool f = true;
	while(f || pos != std::string::npos)
	{
		int deb = pos + 1;
		pos =  std::string::npos;
		Separator sep = Separator::a;
		while ( sep != Separator::size){
			if (pos < 0 ){
				pos = str.find_first_of(*sep, deb);
			}
			else
				pos = fmin (pos, str.find_first_of(*sep, deb));
			sep = sep_next(sep);

		}
		std::string c = str.substr(deb, pos - deb);
		if (c != "" )
			s->push_back(c);
		f=false;
	}
	// delete blank end
	//s->pop_back();

	return *s;
}


static int checkPoint(std::vector<std::string> &string, Point p, int start)
{
	int indice = start;
	REQUIRE ( fabs(stod(string[indice++]) - p.x) <= EPSILON );
	REQUIRE ( fabs(stod(string[indice++]) - p.y) <= EPSILON );
	return indice;
}


static int checkColor(std::vector<std::string> &string, Color c, int start)
{
	for(int i = start; i < start+4; i++)
	{
		REQUIRE( fabs(stod(string[i]) - c[i-start]) <= EPSILON );
	}

	return start + 4;
}

static int checkGroundTexture(std::vector<std::string> &string, World::GroundTexture gt, int start)
{
	int indice = start;
	REQUIRE( fabs(stod(string[indice++]) - gt.width) <= EPSILON );
	REQUIRE( fabs(stod(string[indice++]) - gt.height) <= EPSILON );
	for(int i = 0 ; i < gt.width * gt.height ; i++)
	{
		REQUIRE( fabs(stod(string[i + indice]) - gt.data[i]) <= EPSILON );
	}

	return indice + gt.width * gt.height;
}

static int checkPolygone(std::vector<std::string> &string, Polygone poly, int start)
{
	int indice = start;
	for (int i = 0; i < poly.size(); i++)
	{
		checkPoint(string, poly[i], indice+i*2);
	}

	return indice + poly.size() * 2;
}

static int checkHull(std::vector<std::string> &string, PhysicalObject::Hull hull, int start)
{
	int indice = start;

	for (PhysicalObject::Part p : hull)
	{
		indice = checkPolygone(string, p.getShape(), indice);
		REQUIRE ( fabs(stod(string[indice++]) - p.getHeight()) <= EPSILON );
		REQUIRE ( stod(string[indice++]) == p.isTextured() );

		if (p.isTextured())
		{
			const Textures& textures = p.getTextures();
			for(int i = 0 ; i < textures.size() ; i++)
			{
				Texture t = textures[i];
				for(int j = 0 ; j < t.size() ; j++)
				{
					indice = checkColor(string, t[j], indice);
				}
			}
		}
	}
	return indice;
}

static int checkRobot(std::vector<std::string> &string, const Robot &r, int start)
{
	int indice = start;
	indice = checkPoint(string, r.pos, indice);
	REQUIRE ( fabs(stod(string[indice++]) - r.angle) <= EPSILON );

	return indice;
}


static int checkThymio(std::vector<std::string> &string, const Thymio2 &t, int start)
{
	int indice = start;
	REQUIRE ( stod(string[indice++]) ==  static_cast<int>(Factory::ROBOT_TYPES::THYMIO2) );
	REQUIRE ( stod(string[indice++]) == t.getId() );

	indice = checkRobot(string, t, indice);

	for (int i = 0; i < Thymio2::LED_COUNT; ++i)
	{
		checkColor(string, t.getColorLed((Thymio2::LedIndex)i), indice + i*4);
	}

	return indice + Thymio2::LED_COUNT * 4;
}

static int checkEPuck(std::vector<std::string> &string, const EPuck &e, int start)
{
	int indice = start;
	REQUIRE ( stod(string[indice++]) ==  static_cast<int>(Factory::ROBOT_TYPES::EPUCK) );
	REQUIRE ( stod(string[indice++]) == e.getId() );

	indice = checkRobot(string, e, indice);

	return indice;
}

static int checkSbot(std::vector<std::string> &string, const Sbot &s, int start)
{
	int indice = start;
	REQUIRE ( stod(string[indice++]) ==  static_cast<int>(Factory::ROBOT_TYPES::SBOT) );
	REQUIRE ( stod(string[indice++]) == s.getId() );

	indice = checkRobot(string, s, indice);

	return indice;
}

static int checkMarxbot(std::vector<std::string> &string, const Marxbot &m, int start)
{
	int indice = start;
	REQUIRE ( stod(string[indice++]) ==  static_cast<int>(Factory::ROBOT_TYPES::MARXBOT) );
	REQUIRE ( stod(string[indice++]) == m.getId() );

	indice = checkRobot(string, m, indice);

	return indice;
}

static int checkKhepera(std::vector<std::string> &string, const Khepera &k, int start)
{
	int indice = start;
	REQUIRE ( stod(string[indice++]) ==  static_cast<int>(Factory::ROBOT_TYPES::KHEPERA) );
	REQUIRE ( stod(string[indice++]) == k.getId() );

	indice = checkRobot(string, k, indice);

	return indice;
}

static int checkBasePhysObject(std::vector<std::string> &string, const PhysicalObject &po, int start)
{
	int indice = start;
	REQUIRE ( stod(string[indice++]) ==  static_cast<int>(Factory::ROBOT_TYPES::PHYSICAL_OBJECT) );
	REQUIRE ( stod(string[indice++]) == po.getId() );

	indice = checkPoint(string, po.pos, indice);
	REQUIRE ( fabs(stod(string[indice++]) - po.angle) <= EPSILON );

	indice = checkColor(string, po.getColor(), indice);

	return indice;
}

static int checkPhysicalObject(std::vector<std::string> &string, const PhysicalObject &po, int start)
{
	int indice = checkBasePhysObject(string, po, start);

	REQUIRE ( stod(string[indice++]) == po.isCylindric() );

	if (po.isCylindric())
	{
		REQUIRE ( fabs(stod(string[indice++]) - po.getRadius()) <= EPSILON );
		REQUIRE ( fabs(stod(string[indice++]) - po.getHeight()) <= EPSILON );
		REQUIRE ( fabs(stod(string[indice++]) - po.getMass()) <= EPSILON );
	}
	else
	{
		indice = checkHull(string, po.getHull(), indice);
		REQUIRE ( fabs(stod(string[indice++]) - po.getMass()) <= EPSILON );
	}

	return indice;
}

TEST_CASE( "Unitary TEST_CASE on serialization", "[UNIT Serialization]" )
{
	Randomizer r;
	SECTION("Color serialization") {
		for (int i = 0; i < NB_ITERATIONS; i++)
		{
			Color c = r.randColor();

			std::ostringstream stream;
			serialize(c, stream, Separator::a);

			std::vector<std::string> &splittedString = splitAll(stream.str());
			checkColor(splittedString, c, 0);
			delete &splittedString;
		}
	}

	SECTION("GroundTexture serialization") {
		for (int i = 0; i < NB_ITERATIONS; i++)
		{
			World::GroundTexture gt = r.randGroundTexture(0, 0);

			std::ostringstream stream;
			serialize(gt, stream, Separator::a, true);

			std::vector<std::string> &splittedString = splitAll(stream.str());
			checkGroundTexture(splittedString, gt, 0);
			delete &splittedString;
		}
	}

	SECTION( "Hull/Part/Polygone serialization" ) {
		for (int i = 0; i < NB_ITERATIONS; i++)
		{
			PhysicalObject::Hull hull = r.randHull(-1);

			std::ostringstream stream;
			serialize(hull, stream, Separator::a);
			std::vector<std::string> &splittedString = splitAll(stream.str());
			checkHull(splittedString, hull, 0);
			delete &splittedString;
		}
	}

	SECTION ( "PhysicalObject (cylindric) full serialization" ) {
		for (int i = 0; i < NB_ITERATIONS; i++)
		{
			PhysicalObject* po = r.randPhysicalObject(0);

			std::ostringstream stream;
			serialize(po, stream, Separator::a, true);

			std::vector<std::string> &splittedString = splitAll(stream.str());
			checkPhysicalObject(splittedString, *po, 0);
			delete &splittedString;
			delete po;
		}
	}

	SECTION ( "PhysicalObject (custom hull) full serialization" ) {
		for (int i = 0; i < NB_ITERATIONS; i++)
		{
			PhysicalObject* po = r.randPhysicalObject(-1);

			std::ostringstream stream;
			serialize(po, stream, Separator::a, true);

			std::vector<std::string> &splittedString = splitAll(stream.str());
			checkPhysicalObject(splittedString, *po, 0);
			delete &splittedString;
			delete po;
		}
	}

	SECTION ( "PhysicalObject (CH with textures) full serialization" ) {
		for (int i = 0; i < NB_ITERATIONS; i++)
		{
			PhysicalObject* po = r.randPhysicalObject(-1);
			while( !po->getHull()[0].isTextured() )
			{
				delete po;
				po = r.randPhysicalObject(-1);
			}

			std::ostringstream stream;
			serialize(po, stream, Separator::a, true);

			std::vector<std::string> &splittedString = splitAll(stream.str());
			checkPhysicalObject(splittedString, *po, 0);
			delete &splittedString;
			delete po;
		}
	}

	SECTION ( "PhysicalObject update serialization" ) {
		for (int i = 0; i < NB_ITERATIONS; i++)
		{
			PhysicalObject* po = r.randPhysicalObject(0);

			std::ostringstream stream;
			serialize(po, stream, Separator::a, false);

			std::vector<std::string> &splittedString = splitAll(stream.str());
			checkBasePhysObject(splittedString, *po, 0);
			delete &splittedString;
			delete po;
		}
	}

	SECTION( "Robot serialization" ) {
		for (int i = 0; i < NB_ITERATIONS; i++)
		{
			Robot* robot = r.randRobot();

			std::ostringstream stream;
			serialize(robot, stream, Separator::a, true);

			std::vector<std::string> &splittedString = splitAll(stream.str());

			checkRobot(splittedString, *robot, 2);
			delete &splittedString;
			delete robot;
		}
	}

	SECTION ( "Thymio2 full serialization" ) {
		for (int i = 0; i < NB_ITERATIONS; i++)
		{
			Thymio2* t = r.randThymio();

			std::ostringstream stream;
			serialize(t, stream, Separator::a, true);

			std::vector<std::string> &splittedString = splitAll(stream.str());
			checkThymio(splittedString, *t, 0);
			delete &splittedString;
			delete t;
		}
	}

	SECTION( "EPuck full serialization" ) {
		for (int i = 0; i < NB_ITERATIONS; i++)
		{
			EPuck* e = r.randEPuck();

			std::ostringstream stream;
			serialize(e, stream, Separator::a, true);

			std::vector<std::string> &splittedString = splitAll(stream.str());
			checkEPuck(splittedString, *e, 0);
			delete &splittedString;
			delete e;
		}
	}

	SECTION( "Sbot full serialization" ) {
		for (int i = 0; i < NB_ITERATIONS; i++)
		{
			Sbot* s = r.randSbot();

			std::ostringstream stream;
			serialize(s, stream, Separator::a, true);

			std::vector<std::string> &splittedString = splitAll(stream.str());
			checkSbot(splittedString, *s, 0);
			delete &splittedString;
			delete s;
		}
	}

	SECTION( "Marxbot full serialization" ) {
		for (int i = 0; i < NB_ITERATIONS; i++)
		{
			Marxbot* m = r.randMarxbot();

			std::ostringstream stream;
			serialize(m, stream, Separator::a, true);

			std::vector<std::string> &splittedString = splitAll(stream.str());
			checkMarxbot(splittedString, *m, 0);
			delete &splittedString;
			delete m;
		}
	}

	SECTION( "Khepera full serialization" ) {
		for (int i = 0; i < NB_ITERATIONS; i++)
		{
			Khepera* k = r.randKhepera();

			std::ostringstream stream;
			serialize(k, stream, Separator::a, true);

			std::vector<std::string> &splittedString = splitAll(stream.str());
			checkKhepera(splittedString, *k, 0);
			delete &splittedString;
			delete k;
		}
	}

	SECTION( "Empty Circular World (No GT)" ) {
		for (int i = 0; i < NB_ITERATIONS; i++)
		{
			World* world = new World(r.randInt(1, 800), r.randColor());
			std::string serializedWorld = serialize(world, true);

			std::vector<std::string> &splittedString = splitAll(serializedWorld);

			// World Information.
			int j = 0;
			REQUIRE ( stod(splittedString[j++]) == world->wallsType );
			REQUIRE ( stod(splittedString[j++]) == world->r );

			// Color.
			j = checkColor(splittedString, world->color, j);

			// GroundTexture.
			j = checkGroundTexture(splittedString, world->groundTexture, j);

			delete world;
			delete &splittedString;
		}
	}

	SECTION ( "Empty Rectangular World (No GT)" ) {
		for (int i = 0; i < NB_ITERATIONS; i++)
		{
			World* world = new World(r.randInt(1, 800), r.randInt(1, 800), r.randColor());
			std::string serializedWorld = serialize(world, true);

			std::vector<std::string> &splittedString = splitAll(serializedWorld);

			// World Information.
			int j = 0;
			REQUIRE ( stod(splittedString[j++]) == world->wallsType );
			REQUIRE ( stod(splittedString[j++]) == world->w );
			REQUIRE ( stod(splittedString[j++]) == world->h );

			// Color.
			j = checkColor(splittedString, world->color, j);

			// GroundTexture.
			j = checkGroundTexture(splittedString, world->groundTexture, j);

			delete world;
			delete &splittedString;
		}
	}

	SECTION ( "Empty Rectangular World (GT)" ) {
		for (int i = 0; i < NB_ITERATIONS; i++)
		{
			World::GroundTexture gt = r.randGroundTexture(0, 0);

			World* world = new World(r.randInt(1, 800), r.randInt(1, 800), r.randColor(), gt);
			std::string serializedWorld = serialize(world, true);

			std::vector<std::string> &splittedString = splitAll(serializedWorld);

			// World Information.
			int j = 0;
			REQUIRE ( stod(splittedString[j++]) == world->wallsType );
			REQUIRE ( stod(splittedString[j++]) == world->w );
			REQUIRE ( stod(splittedString[j++]) == world->h );

			// Color.
			j = checkColor(splittedString, world->color, j);

			// GroundTexture.
			j = checkGroundTexture(splittedString, world->groundTexture, j);

			delete world;
			delete &splittedString;
		}
	}

	SECTION ( "Empty Circular World (GT)" ) {
		for (int i = 0; i < NB_ITERATIONS; i++)
		{
			World::GroundTexture gt = r.randGroundTexture(0, 0);

			World* world = new World(r.randInt(1, 800), r.randColor(), gt);
			std::string serializedWorld = serialize(world, true);

			std::vector<std::string> &splittedString = splitAll(serializedWorld);

			// World Information.
			int j = 0;
			REQUIRE ( stod(splittedString[j++]) == world->wallsType );
			REQUIRE ( stod(splittedString[j++]) == world->r );

			// Color.
			j = checkColor(splittedString, world->color, j);

			// GroundTexture.
			j = checkGroundTexture(splittedString, world->groundTexture, j);

			delete world;
			delete &splittedString;
		}
	}
}
