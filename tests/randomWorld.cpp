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

#include <enki/worldgenerator/WorldGenerator.h>
#include <enki/PhysicalEngine.h>

/*!	\file randomWorld.cpp
	\brief tests for 'random' module
*/

using namespace std;
using namespace Enki;

// Used to activate/deactive DEBUG informations
#define DEBUG 0

// Used to ensure the robustness of the tested code.
const int ITERATION_NUMBER = 100;

TEST_CASE( "UNIT Testing", "[Enki::RandomWorld.h]" ) {
	Randomizer* globalRandomizer = new Randomizer();
	SECTION ( "Random integer generation" )
	{
		int previousInt = -1;
		int trueRandom = 1;
		for (int i = 0; i < ITERATION_NUMBER; i++)
		{
			int number = globalRandomizer->randInt(0, i);
			REQUIRE ( ( 0 <= number && number <= i) );
			if (number == previousInt)
				trueRandom++;
			previousInt = number;
		}
		REQUIRE (trueRandom != ITERATION_NUMBER - 1);

	}

	SECTION ( "Dice roll test" )
	{
		int diceFaces = 6;
		int dice[diceFaces];
		for (int i = 0; i < diceFaces; i++)
		{
			dice[i] = 0;
		}

		int diceThrows = 100000;
		for (int i = 0; i < diceThrows; i++)
		{
			int number = globalRandomizer->randInt(1, diceFaces);
			dice[number-1] += 1;
		}

		for (int i = 0; i < diceFaces; i++)
		{
			float proba = (dice[i] / (float)diceThrows) * 100;
			if (DEBUG)
				std::cerr << "Proba of " << i+1 << ": " << proba << "%" << std::endl;
			REQUIRE ( ((100/(float)diceFaces - 2) <= proba && proba <= (100/(float)diceFaces + 2)) );
		}
	}

	SECTION ( "Random float generation") {
		for (int i = 0; i < ITERATION_NUMBER; i++)
		{
			float number = globalRandomizer->randFloat(0, 1);
			REQUIRE ( (0 <= number && number <= 1) );
		}
	}

	SECTION ( "A random color" ) {
		for (int i = 0; i < ITERATION_NUMBER; i++)
		{
			Color c = globalRandomizer->randColor();
			REQUIRE ( (0 <= c.r() && c.r() <= 1) );
			REQUIRE ( (0 <= c.g() && c.g() <= 1) );
			REQUIRE ( (0 <= c.b() && c.b() <= 1) );
			REQUIRE ( (0 <= c.a() && c.a() <= 1) );
		}
	}

	// This means that the Point should be within the world area
	// If you want to see if it correctly rands random numbers
	// You should check the first SECTION.
	SECTION ( "A random position in the world" ) {
		World* w = globalRandomizer->getWorld();
		for (int i = 0; i < ITERATION_NUMBER; i++)
		{
			Point p = globalRandomizer->randPoint();
			if (w->wallsType == World::WALLS_CIRCULAR)
			{
				REQUIRE ( ((p.x * p.x) + (p.y * p.y) <= w->r * w->r) );
			}
			else
			{
				REQUIRE ( (p.x <= w->w && p.y <= w->h) );
			}
		}
	}

	SECTION ( "A random convex polygon" ) {
		for (int i = 0; i < ITERATION_NUMBER; i++)
		{
			Polygone p = globalRandomizer->randConvexPolygone(i);
			REQUIRE ( p.size() >= 3 );
		}

	}

	SECTION ( "A random part" ) {
		for (int i = 0; i < ITERATION_NUMBER; i++)
		{
			PhysicalObject::Part p = globalRandomizer->randComplexPart();
			REQUIRE ( (p.getShape().size() >= 3) );
			REQUIRE ( (1 <= p.getHeight() && p.getHeight() <= 5) );
		}
		for (int i = 0; i < ITERATION_NUMBER; i++)
		{
			PhysicalObject::Part p = globalRandomizer->randRectanglePart();
			REQUIRE ( (1 <= p.getHeight() && p.getHeight() <= 30) );
			REQUIRE ( (1*1 <= p.getArea() && p.getArea() <= 30 * 30) );
		}
	}

	SECTION ( "A random texture" ) {
		for (int i = 0; i < ITERATION_NUMBER; i++)
		{
			Texture t = globalRandomizer->randTexture();
			REQUIRE ( ( 1 <= t.size() && t.size() <= 5) );
			Texture t1 = globalRandomizer->randTexture(i+1);
			REQUIRE ( (t1.size() == i+1) );
		}

		for (int i = 0; i < ITERATION_NUMBER; i++)
		{
			Textures t = globalRandomizer->randTextures();
			REQUIRE ( (1 <= t.size() && t.size() <= 5) );
			Textures t1 = globalRandomizer->randTextures(i+1);
			REQUIRE ( (t1.size() == i+1) );

		}
	}

	SECTION ( "A random GroundTexture" )
	{
		for(int i = 0; i < ITERATION_NUMBER; i++)
		{
			float width = i+100/2.0;
			float height = i+50;
			World::GroundTexture gt = globalRandomizer->randGroundTexture(width, height);

			REQUIRE ( (1.0 <= gt.width && gt.width <= width) );
			REQUIRE ( (1.0 <= gt.height && gt.width <= height) );
			REQUIRE ( (gt.height * gt.width) == gt.data.size() );
		}
	}


	SECTION ( "A random Hull" ) {
		for (int i = 0; i < ITERATION_NUMBER; i++)
		{
			PhysicalObject::Hull hull = globalRandomizer->randHull(i);
			if (i > 0)
				REQUIRE ( hull.size() == i );
			else
				REQUIRE ( hull.size() > 0 );
		}
	}

	SECTION ( "A random empty world" ) {

		for (int i = 0; i < ITERATION_NUMBER; i++)
		{
			World* w = globalRandomizer->randWorld();

			if (w->wallsType == World::WALLS_SQUARE)
			{
				REQUIRE ( (w->r == 0 &&
					(MIN_HEIGHT <= w->h && w->h <= MAX_HEIGHT) &&
					(MIN_WIDTH <= w->w && w->w <= MAX_WIDTH)) );
			}
			else
			{
				REQUIRE ( ((MIN_RADIUS <= w->r && w->r <= MAX_RADIUS) &&
					w->h == 0 &&
					w->w == 0) );
			}

			REQUIRE ( (w->objects.size() == 0) );

			delete w;
		}
	}

	SECTION ( "A random Robot" ) {
		globalRandomizer->resetWorld();
		World* w = globalRandomizer->getWorld();

		for (int i = 0; i < ITERATION_NUMBER; i++)
		{
			Robot* robot = globalRandomizer->randRobot();
			REQUIRE ( robot != NULL );

			if (w->wallsType == World::WALLS_SQUARE)
			{
				REQUIRE ( (robot->pos.x <= w->w && robot->pos.y <= w->h) );
			}
			else if (w->wallsType == World::WALLS_CIRCULAR)
			{
				REQUIRE ( ((robot->pos.x * robot->pos.x) + (robot->pos.y * robot->pos.y) <= w->r * w->r) );
			}

			delete robot;
		}
	}

	SECTION ( "A random physical object" ) {
		globalRandomizer->resetWorld();
		World* w = globalRandomizer->getWorld();

		for (int i = 0; i < ITERATION_NUMBER; i++)
		{
			PhysicalObject* obj = globalRandomizer->randPhysicalObject(0);
			REQUIRE ( obj != NULL );

			if (w->wallsType == World::WALLS_SQUARE)
			{
				REQUIRE ( (obj->pos.x <= w->w && obj->pos.y <= w->h) );
			}
			else if (w->wallsType == World::WALLS_CIRCULAR)
			{
				REQUIRE ( (obj->pos.x <= w->r && obj->pos.y <= w->r) );
			}

			delete obj;
		}
	}

	delete globalRandomizer;
}
