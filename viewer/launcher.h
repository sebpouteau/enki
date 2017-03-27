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

#ifndef __LAUNCHER_H_
#define __LAUNCHER_H_

#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QIntValidator>
#include <QGridLayout>
#include <QMainWindow>
#include <QStatusBar>

#include <thread>
#include <unistd.h>
#include <enki/Network.h>
#include <enki/Serialize.h>
#include <QCloseEvent>

#include <viewer/Viewer.h>
#include <enki/robots/thymio2/Thymio2.h>
#include <enki/robots/s-bot/Sbot.h>
#include <enki/robots/e-puck/EPuck.h>
#include <enki/robots/khepera/Khepera.h>
#include <enki/robots/marxbot/Marxbot.h>
#include <enki/worldgenerator/WorldGenerator.h>


#include <QMessageBox>
#include <QMenuBar>
#include <QIntValidator>
#include <QShortcut>

class ClientGui : public QMainWindow
{
	Q_OBJECT
public:
	//! Constructor of the clientGui
	ClientGui(QWidget *parent = 0);
	//! Destructor of the clientGui.
	~ClientGui() {
		if (client != NULL){
			Enki::World* w = client->getWorld();
			client->stop();
			m_thread->join();
			delete client;
			delete w;
		}
	};

public slots:
	//! This slot is activated when the join button is clicked.
	//! It runs a client with the specified parameters and tries to
	//! join a server.
	void joinAct();

	//! This slot is activated when the Help submenu of the Help menu
	//! is clicked. It displays the help window with informations on
	//! how you should fill the fields.
	void displayHelp();

	//! This slot is activated when the About submenu of the Help menu
	//! is clicked. It displays the About window with informations about
	//! the enki project and licences.
	void displayAbout();

	//! This slot locks the join button while the fields aren't correctly
	//! filled.
	void lockButton();


private:
	QPushButton* m_join;
	QLabel* m_port;
	QLabel* m_ip;
	QLineEdit* m_portEdit;
	QLineEdit* m_ipEdit;
	std::thread* m_thread;
	Enki::Client* client;
};

class HostGui : public QMainWindow
{
	Q_OBJECT
public:
	//! Constructor of the HostGui.
	//! The HostGui is a graphical interface that allows the user to run
	//! an Enki::Server.
	HostGui( QWidget *parent = 0);
	//! Destructor of the HostGui.
	~HostGui() {
		if (client != NULL){
			Enki::World* w = client->getWorld();
			client->stop();
			m_client_thread->join();
			delete client;
			delete w;
			delete m_gen;
		}
		if (server != NULL){
			server->stop();
			m_server_thread->join();
			delete server;
			server = NULL;
			delete m_world;
			m_world = NULL;
		}
	};

public slots:
	//! This slot is activated when the host button is clicked.
	//! It will run a Enki::Server with the specified parameters.
	//! It can also try to run an Enki::Client if needed.
	void hostAct();

	//! This slot is activated when the checkbox m_viewerCheck is
	//! clicked. If an Enki::Server is running, it will create
	//! an Enki::Client and join the server.
	void checkAct();

	//! This slot is activated when the Enki::Client window is closed.
	//! It is used to release memory and unlock m_viewerCheck.
	void clientClosed();

	//! This slot is activated when the Help submenu of the Help menu
	//! is clicked. It displays the help window with informations on
	//! how you should fill the fields.
	void displayHelp();

	//! This slot is activated when the About submenu of the Help menu
	//! is clicked. It displays the About window with informations about
	//! the enki project and licences.
	void displayAbout();

	Enki::World* createWorld();

private:
	void clientViewer();

	QPushButton* m_host;
	QLabel* m_port;
	QLabel* m_serverStatus;
	QLabel* m_viewerStatus;
	QLineEdit* m_portEdit;
	QCheckBox* m_viewerCheck;

	Enki::Client* client;
	std::thread * m_client_thread;

	Enki::Server* server;
	std::thread* m_server_thread;
	std::thread* m_timer_thread;

	Enki::World* m_world;
	Enki::WorldGenerator* m_gen;
	Enki::ViewerWidget* m_viewer;

};

class Launcher : public QMainWindow
{
	Q_OBJECT
public:
	Launcher(QWidget *parent = 0);
	~Launcher(){
		if (m_c != NULL)
			delete m_c;
		if (m_h != NULL)
			delete m_h;
	};

public slots:
	//! This slot is activated when the Server button is pushed.
	//! It opens a new HostGui.
	void serverAct();
	//! THis slot is activated when the Client button is pushed.
	//! It opens a new ClientGui.
	void clientAct();

private:
	QPushButton* m_client;
	QPushButton* m_server;
	ClientGui* m_c;
	HostGui* m_h;
};

#endif
