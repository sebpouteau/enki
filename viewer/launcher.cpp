/*
   Enki - a fast 2D robot simulator
   Copyright © 2017 Nicolas Palard <nicolas.palard@etu.u-bordeaux.fr>
   Copyright © 2017 Sebastien Pouteau <sebastien.pouteau@etu.u-bordeaux.fr>

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

#include "launcher.h"

Enki::World* HostGui::createWorld() {
	m_gen = new Enki::WorldGenerator();
	m_gen->add(PHYSICAL_OBJECTS_, 30);
	m_gen->add(ONLY_ROBOTS_, 30);
	Enki::World* w = m_gen->getWorld();
	return w;
}

Launcher::Launcher(QWidget *parent) : QMainWindow(), m_c(NULL), m_h(NULL)
{
	this->setWindowTitle("Enki");
	this->setFixedSize(150, 80);
	this->setAttribute(Qt::WA_AlwaysShowToolTips);

	QWidget *centralWidget = new QWidget;
	QGridLayout* mainLayout = new QGridLayout();
	mainLayout->setSpacing(2);
	mainLayout->setContentsMargins(11, 11, 11, 11);
	mainLayout->setHorizontalSpacing(1);

	m_client = new QPushButton("Client");
	m_client->setToolTip("Join a server.");
	m_server = new QPushButton("Server");
	m_server->setToolTip("Host a server.");

	mainLayout->addWidget(m_client, 0, 0, 1, 1);
	mainLayout->addWidget(m_server, 1, 0, 1, 1);
	centralWidget->setLayout(mainLayout);

	this->setCentralWidget(centralWidget);

	QObject::connect(m_client, SIGNAL(clicked()), this, SLOT(clientAct()));
	QObject::connect(m_server, SIGNAL(clicked()), this, SLOT(serverAct()));
}

void Launcher::clientAct()
{
	this->close();
	m_c = new ClientGui(this);
	m_c->show();
}

void Launcher::serverAct()
{
	this->close();
	m_h = new HostGui(this);
	m_h->show();
}

ClientGui::ClientGui(QWidget *parent) : QMainWindow()
{
	client = NULL;
	this->setWindowTitle("Enki Client");
	this->setFixedSize(295, 115);
	this->setAttribute(Qt::WA_AlwaysShowToolTips);

	QWidget *centralWidget = new QWidget;
	QGridLayout* mainLayout = new QGridLayout();
	mainLayout->setSpacing(6);
	mainLayout->setContentsMargins(11, 11, 11, 11);

	mainLayout->setHorizontalSpacing(10);

	// Ip label.
	m_ip = new QLabel();
	m_ip->setStyleSheet("font-weight:bold");
	m_ip->setText("Ip adress:");

	// Port label.
	m_port = new QLabel();
	m_port->setStyleSheet("font-weight:bold");
	m_port->setText("Port:");

	// Ip text field.
	m_ipEdit = new QLineEdit();
	m_ipEdit->setPlaceholderText("ex: 127.0.0.1");

	// Port text field.
	m_portEdit = new QLineEdit();
	m_portEdit->setPlaceholderText("ex: 80");
	m_portEdit->setValidator(new QIntValidator(0, 65535, 0));

	// Join button.
	m_join = new QPushButton();
	m_join->setText("Join");
	m_join->setToolTip("Please enter a valid <b> Ip adress </b> and a valid <b>Port</b> before any attempt at joining a server."
					   "<br> If you need help, you will find usefull informations within the <b>Help menu</b>.");
	m_join->setEnabled(false);

	mainLayout->addWidget(m_ip, 0, 0, 1, 1);
	mainLayout->addWidget(m_port, 0, 1, 1, 1);
	mainLayout->addWidget(m_ipEdit, 1, 0, 1, 1);
	mainLayout->addWidget(m_portEdit, 1, 1, 1, 1);
	mainLayout->addWidget(m_join, 2, 1, 1, 1);

	// Menu.
	QMenuBar *menuBar = new QMenuBar(this);

	// File menu.
	QMenu *fileMenu = menuBar->addMenu("File");

	QAction *quit = new QAction("Quit", this);
	quit->setShortcut(QKeySequence("Ctrl+Q"));

	fileMenu->addAction(quit);

	// Help menu.
	QMenu *helpMenu = menuBar->addMenu("Help");

	QAction *help = new QAction("Help", this);
	QAction *about = new QAction("About", this);

	helpMenu->addAction(help);
	helpMenu->addAction(about);

	centralWidget->setLayout(mainLayout);
	setCentralWidget(centralWidget);
	// Setting focus to a label in order to see the placeholders.
	m_ip->setFocus();

	this->setMenuBar(menuBar);

	// Connecting widgets
	QObject::connect(quit, SIGNAL(triggered()), qApp, SLOT(quit()));
	QObject::connect(m_join, SIGNAL(clicked()), this, SLOT(joinAct()));
	QObject::connect(about, SIGNAL(triggered()), this, SLOT(displayAbout()));
	QObject::connect(help, SIGNAL(triggered()), this, SLOT(displayHelp()));

	// Locking/Unlocking join button
	QObject::connect(m_ipEdit, SIGNAL(textEdited(const QString&)), this, SLOT(lockButton()));
	QObject::connect(m_portEdit, SIGNAL(textEdited(const QString&)), this, SLOT(lockButton()));
}

void ClientGui::lockButton()
{
	if (m_ipEdit->text() == "") {
		m_join->setToolTip("Please enter a valid <b>Ip adress</b>."
						   "<br> If you need help, you will find usefull informations within the <b>Help menu</b>.");
		m_join->setEnabled(false);
	}
	else if (m_portEdit->text() == "" )
	{
		m_join->setToolTip("Please enter a valid <b>Port</b>."
						   "<br> If you need help, you will find usefull informations within the <b>Help menu</b>.");
		m_join->setEnabled(false);
	}
	else
	{
		m_join->setEnabled(true);
		m_join->setToolTip("Press the button to try connecting to an enki server.");
	}
}

void ClientGui::joinAct() {
	setlocale(LC_ALL, "C");

	try{
		client = new Enki::Client(m_ipEdit->text().toStdString(), m_portEdit->text().toInt());
		Enki::Client *c = client;
		m_thread = new std::thread([c]() { c->run(); });

		while(client->getWorld() == NULL) // Wait client thread initialization
		{
			sleep(1);
		}

		this->close();
		Enki::ViewerWidget* viewer = new Enki::ViewerWidget(client->getWorld(), 0);
		viewer->setWindowTitle("Client View");
		viewer->show();

	} catch (Dashel::DashelException e){
		QMessageBox::critical(0, "ERROR", e.what() );
	}
}

void ClientGui::displayAbout()
{
	QMessageBox m;
	QString s = "<h3>About Enki</h3>"
	"<p>Enki is an open source robot simulator written in C++. </p>"
	"<p>It provides collision and limited physics support for robots evolving on a flat surface. On a contemporary desktop computer, Enki is able to simulate groups of robots hundred times faster than real-time.<p>"
	"<p>Enki is free software released under <a href=\"http://www.gnu.org/copyleft/gpl.html\">the GNU General Public License version 2</a></p>"
	"<p>For more informations you can check "
	"<a href=\"http://www.github.com/enki-community/enki\">Enki's GitHub</a> and <a href=\"http://home.gna.org/enki/\"> Enki's website</a>.</p>"
	"<p> Enki Network is part of the Enki project and aims to provide a simple way of sharing an enki simulation.</p>";
	m.setText(s);
	m.exec();
}

void ClientGui::displayHelp()
{
	QMessageBox m;
	QString s = "<h1> Help </h1>"
	"<p>In this section you will find all the informations you need to fill up the fields.</p>"
	"<p>The <b>ip adress</b> field should be filled with one of the following ip formats : "
	"<ul>"
	"<li> localhost </li>"
	"<li> IPv4 <i> example: 193.43.55.67 </i></li>"
	"<li> IPv6 <i> example: 2001:0db8:0000:85a3:0000:0000:ac1f:8001</i> </li>"
	"<li> domain.name <i> example: mydomain.com </i> </li>"
	"</ul>"
	"<p>For more informations on Internet protocols (IP) you can check <a href=\"https://en.wikipedia.org/wiki/IPv4\">wikipedia IPv4</a> or <a href=\"https://en.wikipedia.org/wiki/IPv6\">wikipedia IPv6</a>. <a href=\"https://en.wikipedia.org/wiki/Domain_name\">wikipedia domain name </a></p>";
	m.setText(s);
	m.exec();
}


using namespace std::chrono;

// Period of timer
const milliseconds timerPeriod (30);

// Define a more precise type than std::chrono::seconds.
using dseconds = duration<double>;

HostGui::HostGui(QWidget *parent) : QMainWindow()
{
	client = NULL;
	m_viewer = NULL;
	server = NULL;
	m_world = createWorld();

	this->setWindowTitle("Enki Server");
	this->setFixedSize(295, 130);
	this->setAttribute(Qt::WA_AlwaysShowToolTips);

	QWidget *centralWidget = new QWidget;
	QGridLayout* mainLayout = new QGridLayout();
	mainLayout->setSpacing(6);
	mainLayout->setContentsMargins(11, 11, 11, 11);

	mainLayout->setHorizontalSpacing(10);

	// Viewer CheckBox
	m_viewerCheck = new QCheckBox();
	m_viewerCheck->setText("start enkiviewer");
	m_viewerCheck->setToolTip("Ticking this box will run a graphical rendering of the simulation.");

	// Port label.
	m_port = new QLabel();
	m_port->setStyleSheet("font-weight:bold");
	m_port->setText("Port:");

	// Server statut label.
	m_serverStatus= new QLabel("Not running");
	m_serverStatus->setStyleSheet("font-weight:bold; color:black");

	// Viewer statut label.
	m_viewerStatus= new QLabel("Not running");
	m_viewerStatus->setStyleSheet("font-weight:bold; color:black");

	// Port text field.
	m_portEdit = new QLineEdit();
	m_portEdit->setPlaceholderText("ex: 80");
	m_portEdit->setValidator(new QIntValidator(0, 65535, 0));

	// Join button.
	m_host = new QPushButton();
	m_host->setText("Host");
	m_host->setToolTip("<p>This will host a server on the specified <b>Port</b>.</p>"
					   "If the port isn't specified, we will find one for you.");

	mainLayout->addWidget(m_port, 0, 0, 1, 1);
	mainLayout->addWidget(m_viewerCheck, 2, 0, 1, 1);
	mainLayout->addWidget(m_portEdit, 0, 1, 1, 1);
	mainLayout->addWidget(m_host, 2, 1, 1, 1);

	// Menu.
	QMenuBar *menuBar = new QMenuBar(this);

	// File menu.
	QMenu *fileMenu = menuBar->addMenu("File");

	// Quit action & shortcut.
	QAction *quit = new QAction("Quit", this);
	quit->setShortcut(QKeySequence("Ctrl+Q"));

	fileMenu->addAction(quit);

	// Help menu.
	QMenu *helpMenu = menuBar->addMenu("Help");

	QAction *help = new QAction("Help", this);
	QAction *about = new QAction("About", this);

	helpMenu->addAction(help);
	helpMenu->addAction(about);
	this->setMenuBar(menuBar);

	centralWidget->setLayout(mainLayout);
	setCentralWidget(centralWidget);

	// Setting focus to a label in order to see the placeholders.
	m_port->setFocus();

	// Creating a status bar to display the current state of the server/viewer.
	QStatusBar* sBar = this->statusBar();
	sBar->addWidget(new QLabel("Server:"));
	sBar->addWidget(m_serverStatus);
	sBar->addWidget(new QLabel("Viewer:"));
	sBar->addWidget(m_viewerStatus);

	// Connecting widgets
	QObject::connect(quit, SIGNAL(triggered()), qApp, SLOT(quit()));
	QObject::connect(m_host, SIGNAL(clicked()), this, SLOT(hostAct()));
	QObject::connect(about, SIGNAL(triggered()), this, SLOT(displayAbout()));
	QObject::connect(help, SIGNAL(triggered()), this, SLOT(displayHelp()));
	QObject::connect(m_viewerCheck, SIGNAL(clicked()), this, SLOT(checkAct()));
}

void HostGui::hostAct() {

	try{
		server = new Enki::Server(m_world, m_portEdit->text().toInt() );
		Enki::Server **s =  &server;
		Enki::World **w = &m_world;
		m_server_thread = new std::thread([s]() { (*s)->run(); });
		sleep(1);
		m_timer_thread = new std::thread([s, w]() {
			while (true)
			{
				if ((*w) == NULL){
					break;
				}
				(*w)->step(duration_cast<dseconds>(timerPeriod).count(), 3);
				(*s)->sendAll();
				std::this_thread::sleep_for(timerPeriod);
			}
		});
		m_timer_thread->detach();

		m_serverStatus->setText("Running");
		m_serverStatus->setStyleSheet("font-weight:bold ; color:green");
		m_host->setEnabled(false);
		m_portEdit->setEnabled(false);

	} catch (Dashel::DashelException e){
		QMessageBox::critical(0, "ERROR", e.what() );
		m_serverStatus->setText("FAILED");
		m_serverStatus->setStyleSheet("font-weight:bold ; color:red");

	}
	
	if(m_viewerCheck->isChecked())
	{
		clientViewer();
	}
}


void HostGui::checkAct()
{
	// If the server isn't created or the checkbox isn't checked, don't try
	// to run a viewer
	if(m_viewerCheck->checkState() == Qt::Checked && !server)
	{
		return;
	}
	else
	{
		clientViewer();
	}
}

void HostGui::clientViewer(){
	m_viewer = new Enki::ViewerWidget(m_world, 0);
	m_viewer->setWindowTitle("Server View");
	m_viewer->setAttribute(Qt::WA_DeleteOnClose);
	// This is used to emit the destroyed() signal.
	QObject::connect(m_viewer, SIGNAL(destroyed()), this, SLOT(clientClosed()));
	
	m_viewerStatus->setText("Running");
	m_viewerStatus->setStyleSheet("font-weight:bold ; color:green");
	
	m_viewerCheck->setEnabled(false);
	m_viewer->show();
}

void HostGui::clientClosed()
{
	m_viewerCheck->setCheckState(Qt::Unchecked);
	m_viewerCheck->setEnabled(true);
	m_viewerStatus->setText("Closed");
	m_viewerStatus->setStyleSheet("font-weight:bold; color:black");
}

void HostGui::displayAbout()
{
	QMessageBox m;
	QString s = "<h3>About Enki</h3>"
	"<p>Enki is an open source robot simulator written in C++. </p>"
	"<p>It provides collision and limited physics support for robots evolving on a flat surface. On a contemporary desktop computer, Enki is able to simulate groups of robots hundred times faster than real-time.<p>"
	"<p>Enki is free software released under <a href=\"http://www.gnu.org/copyleft/gpl.html\">the GNU General Public License version 2</a></p>"
	"<p>For more informations you can check "
	"<a href=\"http://www.github.com/enki-community/enki\">Enki's GitHub</a> and <a href=\"http://home.gna.org/enki/\"> Enki's website</a>.</p>"
	"<p> Enki Network is part of the Enki project and aims to provide a simple way of sharing an enki simulation.</p>";
	m.setText(s);
	m.exec();
}

void HostGui::displayHelp()
{
	QMessageBox m;
	m.setWindowTitle("Enki Server");
	QString s = "<h1> Help </h1>"
	"<p> In this section you will find all the informations you need to fill up the fields."
	"<br/> The <b>port</b> field should be filled with the port on which you want to host the simulation."
	"<br/> If the port field is left empty, it will be set automatically."
	"<br/> <b>Note:</b> The port information will be displayed in the console.</p>"
	"<p> If checked, the checkbox \"start enkiviewer\" will start a graphical interface displaying the current hosted simulation."
	"<br/> <b>Note:</b><i> This option is generally used when you want to host a simulation on severs without a graphic card.</i></p>";
	m.setText(s);
	m.exec();
}

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	Launcher* w = new Launcher();
	w->show();
	app.exec();
	delete w;
}
