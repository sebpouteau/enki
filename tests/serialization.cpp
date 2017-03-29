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

static const double EPSILON = pow(10, -serialize_precision);


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
	REQUIRE( fabs(stod(string[indice++]) - gt.width * gt.height) <= EPSILON );

	for(int i = 0 ; i < gt.width * gt.height ; i++)
	{
		REQUIRE( fabs(stod(string[i + indice]) - gt.data[i]) <= EPSILON );
	}

	return indice + gt.width * gt.height;
}

static int checkPolygone(std::vector<std::string> &string, Polygone poly, int start)
{
	int indice = start;
	REQUIRE ( (stoi(string[indice++]) - poly.size()) <= EPSILON );

	for (int i = 0; i < poly.size(); i++)
	{
		checkPoint(string, poly[i], indice+i*2);
	}

	return indice + poly.size() * 2;
}

static int checkHull(std::vector<std::string> &string, PhysicalObject::Hull hull, int start)
{
	int indice = start;
	REQUIRE( (stoi(string[indice++]) - hull.size()) <= EPSILON );

	for (PhysicalObject::Part p : hull)
	{
		indice = checkPolygone(string, p.getShape(), indice);
		REQUIRE ( fabs(stod(string[indice++]) - p.getHeight()) <= EPSILON );
		REQUIRE ( stod(string[indice++]) == p.isTextured() );

		if (p.isTextured())
		{
			const Textures& textures = p.getTextures();
			REQUIRE ( stoi(string[indice++]) == textures.size() );

			for(int i = 0 ; i < textures.size() ; i++)
			{
				Texture t = textures[i];
				REQUIRE ( stoi(string[indice++]) == t.size() );
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
	REQUIRE ( stoi(string[indice++]) == Factory::TypeObject::THYMIO2);
	REQUIRE ( stoi(string[indice++]) == t.getId() );

	indice = checkRobot(string, t, indice);
	REQUIRE ( stoi(string[indice++]) == Thymio2::LED_COUNT );
	
	for (int i = 0; i < Thymio2::LED_COUNT; ++i)
	{
		checkColor(string, t.getColorLed((Thymio2::LedIndex)i), indice + i*4);
	}

	return indice + Thymio2::LED_COUNT * 4;
}

static int checkEPuck(std::vector<std::string> &string, const EPuck &e, int start)
{
	int indice = start;
	REQUIRE ( stoi(string[indice++]) == Factory::TypeObject::EPUCK );
	REQUIRE ( stoi(string[indice++]) == e.getId() );

	indice = checkRobot(string, e, indice);

	return indice;
}

static int checkSbot(std::vector<std::string> &string, const Sbot &s, int start)
{
	int indice = start;
	REQUIRE ( stoi(string[indice++]) ==  Factory::TypeObject::SBOT );
	REQUIRE ( stoi(string[indice++]) == s.getId() );

	indice = checkRobot(string, s, indice);

	return indice;
}

static int checkMarxbot(std::vector<std::string> &string, const Marxbot &m, int start)
{
	int indice = start;
	REQUIRE ( stoi(string[indice++]) == Factory::TypeObject::MARXBOT );
	REQUIRE ( stoi(string[indice++]) == m.getId() );

	indice = checkRobot(string, m, indice);

	return indice;
}

static int checkKhepera(std::vector<std::string> &string, const Khepera &k, int start)
{
	int indice = start;
	REQUIRE ( stoi(string[indice++]) == Factory::TypeObject::KHEPERA );
	REQUIRE ( stoi(string[indice++]) == k.getId() );

	indice = checkRobot(string, k, indice);

	return indice;
}

static int checkBasePhysObject(std::vector<std::string> &string, const PhysicalObject &po, int start)
{
	int indice = start;
	REQUIRE ( stoi(string[indice++]) == Factory::TypeObject::PHYS_OBJ );
	REQUIRE ( stoi(string[indice++]) == po.getId() );

	indice = checkPoint(string, po.pos, indice);
	REQUIRE ( fabs(stod(string[indice++]) - po.angle) <= EPSILON );

	indice = checkColor(string, po.getColor(), indice);

	return indice;
}

static int checkPhysicalObject(std::vector<std::string> &string, const PhysicalObject &po, int start)
{
	int indice = checkBasePhysObject(string, po, start);

	REQUIRE ( stoi(string[indice++]) == po.isCylindric() );

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
			serialize(c, stream);

			std::vector<std::string> splittedString = split(stream.str(), TYPE_SEPARATOR);
			checkColor(splittedString, c, 0);
		}
	}

	SECTION("GroundTexture serialization") {
		for (int i = 0; i < NB_ITERATIONS; i++)
		{
			World::GroundTexture gt = r.randGroundTexture(0, 0);

			std::ostringstream stream;
			serialize(gt, stream);

			std::vector<std::string> splittedString = split(stream.str(), TYPE_SEPARATOR);
			checkGroundTexture(splittedString, gt, 0);
		}
	}

	SECTION( "Hull/Part/Polygone serialization" ) {
		for (int i = 0; i < NB_ITERATIONS; i++)
		{
			PhysicalObject::Hull hull = r.randHull(-1);

			std::ostringstream stream;
			serialize(hull, stream);
			std::vector<std::string> splittedString = split(stream.str(), TYPE_SEPARATOR);
			checkHull(splittedString, hull, 0);
		}
	}

	SECTION ( "PhysicalObject (cylindric) full serialization" ) {
		for (int i = 0; i < NB_ITERATIONS; i++)
		{
			PhysicalObject* po = r.randPhysicalObject(0);

			std::ostringstream stream;
			serialize(po, stream, true);

			std::vector<std::string> splittedString = split(stream.str(), TYPE_SEPARATOR);
			checkPhysicalObject(splittedString, *po, 0);
			delete po;
		}
	}

	SECTION ( "PhysicalObject (custom hull) full serialization" ) {
		for (int i = 0; i < NB_ITERATIONS; i++)
		{
			PhysicalObject* po = r.randPhysicalObject(-1);

			std::ostringstream stream;
			serialize(po, stream, true);

			std::vector<std::string> splittedString = split(stream.str(), TYPE_SEPARATOR);
			checkPhysicalObject(splittedString, *po, 0);
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
			serialize(po, stream, true);

			std::vector<std::string> splittedString = split(stream.str(), TYPE_SEPARATOR);
			checkPhysicalObject(splittedString, *po, 0);
			delete po;
		}
	}

	SECTION ( "PhysicalObject update serialization" ) {
		for (int i = 0; i < NB_ITERATIONS; i++)
		{
			PhysicalObject* po = r.randPhysicalObject(0);

			std::ostringstream stream;
			serialize(po, stream, false);

			std::vector<std::string> splittedString = split(stream.str(), TYPE_SEPARATOR);
			checkBasePhysObject(splittedString, *po, 0);
			delete po;
		}
	}

	SECTION( "Robot serialization" ) {
		for (int i = 0; i < NB_ITERATIONS; i++)
		{
			Robot* robot = r.randRobot();

			std::ostringstream stream;
			serialize(robot, stream, true);

			std::vector<std::string> splittedString = split(stream.str(), TYPE_SEPARATOR);

			checkRobot(splittedString, *robot, 2);
			delete robot;
		}
	}

	SECTION ( "Thymio2 full serialization" ) {
		for (int i = 0; i < NB_ITERATIONS; i++)
		{
			Thymio2* t = r.randThymio();

			std::ostringstream stream;
			serialize(t, stream, true);

			std::vector<std::string> splittedString = split(stream.str(), TYPE_SEPARATOR);
			checkThymio(splittedString, *t, 0);
			delete t;
		}
	}

	SECTION( "EPuck full serialization" ) {
		for (int i = 0; i < NB_ITERATIONS; i++)
		{
			EPuck* e = r.randEPuck();

			std::ostringstream stream;
			serialize(e, stream, true);

			std::vector<std::string> splittedString = split(stream.str(), TYPE_SEPARATOR);
			checkEPuck(splittedString, *e, 0);
			delete e;
		}
	}

	SECTION( "Sbot full serialization" ) {
		for (int i = 0; i < NB_ITERATIONS; i++)
		{
			Sbot* s = r.randSbot();

			std::ostringstream stream;
			serialize(s, stream, true);

			std::vector<std::string> splittedString = split(stream.str(), TYPE_SEPARATOR);
			checkSbot(splittedString, *s, 0);
			delete s;
		}
	}

	SECTION( "Marxbot full serialization" ) {
		for (int i = 0; i < NB_ITERATIONS; i++)
		{
			Marxbot* m = r.randMarxbot();

			std::ostringstream stream;
			serialize(m, stream, true);

			std::vector<std::string> splittedString = split(stream.str(), TYPE_SEPARATOR);
			checkMarxbot(splittedString, *m, 0);
			delete m;
		}
	}

	SECTION( "Khepera full serialization" ) {
		for (int i = 0; i < NB_ITERATIONS; i++)
		{
			Khepera* k = r.randKhepera();

			std::ostringstream stream;
			serialize(k, stream, true);

			std::vector<std::string> splittedString = split(stream.str(), TYPE_SEPARATOR);
			checkKhepera(splittedString, *k, 0);
			delete k;
		}
	}

	SECTION( "Empty Circular World (No GT)" ) {
		for (int i = 0; i < NB_ITERATIONS; i++)
		{
			World* world = new World(r.randInt(1, 800), r.randColor());
			std::string serializedWorld = serialize(world, true);

			std::vector<std::string> splittedString = split(serializedWorld, TYPE_SEPARATOR);

			// World Information.
			int j = 0;
			REQUIRE ( stoi(splittedString[j++]) == world->wallsType );
			REQUIRE ( stod(splittedString[j++]) == world->r );

			// Color.
			j = checkColor(splittedString, world->color, j);

			// GroundTexture.
			j = checkGroundTexture(splittedString, world->groundTexture, j);

			delete world;
		}
	}

	SECTION ( "Empty Rectangular World (No GT)" ) {
		for (int i = 0; i < NB_ITERATIONS; i++)
		{
			World* world = new World(r.randInt(1, 800), r.randInt(1, 800), r.randColor());
			std::string serializedWorld = serialize(world, true);

			std::vector<std::string> splittedString = split(serializedWorld, TYPE_SEPARATOR);

			// World Information.
			int j = 0;
			REQUIRE ( stoi(splittedString[j++]) == world->wallsType );
			REQUIRE ( stod(splittedString[j++]) == world->w );
			REQUIRE ( stod(splittedString[j++]) == world->h );

			// Color.
			j = checkColor(splittedString, world->color, j);

			// GroundTexture.
			j = checkGroundTexture(splittedString, world->groundTexture, j);

			delete world;
		}
	}

	SECTION ( "Empty Rectangular World (GT)" ) {
		for (int i = 0; i < NB_ITERATIONS; i++)
		{
			World::GroundTexture gt = r.randGroundTexture(0, 0);

			World* world = new World(r.randInt(1, 800), r.randInt(1, 800), r.randColor(), gt);
			std::string serializedWorld = serialize(world, true);

			std::vector<std::string> splittedString = split(serializedWorld, TYPE_SEPARATOR);

			// World Information.
			int j = 0;
			REQUIRE ( stoi(splittedString[j++]) == world->wallsType );
			REQUIRE ( stod(splittedString[j++]) == world->w );
			REQUIRE ( stod(splittedString[j++]) == world->h );

			// Color.
			j = checkColor(splittedString, world->color, j);

			// GroundTexture.
			j = checkGroundTexture(splittedString, world->groundTexture, j);

			delete world;
		}
	}

	SECTION ( "Empty Circular World (GT)" ) {
		for (int i = 0; i < NB_ITERATIONS; i++)
		{
			World::GroundTexture gt = r.randGroundTexture(0, 0);

			World* world = new World(r.randInt(1, 800), r.randColor(), gt);
			std::string serializedWorld = serialize(world, true);

			std::vector<std::string> splittedString = split(serializedWorld, TYPE_SEPARATOR);

			// World Information.
			int j = 0;
			REQUIRE ( stoi(splittedString[j++]) == world->wallsType );
			REQUIRE ( stod(splittedString[j++]) == world->r );

			// Color.
			j = checkColor(splittedString, world->color, j);

			// GroundTexture.
			j = checkGroundTexture(splittedString, world->groundTexture, j);

			delete world;
		}
	}
}
