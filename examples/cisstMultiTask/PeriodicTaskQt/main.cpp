/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet, Ali Uneri
  Created on: 2009-10-22

  (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnLoggerQWidget.h>
#include <cisstCommon/cmnPath.h>
#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsCollectorEvent.h>

#include <QApplication>
#include <QTabWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QWidget>

#include "displayQComponent.h"
#include "mtsCollectorQComponent.h"
#include "mtsCollectorQWidget.h"
#include "sineTask.h"

const unsigned int NumSineTasks = 2;

int main(int argc, char *argv[])
{
    std::cout << cmnPath::GetWorkingDirectory() << std::endl;

    // log configuration
    cmnLogger::SetLoD(CMN_LOG_LOD_VERY_VERBOSE);
    cmnLogger::AddChannel(std::cout, CMN_LOG_LOD_VERY_VERBOSE);

    // add a log per thread
    osaThreadedLogFile threadedLog("PeriodicTaskQt-");
    cmnLogger::AddChannel(threadedLog, CMN_LOG_LOD_VERY_VERBOSE);

    // set the log level of detail on select tasks
    cmnClassRegister::SetLoD("sineTask", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("displayQComponent", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("mtsManagerLocal", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("mtsManagerGlobal", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("mtsCollectorQComponent", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("mtsCollectorState", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("mtsCollectorEvent", CMN_LOG_LOD_VERY_VERBOSE);
    cmnClassRegister::SetLoD("mtsStateTable", CMN_LOG_LOD_VERY_VERBOSE);

    // create Qt user interface
    QApplication application(argc, argv);

    // create a vertical widget for quit button and tabs
    QWidget * mainWidget = new QWidget();
    mainWidget->setWindowTitle("Periodic Task Example");
    QVBoxLayout * mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // tabs
    QTabWidget * tabs = new QTabWidget(mainWidget);
    mainLayout->addWidget(tabs);

    // create a tab with all the sine wave controllers
    QWidget * tab1Widget = new QWidget();
    QGridLayout * tab1Layout= new QGridLayout(tab1Widget);
    tab1Layout->setContentsMargins(0, 0, 0, 0);
    tabs->addTab(tab1Widget, "Main");


    // second tab for data collection
    QWidget * tab2Widget = new QWidget();
    QGridLayout * tab2Layout= new QGridLayout(tab2Widget);
    mtsCollectorQWidget * collectorQWidget = new mtsCollectorQWidget();
    tab2Layout->addWidget(collectorQWidget);
    tabs->addTab(tab2Widget, "Collection");

    // get the component manager to add multiple sine generator tasks
    mtsManagerLocal * taskManager = mtsManagerLocal::GetInstance();
    sineTask * sine;
    displayQComponent * display;
    mtsCollectorState * stateCollector;
    mtsCollectorQComponent * collectorQComponent;

    // create an event collector
    mtsCollectorEvent * eventCollector =
        new mtsCollectorEvent("EventCollector",
                              mtsCollectorBase::COLLECTOR_FILE_FORMAT_CSV);
    taskManager->AddComponent(eventCollector);
    // add QComponent to control the event collector
    collectorQComponent = new mtsCollectorQComponent("EventCollectorQComponent");
    taskManager->AddComponent(collectorQComponent);
    // connect to the existing widget
    collectorQComponent->ConnectToWidget(collectorQWidget);
    taskManager->Connect(collectorQComponent->GetName(), "DataCollection",
                         eventCollector->GetName(), "Control");

    // create multiple sine generators along with their widget and
    // state collectors
    for (unsigned int i = 0; i < NumSineTasks; i++) {
        std::ostringstream index;
        index << i;

        // create the generator and its widget
        sine = new sineTask("SIN" + index.str(), 5.0 * cmn_ms);
        taskManager->AddComponent(sine);
        std::cout << *sine << std::endl;
        display = new displayQComponent("DISP" + index.str());
        taskManager->AddComponent(display);
        tab1Layout->addWidget(display->GetWidget(), 1, i);
        taskManager->Connect(display->GetName(), "DataGenerator",
                             sine->GetName(), "MainInterface");

        // create the state collector and connect it to the generator
        stateCollector = new mtsCollectorState(sine->GetName(),
                                               sine->GetDefaultStateTableName(),
                                               mtsCollectorBase::COLLECTOR_FILE_FORMAT_CSV);
        stateCollector->AddSignal("SineData");
        taskManager->AddComponent(stateCollector);
        stateCollector->Connect();
        // create the QComponent to bridge between the collection widget and the collector
        collectorQComponent = new mtsCollectorQComponent(sine->GetName() + "StateCollectorQComponent");
        taskManager->AddComponent(collectorQComponent);
        collectorQComponent->ConnectToWidget(collectorQWidget);
        taskManager->Connect(collectorQComponent->GetName(), "DataCollection",
                             stateCollector->GetName(), "Control");

        // add events to observe
        eventCollector->AddObservedComponent(sine);
    }

    // connect all interfaces for event collector
    eventCollector->Connect();

    // third tab for logger widget
    QWidget * tab3Widget = new QWidget();
    QGridLayout * tab3Layout= new QGridLayout(tab3Widget);
    cmnLoggerQWidget * loggerWidget = new cmnLoggerQWidget(tab3Widget);
    tab3Layout->addWidget(loggerWidget->GetWidget());    
    tabs->addTab(tab3Widget, "Logger");

    // one large quit button under all tabs
    QPushButton * buttonQuit = new QPushButton("Quit", mainWidget);
    mainLayout->addWidget(buttonQuit);
    QObject::connect(buttonQuit, SIGNAL(clicked()),
                     QApplication::instance(), SLOT(quit()));

    // generate a nice tasks diagram
    std::ofstream dotFile("PeriodicTaskQt.dot");
    taskManager->ToStreamDot(dotFile);
    dotFile.close();

    // create and start all tasks
    taskManager->CreateAll();
    taskManager->StartAll();
    
    // run Qt user interface
    mainWidget->resize(NumSineTasks * 220, 360);
    mainWidget->show();
    application.exec();

    // kill all tasks and perform cleanup
    taskManager->KillAll();
    taskManager->Cleanup();

    // stop all logs
    cmnLogger::SetLoD(CMN_LOG_LOD_NONE);

    return 0;
}
