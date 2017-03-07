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
#include <enki/Serialize.h>
#include "Randomizer.h"
#include <climits>

const int ITERATION_NUMBER = 10;
const int NB_MAX_BYTES_SERIALIZE = 50000;

using namespace Enki;
using namespace std;

static PhysicalObject* createPhysObjWith0Hull()
{
	PhysicalObject* o = new PhysicalObject;
	o->pos = Point(40, 20);
	o->setCylindric(4, 2, 10);
	o->setColor(Color(0.2, 0.2, 0.6));
	o->dryFrictionCoefficient = 0.;
	return o;
}

static PhysicalObject* createPhysObjWith1Hull()
{
	const double amount = 9;
	const double radius = 5;
	const double height = 20;
	Polygone p;
	for (double a = 0; a < 2*M_PI; a += 2*M_PI/amount)
		p.push_back(Point(radius * cos(a), radius * sin(a)));
			
	PhysicalObject* o = new PhysicalObject;
	PhysicalObject::Hull hull(Enki::PhysicalObject::Part(p, height));
	o->setCustomHull(hull, -1);
	o->setColor(Color(0.4,0.6,0.8));
	o->pos = Point(100, 100);
	return o;
}

static PhysicalObject* createPhysObjWith2Hull()
{
	PhysicalObject* o = new PhysicalObject;
	PhysicalObject::Hull hull;
	hull.push_back(Enki::PhysicalObject::Part(Polygone() << Point(5,1) << Point(-5,1) << Point(-5,-1) << Point(5,-1), 2));
	hull.push_back(Enki::PhysicalObject::Part(Polygone() << Point(1,5) << Point(-1,5) << Point(-1,-5) << Point(1,-5), 4));
	o->setCustomHull(hull, 60);
	o->setColor(Color(0.2, 0.4, 0.6));
	o->collisionElasticity = 0.2;
	o->pos = Point(UniformRand(20, 100)(), UniformRand(20, 100)());
	return o;
}

TEST_CASE( "BenchMarks Size", "[BenchMarks Size]" ) {
	SECTION( "[Size] World with Thymio" ) {
		int nbThymioMax = 0;
		int nbThymioMin = INT_MAX;
		
		for (int i = 0; i < ITERATION_NUMBER; i++) {
			int nbThymio = 0;
			World* world = randomWorld();
			string strSerialize;
			while (strSerialize.size() < NB_MAX_BYTES_SERIALIZE) {
				world->addObject(randomThymio(world));
				nbThymio++;
				strSerialize = world->serialize(false);
			}
			nbThymio--;
			
			REQUIRE(nbThymio > 30);

			if (nbThymioMax < nbThymio) { nbThymioMax = nbThymio; }
			if (nbThymioMin > nbThymio) { nbThymioMin = nbThymio; }
		}

		cerr << "BenchMarks Size - Thymio : " << endl
			 << " - Max number of Thymio = " << nbThymioMax << endl
			 << " - Min number of Thymio = " << nbThymioMin << endl;
	}

	SECTION( "[Size] World with Physical Object (0 hull)" ) {
		int nbPhysObjMax = 0;
		int nbPhysObjMin = INT_MAX;
		
		for (int i = 0; i < ITERATION_NUMBER; i++) {
			int nbPhysObj = 0;
			World* world = randomWorld();
			string strSerialize;
			while (strSerialize.size() < NB_MAX_BYTES_SERIALIZE) {
				// TODO : remplace createPhysObjWith0Hull by a random function
				world->addObject(createPhysObjWith0Hull());
				nbPhysObj++;
				strSerialize = world->serialize(false);
			}
			nbPhysObj--;
			
			REQUIRE(nbPhysObj > 50);

			if (nbPhysObjMax < nbPhysObj) { nbPhysObjMax = nbPhysObj; }
			if (nbPhysObjMin > nbPhysObj) { nbPhysObjMin = nbPhysObj; }
		}

		cerr << "BenchMarks Size - Physical Object (0 hull) : " << endl
			 << " - Max number of Physical Object = " << nbPhysObjMax << endl
			 << " - Min number of Physical Object = " << nbPhysObjMin << endl;
	}

	SECTION( "[Size] World with Physical Object (1 hull)" ) {
		int nbPhysObjMax = 0;
		int nbPhysObjMin = INT_MAX;
		
		for (int i = 0; i < ITERATION_NUMBER; i++) {
			int nbPhysObj = 0;
			World* world = randomWorld();
			string strSerialize;
			while (strSerialize.size() < NB_MAX_BYTES_SERIALIZE) {
				// TODO : remplace createPhysObjWith1Hull by a random function
				world->addObject(createPhysObjWith1Hull());
				nbPhysObj++;
				strSerialize = world->serialize(false);
			}
			nbPhysObj--;
			
			REQUIRE(nbPhysObj > 50);

			if (nbPhysObjMax < nbPhysObj) { nbPhysObjMax = nbPhysObj; }
			if (nbPhysObjMin > nbPhysObj) { nbPhysObjMin = nbPhysObj; }
		}

		cerr << "BenchMarks Size - Physical Object (1 hull) : " << endl
			 << " - Max number of Physical Object = " << nbPhysObjMax << endl
			 << " - Min number of Physical Object = " << nbPhysObjMin << endl;
	}

	SECTION( "[Size] World with Physical Object (2 hull)" ) {
		int nbPhysObjMax = 0;
		int nbPhysObjMin = INT_MAX;
		
		for (int i = 0; i < ITERATION_NUMBER; i++) {
			int nbPhysObj = 0;
			World* world = randomWorld();
			string strSerialize;
			while (strSerialize.size() < NB_MAX_BYTES_SERIALIZE) {
				// TODO : remplace createPhysObjWith2Hull by a random function
				world->addObject(createPhysObjWith2Hull());
				nbPhysObj++;
				strSerialize = world->serialize(false);
			}
			nbPhysObj--;
			
			REQUIRE(nbPhysObj > 50);

			if (nbPhysObjMax < nbPhysObj) { nbPhysObjMax = nbPhysObj; }
			if (nbPhysObjMin > nbPhysObj) { nbPhysObjMin = nbPhysObj; }
		}

		cerr << "BenchMarks Size - Physical Object (2 hull) : " << endl
			 << " - Max number of Physical Object = " << nbPhysObjMax << endl
			 << " - Min number of Physical Object = " << nbPhysObjMin << endl;
	}

	SECTION( "[Size] World with (at least) 30 Thymio & 50 Physical Objects (1 hull)" ) {
		int nbThymioMax = 0;
		int nbThymioMin = INT_MAX;
		
		int nbPhysObjMax = 0;
		int nbPhysObjMin = INT_MAX;
		
		for (int i = 0; i < ITERATION_NUMBER; i++) {
			int nbThymio = 30;
			int nbPhysObj = 50;
			World* world = randomWorld();
			
			for (int j = 0; j < 30; ++j) {
				world->addObject(randomThymio(world));
			}
			
			for (int j = 0; j < 50; ++j) {
				// TODO : remplace createPhysObjWith1Hull by a random function
				world->addObject(createPhysObjWith1Hull());
			}
			
			string strSerialize = world->serialize(false);
			REQUIRE(strSerialize.size() < NB_MAX_BYTES_SERIALIZE);
			
			while (strSerialize.size() < NB_MAX_BYTES_SERIALIZE) {
				world->addObject(randomThymio(world));
				nbThymio++;
				
				// TODO : remplace createPhysObjWith2Hull by a random function
				world->addObject(createPhysObjWith2Hull());
				nbPhysObj++;
				
				strSerialize = world->serialize(false);
			}
			nbPhysObj--;
			nbThymio--;
			
			REQUIRE(nbThymio > 30);
			REQUIRE(nbPhysObj > 50);

			if (nbPhysObjMax < nbPhysObj) { nbPhysObjMax = nbPhysObj; }
			if (nbPhysObjMin > nbPhysObj) { nbPhysObjMin = nbPhysObj; }
			
			if (nbThymioMax < nbThymio) { nbThymioMax = nbThymio; }
			if (nbThymioMin > nbThymio) { nbThymioMin = nbThymio; }
		}

		cerr << "BenchMarks Size - (at least) 30 Thymio & 50 Physical Objects (1 hull) : " << endl
			 << " - Max number of Thymio = " << nbThymioMax << endl
			 << " - Min number of Thymio = " << nbThymioMin << endl
			 << " - Max number of Physical Object = " << nbPhysObjMax << endl
			 << " - Min number of Physical Object = " << nbPhysObjMin << endl;
	}
}
