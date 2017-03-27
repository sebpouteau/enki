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
#include <chrono>

#include <enki/PhysicalEngine.h>
#include <enki/robots/thymio2/Thymio2.h>
#include <enki/Serialize.h>

const int MAX_SIZE = 40000; // bytes
const int MAX_TIME = 20; // ms
const int NB_ITERATIONS = 1; 

using namespace Enki;
using namespace std;

struct perfs
{
	string name = "Perf";
	double mean = -1;
	double max = -1;
	double min = -1;
};

typedef chrono::system_clock::time_point timepoint;

// Return the elapsed time in milliseconds between two timepoints objects.
double msTime(timepoint start, timepoint end)
{
	return chrono::duration_cast<chrono::nanoseconds>(end - start).count() / 1000000.0;
}

static PhysicalObject* createPhysObj(int nbHull)
{
	PhysicalObject* po = new PhysicalObject;
	po->pos = Point(0, 0);
	po->setColor(Color(0.2, 0.2, 0.6));

	if (nbHull == 0) {
		po->setCylindric(1.11, 1.11, 1.11);
	} else {
		PhysicalObject::Hull hull;
		for (int i = 0; i < nbHull; ++i) {
			hull.push_back(Enki::PhysicalObject::Part(i/10, i/10, i/10));
		}
		po->setCustomHull(hull, 1.11);
	}
	
	return po;
}

// Update the perfs structure with a given value
static void updatePerfs(perfs* p, double value)
{
	if (p->max < 0) {
		p->max = value;
		p->min = value;
		p->mean = value;
	} else {
		if (p->max < value)	{
			p->max = value;
		}
		if (p->min > value) {
			p->min = value;
		}
		p->mean += value;
	}
}

static void displayPerf(const perfs& p) {
	cerr << " - " << p.name << " : " << endl
		 << "   - Min :  \t" << p.min << endl
		 << "   - Max :  \t" << p.max << endl
		 << "   - Mean : \t" << p.mean/NB_ITERATIONS << endl;
}


TEST_CASE( "BenchMarks Serialization Size", "[BenchMarks Serialization Size]" ) {
	SECTION( "[Size Init] World with Thymio" ) {
		cerr << "[Size Init] World with Thymio : " << endl;

		World* world = new World();

		int sizeSerialization = 0;
		while (true) {
			string strSerialization = serialize(world, true);
			if (strSerialization.size() > MAX_SIZE) { break; }
			sizeSerialization = strSerialization.size();
			world->addObject(new Thymio2());
		}

		int nbThymio = world->objects.size();
		
		delete world;
		REQUIRE (nbThymio >= 30);

		cerr << " - Number of Thymio : " << nbThymio << endl
			 << " - Size : " << sizeSerialization << " bytes (" << sizeSerialization / 1000.0 << " kB) " << endl
			 << endl;
	}
	
	SECTION( "[Size Update] World with Thymio" ) {
		cerr << "[Size Update] World with Thymio : " << endl;

		World* world = new World();
		
		int sizeSerialization = 0;
		while (true) {
			string strSerialization = serialize(world, false);
			if (strSerialization.size() > MAX_SIZE) { break; }
			sizeSerialization = strSerialization.size();
			world->addObject(new Thymio2());
		}

		int nbThymio = world->objects.size();
		
		delete world;
		REQUIRE (nbThymio >= 30);

		cerr << " - Number of Thymio : " << nbThymio << endl
			 << " - Size : " << sizeSerialization << " bytes (" << sizeSerialization / 1000.0 << " kB) " << endl
			 << endl;
	}
	
	
	SECTION( "[Size Init] World with Physical Objects" ) {
		cerr << "[Size Init] World with Physical Objects : "  << endl
			 << "|   nb  |  nb Physical  |    Size (kB)    |" << endl
			 << "|  Hull |    Objects    |  Serialization  |" << endl
			 << "|-------|---------------|-----------------|" << endl;
				
		int nbHull = 0;
		while (true) {
			World* world = new World();			

			int sizeSerialization = 0;
			while (true) {
				string strSerialization = serialize(world, true);
				if (strSerialization.size() > MAX_SIZE) { break; }
				sizeSerialization = strSerialization.size();
				world->addObject(createPhysObj(nbHull));
			}

			int nbPhysObj = world->objects.size();

			delete world;
			if (nbPhysObj < 50) {
				REQUIRE(nbHull != 0);
				break;
			}
			
			cerr.precision(4);
			cerr << "|  "     << nbHull    << "\t" 
				 << "|     "  << nbPhysObj << "  \t"
				 << "|     "  << sizeSerialization / 1000.0 << "    \t  |" << endl;
			
			nbHull++;
		}
		
		cerr << endl;
	}
	
	
	SECTION( "[Size Update] World with Physical Objects" ) {
		cerr << "[Size Update] World with Physical Objects : " << endl;

		World* world = new World();			

		int sizeSerialization = 0;
		while (true) {
			string strSerialization = serialize(world, false);
			if (strSerialization.size() > MAX_SIZE) { break; }
			sizeSerialization = strSerialization.size();
			world->addObject(new PhysicalObject());
		}

		int nbPhysObj = world->objects.size();

		delete world;
		REQUIRE(nbPhysObj >= 50);
		
		cerr << " - Number of Physical Objects : " << nbPhysObj << endl
			 << " - Size : " << sizeSerialization << " bytes (" << sizeSerialization / 1000.0 << " kB) " << endl << endl;
	}
	
	SECTION( "[Size Init] World with 30 Thymio & (at least) 50 Physical Objects" ) {
		cerr << "[Size Init] World with 30 Thymio & (at least) 50 Physical Objects : " << endl
			 << "|   nb  | nb Physical |   nb   |   Size (kB)    |" << endl
			 << "|  Hull |   Objects   | Thymio | Serialization  |" << endl
			 << "|-------|-------------|--------|----------------|" << endl;
		
		int nbHull = 0;
		while (true) {
			World* world = new World();
			for (int i = 0; i < 30; i++) { world->addObject(new Thymio2()); }
			for (int i = 0; i < 50; i++) { world->addObject(createPhysObj(nbHull)); }
			
			int sizeSerialization = 0;
			while (true) {
				string strSerialization = serialize(world, true); // Serialize Init
				if (strSerialization.size() > MAX_SIZE) { break; }
				sizeSerialization = strSerialization.size();
				world->addObject(createPhysObj(nbHull));
			}
			
			int nbPhysObj = world->objects.size() - 30 - 1;
			
			delete world;		   
			if (nbPhysObj < 50) {
				REQUIRE(nbHull != 0);
				break;
			}
			
			cerr.precision(4);
			cerr << "|  "  << nbHull    << "\t" 
				 << "|  "  << nbPhysObj << "\t      "
				 << "|   30   " 
				 << "|   " << sizeSerialization / 1000.0 << "    \t|" << endl;
			
			nbHull++;
		}
		
		cerr << endl;
	}
	
	SECTION( "[Size Update] World with 30 Thymio & (at least) 50 Physical Objects" ) {
		cerr << "[Size Update] World with 30 Thymio & (at least) 50 Physical Objects : " << endl;
		
		World* world = new World();
		for (int i = 0; i < 30; i++) { world->addObject(new Thymio2()); }
		for (int i = 0; i < 49; i++) { world->addObject(new PhysicalObject()); }

		int sizeSerialization = 0;
		while (true) {
			string strSerialization = serialize(world, false); // Serialize Init
			if (strSerialization.size() > MAX_SIZE) { break; }
			sizeSerialization = strSerialization.size();
			world->addObject(new PhysicalObject());
		}
		
		int nbPhysObj = world->objects.size() - 30;
		
		delete world;
		REQUIRE(nbPhysObj >= 50);
		
		cerr << " - Number of Physical Objects : " << nbPhysObj << endl
			 << " - Size : " << sizeSerialization << " bytes (" << sizeSerialization / 1000.0 << " kB) " << endl
			 << endl;
	}
}


TEST_CASE( "BenchMarks Time Serialization", "[BenchMarks Time Serialization]" ) {
	SECTION( "[Time Serialization Init] World with Thymio" ) {
		cerr << "[Time Serialization Init] World with Thymio : " << endl;

		perfs perfTime;
		perfTime.name = "Time of Serialization";
		perfs perfNbThymio;
		perfNbThymio.name = "Number of Thymio";
		
		for (int i = 0; i < NB_ITERATIONS; ++i) {
			World* world = new World();

			double timeSerialization = 0;
			while (true) {
				timepoint start = chrono::system_clock::now();
				string strSerialization = serialize(world, true); // Serialize Init
				timepoint end = chrono::system_clock::now();
			
				if (msTime(start, end) > MAX_TIME) { break; }
				timeSerialization = msTime(start, end);
				world->addObject(new Thymio2());
			}
			
			int nbThymio = world->objects.size();

			delete world;
			REQUIRE(nbThymio >= 30);

			updatePerfs(&perfTime, timeSerialization);
			updatePerfs(&perfNbThymio, nbThymio);
			
			cerr << " - Iteration " << i + 1 << "/" << NB_ITERATIONS << " \t|"
				 << " Nb Thymio : " <<  nbThymio << "\t|"
				 << " Time : " << timeSerialization << endl;
		}

		cerr << " ----- " << endl
			 << "[Resume - Time Serialization Init] World with Thymio : " << endl;
		displayPerf(perfNbThymio);
		displayPerf(perfTime);
		cerr << endl;
	}
	
	
	SECTION( "[Time Serialization Update] World with Thymio" ) {
		cerr << "[Time Serialization Update] World with Thymio : " << endl;

		perfs perfTime;
		perfTime.name = "Time of Serialization";
		perfs perfNbThymio;
		perfNbThymio.name = "Number of Thymio";
		
		for (int i = 0; i < NB_ITERATIONS; ++i) {
			World* world = new World();

			double timeSerialization = 0;
			while (true) {
				timepoint start = chrono::system_clock::now();
				string strSerialization = serialize(world, false); // Serialize Update
				timepoint end = chrono::system_clock::now();
			
				if (msTime(start, end) > MAX_TIME) { break; }
				timeSerialization = msTime(start, end);
				world->addObject(new Thymio2());
			}
			
			int nbThymio = world->objects.size();

			delete world;
			REQUIRE(nbThymio >= 30);

			updatePerfs(&perfTime, timeSerialization);
			updatePerfs(&perfNbThymio, nbThymio);
			
			cerr << " - Iteration " << i + 1 << "/" << NB_ITERATIONS << " \t|"
				 << " Nb Thymio : " <<  nbThymio << "\t|"
				 << " Time : " << timeSerialization << endl;
		}

		cerr << " ----- " << endl
			 << "[Resume - Time Serialization Update] World with Thymio : " << endl;
		displayPerf(perfNbThymio);
		displayPerf(perfTime);
		cerr << endl;
	}
	
	
	SECTION( "[Time Serialization Init] World with Physical Objects" ) {
		cerr << "[Time Serialization Init] World with Physical Objects : "  << endl
			 << "|  nb   |  nb Physical Objects  |   Time Serialization  |" << endl
			 << "| Hull  |  min  |  max  |  mean |  min  |  max  |  mean |" << endl
			 << "|-------|-------|-------|-------|-------|-------|-------|" << endl;
		
		int nbHull = 50;
		while (true) {
			perfs perfTime;
			perfTime.name = "Time of Serialization";
			perfs perfNbPhysObj;
			perfNbPhysObj.name = "Number of Physical Objects";

			int nbPhysObj = 0;
			for (int i = 0; i < NB_ITERATIONS; ++i) {
				World* world = new World();

				double timeSerialization = 0;
				while (true) {
					timepoint start = chrono::system_clock::now();
					string strSerialization = serialize(world, true);
					timepoint end = chrono::system_clock::now();
			
					if (msTime(start, end) > MAX_TIME) { break; }
					timeSerialization = msTime(start, end);
					world->addObject(createPhysObj(nbHull));
				}
		   
				nbPhysObj = world->objects.size();
				
				delete world;
				if (nbPhysObj < 50) {
				    REQUIRE(nbHull != 0);
					break;
				}

				updatePerfs(&perfTime, timeSerialization);
				updatePerfs(&perfNbPhysObj, nbPhysObj);
			}

			if (nbPhysObj < 50) { break; }

			cerr.precision(4);
			cerr << "| "  << nbHull             << "\t"
				 << "|  " << perfNbPhysObj.min  << "\t"
				 << "|  " << perfNbPhysObj.max  << "\t"
				 << "|  " << perfNbPhysObj.mean << "\t"
				 << "| "  << perfTime.min       << "\t"
				 << "| "  << perfTime.max       << "\t"
				 << "| "  << perfTime.mean      << "\t|"
				 << endl;
	
			nbHull++;
		}
		
		cerr << endl;
	}
	
	
	SECTION( "[Time Serialization Update] World with Physical Objects" ) {
		cerr << "[Time Serialization Update] World with Physical Objects : " << endl;

		perfs perfTime;
		perfTime.name = "Time of Serialization";
		perfs perfNbPhysObj;
		perfNbPhysObj.name = "Number of Physical Objects";
		
		for (int i = 0; i < NB_ITERATIONS; ++i) {
			World* world = new World();

			double timeSerialization = 0;
			while (true) {
				timepoint start = chrono::system_clock::now();
				string strSerialization = serialize(world, false);
				timepoint end = chrono::system_clock::now();
			
				if (msTime(start, end) > MAX_TIME) { break; }
				timeSerialization = msTime(start, end);
				world->addObject(new PhysicalObject());
			}

			int nbPhysObj = world->objects.size();
			
			delete world;
			if (nbPhysObj < 50) { break; }

			updatePerfs(&perfTime, timeSerialization);
			updatePerfs(&perfNbPhysObj, nbPhysObj);

			cerr << " - Iteration " << i + 1 << "/" << NB_ITERATIONS << " \t|"
				 << " Nb Physical Objects : " <<  nbPhysObj << "\t|"
				 << " Time : " << timeSerialization << endl;
		}
			
		cerr << " ----- " << endl
			 << "[Resume - Time Serialization Update] World with Physical Objects : " << endl;
		displayPerf(perfNbPhysObj);
		displayPerf(perfTime);
		cerr << endl;
	}

	SECTION( "[Time Serialization Init] World with 30 Thymio & (at least) 50 Physical Objects" ) {
		cerr << "[Time Serialization Init] World with 30 Thymio & (at least) 50 Physical Objects : " << endl;

		cerr << "|  nb   |  nb Physical Objects  |   Time Serialization  |" << endl
			 << "| Hull  |  min  |  max  |  mean |  min  |  max  |  mean |" << endl
			 << "|-------|-------|-------|-------|-------|-------|-------|" << endl;
		
		int nbHull = 50;
		while (true) {
			perfs perfTime;
			perfTime.name = "Time of Serialization";
			perfs perfNbPhysObj;
			perfNbPhysObj.name = "Number of Physical Objects";
						
			int nbPhysObj = 0;
			for (int i = 0; i < NB_ITERATIONS; ++i) {
				World* world = new World();
				for (int i = 0; i < 30; i++) { world->addObject(new Thymio2()); }
				for (int i = 0; i < 49; i++) { world->addObject(createPhysObj(nbHull)); }

				double timeSerialization = 0;
				while (true) {
					timepoint start = chrono::system_clock::now();
					string strSerialization = serialize(world, true); // Serialize Init
					timepoint end = chrono::system_clock::now();

					if (msTime(start, end) > MAX_TIME) { break; }
					timeSerialization = msTime(start, end);
					world->addObject(createPhysObj(nbHull));
				}
			   
				nbPhysObj = world->objects.size() - 30;
				
				delete world;
				if (nbPhysObj < 50) {
					REQUIRE(nbHull != 0);
					break;
				}
				
				updatePerfs(&perfTime, timeSerialization);
				updatePerfs(&perfNbPhysObj, nbPhysObj);
			}
			
			if (nbPhysObj < 50) { break; }
			
			cerr.precision(4);
			cerr << "| "  << nbHull             << "\t"
				 << "|  " << perfNbPhysObj.min  << "\t"
				 << "|  " << perfNbPhysObj.max  << "\t"
				 << "|  " << perfNbPhysObj.mean << "\t"
				 << "| " << perfTime.min        << "\t"
				 << "| " << perfTime.max        << "\t"
				 << "| " << perfTime.mean       << "\t|"
				 << endl;
			
			nbHull++;
		}
		
		cerr << endl;	
	}
	
	SECTION( "[Time Serialization Update] World with 30 Thymio & (at least) 50 Physical Objects" ) {
		cerr << "[Time Serialization Update] World with 30 Thymio & (at least) 50 Physical Objects : " << endl;

		perfs perfTime;
		perfTime.name = "Time of Serialization";
		perfs perfNbPhysObj;
		perfNbPhysObj.name = "Number of Physical Objects";

		for (int i = 0; i < NB_ITERATIONS; ++i) {
			World* world = new World();
			for (int i = 0; i < 30; i++) { world->addObject(new Thymio2()); }
			for (int i = 0; i < 50; i++) { world->addObject(new PhysicalObject()); }

			double timeSerialization = 0;
			while (true) {
				timepoint start = chrono::system_clock::now();
				string strSerialization = serialize(world, false); // Serialize Update
				timepoint end = chrono::system_clock::now();
	   
				if (msTime(start, end) > MAX_TIME) { break; }
				timeSerialization = msTime(start, end);
				world->addObject(new PhysicalObject());
			}
			
			int nbPhysObj = world->objects.size() - 30;
			
			delete world;
			REQUIRE(timeSerialization != 0);
			
			updatePerfs(&perfTime, timeSerialization);
			updatePerfs(&perfNbPhysObj, nbPhysObj);
			
			cerr << "   - Iteration " << i + 1 << "/" << NB_ITERATIONS << " \t|"
				 << " Nb PhysObj : " << nbPhysObj << "\t|"
				 << " Nb Thymio : 30 \t| " 
				 << " Time : " << timeSerialization << endl;
		}
		
		cerr << " ----- " << endl
			 << "[Resume - Time Serialization Update] World with 30 Thymio & (at least) 50 Physical Objects : " << endl;
		displayPerf(perfNbPhysObj);
		displayPerf(perfTime);		
		cerr << endl;
	}
}

TEST_CASE( "BenchMarks Time Deserialization", "[BenchMarks Time Deserialization]" ) {
	
	SECTION( "[Time Deserialization Init] World with Thymio" ) {
		cerr << "[Time Deserialization Init] World with Thymio : " << endl;

		perfs perfTime;
		perfTime.name = "Time of Deserialization";
		perfs perfNbThymio;
		perfNbThymio.name = "Number of Thymio";
		
		for (int i = 0; i < NB_ITERATIONS; ++i) {
			World* world = new World();

			double timeSerialization = 0;
			while (true) {
				string strSerialization = serialize(world, true);
				timepoint start = chrono::system_clock::now();
				World* newWorld = initWorld(strSerialization);
				timepoint end = chrono::system_clock::now();
				delete newWorld;
			
				if (msTime(start, end) > MAX_TIME) { break; }
				timeSerialization = msTime(start, end);
				world->addObject(new Thymio2());
			}
			
			int nbThymio = world->objects.size();

			delete world;

			updatePerfs(&perfTime, timeSerialization);
			updatePerfs(&perfNbThymio, nbThymio);
			
			cerr << " - Iteration " << i + 1 << "/" << NB_ITERATIONS << " \t|"
				 << " Nb Thymio : " <<  nbThymio << "\t|"
				 << " Time : " << timeSerialization << endl;
		}

		cerr << " ----- " << endl
			 << "[Resume - Time Deserialization Init] World with Thymio : " << endl;
		displayPerf(perfNbThymio);
		displayPerf(perfTime);
		cerr << endl;
	}
	
	SECTION( "[Time Deserialization Update] World with Thymio" ) {
		cerr << "[Time Deserialization Update] World with Thymio : " << endl;

		perfs perfTime;
		perfTime.name = "Time of Deserialization";
		perfs perfNbThymio;
		perfNbThymio.name = "Number of Thymio";
		
		for (int i = 0; i < NB_ITERATIONS; ++i) {
			World* world = new World();

			double timeSerialization = 0;
			while (true) {
				string strSerialization = serialize(world, false);
				timepoint start = chrono::system_clock::now();
                deserialize(world, strSerialization, Enki::Separator::a, false);
				timepoint end = chrono::system_clock::now();
				
				if (msTime(start, end) > MAX_TIME) { break; }
				timeSerialization = msTime(start, end);
				world->addObject(new Thymio2());
			}
			
			int nbThymio = world->objects.size();

			delete world;
			REQUIRE(nbThymio >= 30);

			updatePerfs(&perfTime, timeSerialization);
			updatePerfs(&perfNbThymio, nbThymio);
			
			cerr << " - Iteration " << i + 1 << "/" << NB_ITERATIONS << " \t|"
				 << " Nb Thymio : " <<  nbThymio << "\t|"
				 << " Time : " << timeSerialization << endl;
		}
	
		cerr << "--------------------------------" << endl
			 << "[Resume - Time Deserialization Update] World with Thymio : " << endl;
		displayPerf(perfNbThymio);
		displayPerf(perfTime);
		cerr << endl;
	}

	
	SECTION( "[Time Deserialization Init] World with Physical Objects" ) {
		cerr << "[Time Deserialization Init] World with Physical Objects :" << endl
			 << "|  nb   |  nb Physical Objects  |  Time Deserialization |" << endl
			 << "| Hull  |  min  |  max  |  mean |  min  |  max  |  mean |" << endl
			 << "|-------|-------|-------|-------|-------|-------|-------|" << endl;
		
		int nbHull = 50;
		while (true) {
			perfs perfTime;
			perfTime.name = "Time of Serialization";
			perfs perfNbPhysObj;
			perfNbPhysObj.name = "Number of Physical Objects";

			int nbPhysObj = 0;
			for (int i = 0; i < NB_ITERATIONS; ++i) {
				World* world = new World();

				double timeSerialization = 0;
				while (true) {
					string strSerialization = serialize(world, true);
					timepoint start = chrono::system_clock::now();
					World* newWorld = initWorld(strSerialization);
					timepoint end = chrono::system_clock::now();
					delete newWorld;
					
					if (msTime(start, end) > MAX_TIME) { break; }
					timeSerialization = msTime(start, end);
					world->addObject(createPhysObj(nbHull));
				}
		   
				nbPhysObj = world->objects.size();
				delete world;
				if (nbPhysObj < 50) { break; }

				updatePerfs(&perfTime, timeSerialization);
				updatePerfs(&perfNbPhysObj, nbPhysObj);
			}

			if (nbPhysObj < 50) { break; }

			cerr.precision(4);
			cerr << "| "  << nbHull             << "\t"
				 << "|  " << perfNbPhysObj.min  << "\t"
				 << "|  " << perfNbPhysObj.max  << "\t"
				 << "|  " << perfNbPhysObj.mean << "\t"
				 << "| " << perfTime.min        << "\t"
				 << "| " << perfTime.max        << "\t"
				 << "| " << perfTime.mean       << "\t|"
				 << endl;
	
			nbHull++;
		}
		cerr << endl;
	}
	
	SECTION( "[Time Deserialization Update] World with Physical Objects" ) {
		cerr << "[Time Deserialization Update] World with Physical Objects : " << endl;

		perfs perfTime;
		perfTime.name = "Time of Deserialization";
		perfs perfNbPhysObj;
		perfNbPhysObj.name = "Number of Physical Objects";
		
		for (int i = 0; i < NB_ITERATIONS; ++i) {
			World* world = new World();

			double timeSerialization = 0;
			while (true) {
				string strSerialization = serialize(world, false);
				timepoint start = chrono::system_clock::now();
                deserialize(world, strSerialization, Enki::Separator::a, false);
				timepoint end = chrono::system_clock::now();
				
				if (msTime(start, end) > MAX_TIME) { break; }
				timeSerialization = msTime(start, end);
				world->addObject(new PhysicalObject());
			}

			int nbPhysObj = world->objects.size();
			delete world;

			updatePerfs(&perfTime, timeSerialization);
			updatePerfs(&perfNbPhysObj, nbPhysObj);

			cerr << " - Iteration " << i + 1 << "/" << NB_ITERATIONS << " \t|"
				 << " Nb Physical Objects : " <<  nbPhysObj << "\t|"
				 << " Time : " << timeSerialization << endl;
		}
			
		cerr << " ----- " << endl
			 << "[Resume - Time Deserialization Update] World with Physical Objects : " << endl;
		displayPerf(perfNbPhysObj);
		displayPerf(perfTime);
		cerr << endl;
	}
																				 
	SECTION( "[Time Deserialization Init] World with 30 Thymio & (at least) 50 Physical Objects" ) {
		cerr << "[Time Deserialization Init] World with 30 Thymio & (at least) 50 Physical Objects : " << endl
			 << "|  nb   |  nb Physical Objects  |  Time Deserialization |" << endl
			 << "| Hull  |  min  |  max  |  mean |  min  |  max  |  mean |" << endl
			 << "|-------|-------|-------|-------|-------|-------|-------|" << endl;
		
		
		int nbHull = 50;
		while (true) {
			perfs perfTime;
			perfTime.name = "Time of Deserialization";
			perfs perfNbPhysObj;
			perfNbPhysObj.name = "Number of Physical Objects";
						
			int nbPhysObj = 0;
			for (int i = 0; i < NB_ITERATIONS; ++i) {
				World* world = new World();
				for (int i = 0; i < 30; i++) { world->addObject(new Thymio2()); }
				for (int i = 0; i < 49; i++) { world->addObject(createPhysObj(nbHull)); }

				double timeSerialization = 0;
				while (true) {
					string strSerialization = serialize(world, true);
					timepoint start = chrono::system_clock::now();
					World* newWorld = initWorld(strSerialization);
					timepoint end = chrono::system_clock::now();
					delete newWorld;
					
					if (msTime(start, end) > MAX_TIME) { break; }
					timeSerialization = msTime(start, end);
					world->addObject(createPhysObj(nbHull));
				}
			   
				nbPhysObj = world->objects.size() - 30;
				delete world;
				if (nbPhysObj < 50) { break; }
				
				updatePerfs(&perfTime, timeSerialization);
				updatePerfs(&perfNbPhysObj, nbPhysObj);
			}
			
			if (nbPhysObj < 50) { break; }
			
			cerr.precision(4);
			cerr << "| "  << nbHull             << "\t"
				 << "|  " << perfNbPhysObj.min  << "\t"
				 << "|  " << perfNbPhysObj.max  << "\t"
				 << "|  " << perfNbPhysObj.mean << "\t"
				 << "| " << perfTime.min        << "\t"
				 << "| " << perfTime.max        << "\t"
				 << "| " << perfTime.mean       << "\t|"
				 << endl;
			
			nbHull++;
		}
	}
	
	SECTION( "[Time Deserialization Update] World with 30 Thymio & (at least) 50 Physical Objects" ) {
		cerr << "[Time Deserialization Update] World with 30 Thymio & (at least) 50 Physical Objects : " << endl;

		perfs perfTime;
		perfTime.name = "Time of Deserialization";
		perfs perfNbPhysObj;
		perfNbPhysObj.name = "Number of Physical Objects";
		
		for (int i = 0; i < NB_ITERATIONS; ++i) {
			World* world = new World();
			for (int i = 0; i < 30; i++) { world->addObject(new Thymio2()); }
			for (int i = 0; i < 49; i++) { world->addObject(new PhysicalObject()); }

			double timeSerialization = 0;
			while (true) {
				string strSerialization = serialize(world, false);
				timepoint start = chrono::system_clock::now();
                deserialize(world, strSerialization, Enki::Separator::a, false);
				timepoint end = chrono::system_clock::now();
				
				if (msTime(start, end) > MAX_TIME) { break; }
				timeSerialization = msTime(start, end);
				world->addObject(new PhysicalObject());
			}
			
			int nbPhysObj = world->objects.size() - 30;
			delete world;
			
			updatePerfs(&perfTime, timeSerialization);
			updatePerfs(&perfNbPhysObj, nbPhysObj);
			
			cerr << "   - Iteration " << i + 1 << "/" << NB_ITERATIONS << " \t|"
				 << " Nb PhysObj : " << nbPhysObj << "\t|"
				 << " Nb Thymio : 30 \t| " 
				 << " Time : " << timeSerialization << endl;
		}
		
		cerr << " ----- " << endl
			 << "[Resume - Time Deserialization Update] World with 30 Thymio & (at least) 50 Physical Objects :" << endl;
		displayPerf(perfNbPhysObj);
		displayPerf(perfTime);		
		cerr << endl;
	}
}
