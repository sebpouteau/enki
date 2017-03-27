/*
   Enki - a fast 2D robot simulator
   Copyright © 2017 Nicolas Palard <nicolas.palard@etu.u-bordeaux.fr>
   Copyright © 2017 Mathieu Lirzin <mathieu.lirzin@etu.u-bordeaux.fr>

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

#include "Randomizer.h"

using namespace Enki;

Randomizer::Randomizer(World* world, const long long int &seed)
{
	this->seed = seed == -1 ?
		std::chrono::system_clock::now().time_since_epoch().count() :
		seed;
	this->randomEngine.seed(this->seed);
	if (world->wallsType == World::WALLS_NONE)
	{
		this->world = randWorld();
	}
	else
	{
		this->world = world;
	}
	std::cerr << "seed: " << this->seed << std::endl;
}

Randomizer::Randomizer(const long long int &seed)
{
	/*
	Since c++11 you can call something like "super" constructor.
	Using: `Randomizer(randWorld(), seed);` would be more convenient
	but this won't work as the seed needs to be set	before any call of
	randWorld();
	*/
	this->seed = seed == -1 ?
		std::chrono::system_clock::now().time_since_epoch().count() :
		seed;
	this->randomEngine.seed(this->seed);
	this->world = randWorld();
	std::cerr << "seed: " << this->seed << std::endl;
}

Randomizer::~Randomizer()
{
	delete this->world;
}

long long int Randomizer::getSeed()
{
	return this->seed;
}

void Randomizer::setSeed(const long long int &seed)
{
	this->seed = seed;
	this->randomEngine.seed(seed);
	std::cerr << "set seed: " << this->seed << std::endl;
}

World* Randomizer::getWorld()
{
	return this->world;
}

void Randomizer::resetWorld()
{
	delete this->world;
	this->world = randWorld();
}

World* Randomizer::randWorld()
{
	int wallsType = randInt(0, 1);

	Color color = randColor();

	int groundTexture = randBool();

	if (wallsType == World::WALLS_SQUARE)
	{
		int width = randFloat(MIN_WIDTH, MAX_WIDTH);
		int height = randFloat(MIN_HEIGHT, MAX_HEIGHT);

		if (groundTexture)
		{
			World::GroundTexture gt = randGroundTexture(width/4, height/4);
			return new World(width, height, color, gt);
		}
		return new World(width, height, color);
	}
	else if (wallsType == World::WALLS_CIRCULAR)
	{
		int radius = randFloat(MIN_RADIUS, MAX_RADIUS);
		if (groundTexture)
		{
			World::GroundTexture gt = randGroundTexture(radius/4, radius/4);
			return new World(radius, color, gt);
		}
		return new World(radius, color);
	}
}

PhysicalObject* Randomizer::randObject()
{
	int which = randBool();
	return which ?
		randRobot() :
		randPhysicalObject();
}

PhysicalObject* Randomizer::randPhysicalObject(const int &hullSize)
{
	PhysicalObject* object = new PhysicalObject();
	object->pos = randPoint();
	object->angle = randFloat(0.0, 2*M_PI);
	object->setColor(randColor());

 	if (hullSize == 0)
	{
		object->setCylindric(randFloat(1.0, 5.0), randFloat(1.0, 5.0), randFloat(1.0, 5.0));
	}
	else
	{
		object->setCustomHull(randHull(hullSize), randInt(1, 50));
	}

	return object;
}

Robot* Randomizer::randRobot(int type)
{
	Robot* r;
	type = type == -1 ?
		randInt(0, NUMBER_OF_ROBOTS_TYPES - 1) :
		type;
	switch (type)
	{
		case THYMIO2_:
			r = randThymio();
			break;
		case EPUCK_:
			r = randEPuck();
			break;
		case SBOT_:
			r = randSbot();
			break;
		case MARXBOT_:
			r = randMarxbot();
			break;
		case KHEPERA_:
			r = randKhepera();
			break;
	}

	return r;
}

Thymio2* Randomizer::randThymio()
{
	Thymio2* thymio = new Thymio2();

	thymio->setLedColor(Thymio2::TOP, randColor());
	thymio->setLedColor(Thymio2::BOTTOM_LEFT, randColor());
	thymio->setLedColor(Thymio2::BOTTOM_RIGHT, randColor());

	thymio->setLedIntensity(Thymio2::BUTTON_UP, randColorFloat());
	thymio->setLedIntensity(Thymio2::BUTTON_DOWN, randColorFloat());
	thymio->setLedIntensity(Thymio2::BUTTON_LEFT, randColorFloat());
	thymio->setLedIntensity(Thymio2::BUTTON_RIGHT, randColorFloat());

	thymio->setLedIntensity(Thymio2::RING_0, randColorFloat());
	thymio->setLedIntensity(Thymio2::RING_1, randColorFloat());
	thymio->setLedIntensity(Thymio2::RING_2, randColorFloat());
	thymio->setLedIntensity(Thymio2::RING_3, randColorFloat());
	thymio->setLedIntensity(Thymio2::RING_4, randColorFloat());
	thymio->setLedIntensity(Thymio2::RING_5, randColorFloat());
	thymio->setLedIntensity(Thymio2::RING_6, randColorFloat());
	thymio->setLedIntensity(Thymio2::RING_7, randColorFloat());

	thymio->setLedIntensity(Thymio2::IR_FRONT_0, randColorFloat());
	thymio->setLedIntensity(Thymio2::IR_FRONT_1, randColorFloat());
	thymio->setLedIntensity(Thymio2::IR_FRONT_2, randColorFloat());
	thymio->setLedIntensity(Thymio2::IR_FRONT_3, randColorFloat());
	thymio->setLedIntensity(Thymio2::IR_FRONT_4, randColorFloat());
	thymio->setLedIntensity(Thymio2::IR_FRONT_5, randColorFloat());

	thymio->setLedIntensity(Thymio2::IR_BACK_0, randColorFloat());
	thymio->setLedIntensity(Thymio2::IR_BACK_1, randColorFloat());

	thymio->setLedIntensity(Thymio2::LEFT_RED, randColorFloat());
	thymio->setLedIntensity(Thymio2::LEFT_BLUE, randColorFloat());
	thymio->setLedIntensity(Thymio2::RIGHT_BLUE, randColorFloat());
	thymio->setLedIntensity(Thymio2::RIGHT_RED, randColorFloat());

	thymio->pos = randPoint();
	thymio->angle = randFloat(0.0, 2*M_PI);
	thymio->leftSpeed = randFloat(0.0, 10.0);
	thymio->rightSpeed = randFloat(0.0, 10.0);

	return thymio;
}

EPuck* Randomizer::randEPuck()
{
	EPuck* epuck = new EPuck();
	epuck->pos = randPoint();
	epuck->angle = randFloat(0.0, 2*M_PI);
	epuck->leftSpeed = randFloat(0.0, 10.0);
	epuck->rightSpeed = randFloat(0.0, 10.0);
	return epuck;
}

Khepera* Randomizer::randKhepera()
{
	Khepera* khepera = new Khepera();
	khepera->pos = randPoint();
	khepera->angle = randFloat(0.0, 2*M_PI);
	khepera->leftSpeed = randFloat(0.0, 10.0);
	khepera->rightSpeed = randFloat(0.0, 10.0);
	return khepera;
}

Sbot* Randomizer::randSbot()
{
	Sbot* sbot = new Sbot();
	sbot->setColor(randColor());
	sbot->pos = randPoint();
	sbot->angle = randFloat(0.0, 2*M_PI);
	sbot->leftSpeed = randFloat(0.0, 10.0);
	sbot->rightSpeed = randFloat(0.0, 10.0);
	return sbot;
}

Marxbot* Randomizer::randMarxbot()
{
	Marxbot* marxbot = new Marxbot();
	marxbot->setColor(randColor());
	marxbot->pos = randPoint();
	marxbot->angle = randFloat(0.0, 2*M_PI);
	marxbot->leftSpeed = randFloat(0.0, 10.0);
	marxbot->rightSpeed = randFloat(0.0, 10.0);
	return marxbot;
}

PhysicalObject::Hull Randomizer::randHull(int hullSize)
{
	PhysicalObject::Hull hull;

	hullSize = hullSize <= 0 ?
		randInt(1, 30) :
		hullSize;
	int complex = randBool();
	for (int i = 0; i < hullSize; i++)
	{
		PhysicalObject::Part p = (complex ?
			randComplexPart() :
			randRectanglePart());
		hull.push_back(p);
	}

	return hull;
}

PhysicalObject::Part Randomizer::randComplexPart()
{
	Polygone p = randConvexPolygone(randInt(3, 10));
	int textured = randBool();
	if (textured)
	{
		Textures t = randTextures(p.size());
		return PhysicalObject::Part(p, randFloat(1.0, 5.0), t);
	}

	return PhysicalObject::Part(p, randFloat(1.0, 5.0));
}

PhysicalObject::Part Randomizer::randRectanglePart()
{
	float size1 = randFloat(1.0, 15.0), size2 = randFloat(1.0, 15.0), height = randFloat(1.0, 15.0);
	return PhysicalObject::Part(size1, size2, height);
}

Polygone Randomizer::randConvexPolygone(int polygoneSize)
{
	polygoneSize = polygoneSize < 3 ?
		3 :
		polygoneSize;

	// This uses simple circle based trigonometry to compute a convex polygon.
	std::vector<float> points;
	for (int i = 0; i < polygoneSize; i++)
	{
		points.push_back(randFloat(0.0, 2*M_PI));
	}

	std::sort(points.begin(), points.end());

	Polygone p;
	int center_x = 0, center_y = 0;
	int radius = randInt(1, 7);
	for (int i = 0; i < points.size(); i++)
	{
		float posx = center_x + radius * cos(points.at(i));
		float posy = center_y + radius * sin(points.at(i));
		p << Point(posx, posy);
	}

	points.clear();
	return p;
}

Point Randomizer::randPoint()
{
	float posx, posy;
	if (this->world->wallsType == World::WALLS_SQUARE)
	{
		posx = randFloat(0, this->world->w);
		posy = randFloat(0, this->world->h);
	}
	else if (this->world->wallsType == World::WALLS_CIRCULAR)
	{
		bool in = false;
		while (!in)
		{
			posx = randFloat(-this->world->r, this->world->r);
			posy = randFloat(-this->world->r, this->world->r);
			in = (posx * posx) + (posy * posy) <= this->world->r * this->world->r ? true : false;
		}
	}

	return Point(posx, posy);
}

Color Randomizer::randColor()
{
	float r = randColorFloat();
	float g = randColorFloat();
	float b = randColorFloat();
	// We do not generate the alpha channel between 0 & 1 otherwise we're not seeing anything
	float a = randFloat(0.6, 1.0);

	return Color(r, g, b, a);
}

Texture Randomizer::randTexture(int nbColor)
{
	nbColor = nbColor <= 0 ?
		randInt(1, 5) :
		nbColor;
	Texture t;
	for (int i = 0; i < nbColor; i++)
	{
		t.push_back(randColor());
	}

	return t;
}

Textures Randomizer::randTextures(int nbTexture)
{
	nbTexture = nbTexture <= 0 ?
		randInt(1, 5) :
		nbTexture;
	Textures t;
	for (int i = 0; i < nbTexture; i++)
	{
		t.push_back(randTexture());
	}

	return t;
}

World::GroundTexture Randomizer::randGroundTexture(float width, float height)
{
	width = width <= 0 ?
		randFloat(1.0, 50.0) :
		width;
	height = height <= 0 ?
		randFloat(1.0, 50.0) :
		height;

	std::vector<uint32_t> data;
	for (int i = 0; i < width * height; i++)
	{
		data.push_back(Color::toARGB(randColor()));
	}

	return World::GroundTexture(width, height, data.data());
}

float Randomizer::randFloat(const float& min, const float &max)
{
	return std::uniform_real_distribution<>(min, max)(this->randomEngine);
}

int Randomizer::randInt(const int &min, const int &max)
{
	return std::uniform_int_distribution<>(min, max)(this->randomEngine);
}

float Randomizer::randColorFloat()
{
	return this->color_distr(this->randomEngine);
}

int Randomizer::randBool()
{
	return this->bool_distr(this->randomEngine);
}
