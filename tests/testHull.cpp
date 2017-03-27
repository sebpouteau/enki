#include <viewer/Viewer.h>
#include <enki/PhysicalEngine.h>
#include <QApplication>
#include <enki/worldgenerator/WorldGenerator.h>

using namespace Enki;

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	WorldGenerator* gen = new WorldGenerator();;
	gen->add(Randomizer::THYMIO2_, 10);

	ViewerWidget* viewer = new ViewerWidget(gen->getWorld(), 0);
	viewer->show();
	return app.exec();
}
