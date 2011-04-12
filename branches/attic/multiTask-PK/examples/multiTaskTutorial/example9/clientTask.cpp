/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id$ */

#include "clientTask.h"
#include "fltkMutex.h"

CMN_IMPLEMENT_SERVICES_TEMPLATED(clientTaskDouble);
CMN_IMPLEMENT_SERVICES_TEMPLATED(clientTaskmtsDouble);

template <class _dataType>
clientTask<_dataType>::clientTask(const std::string & taskName, double period):
    clientTaskBase(taskName, period)
{
    // to communicate with the interface of the resource
    mtsRequiredInterface * required = AddRequiredInterface("Required");
    if (required) {
        required->AddFunction("Void", this->VoidServer);
        required->AddFunction("Write", this->WriteServer);
        required->AddFunction("Read", this->ReadServer);
        required->AddFunction("QualifiedRead", this->QualifiedReadServer);
        required->AddEventHandlerVoid(&clientTask<_dataType>::EventVoidHandler, this, "EventVoid");
        required->AddEventHandlerWrite(&clientTask<_dataType>::EventWriteHandler, this, "EventWrite");
    }
}


template <class _dataType>
void clientTask<_dataType>::Configure(const std::string & CMN_UNUSED(filename))
{}


template <class _dataType>
void clientTask<_dataType>::Startup(void) 
{
    // make the UI visible
    fltkMutex.Lock();
    {
        UI.show(0, NULL);
        UI.Opened = true;
    }
    fltkMutex.Unlock();
    // check argument prototype for event handler
    mtsRequiredInterface * required = GetRequiredInterface("Required");
    CMN_ASSERT(required);
    mtsCommandWriteBase * eventHandler = required->GetEventHandlerWrite("EventWrite");
    CMN_ASSERT(eventHandler);
    std::cout << "Event handler argument prototype: " << *(eventHandler->GetArgumentPrototype()) << std::endl;
}


template <class _dataType>
void clientTask<_dataType>::EventWriteHandler(const _dataType & value)
{
    fltkMutex.Lock();
    {
        double result = (double)value + UI.EventValue->value();
        UI.EventValue->value(result);
    }
    fltkMutex.Unlock();
}


template <class _dataType>
void clientTask<_dataType>::EventVoidHandler(void)
{
    fltkMutex.Lock();
    {
        UI.EventValue->value(0);
    }
    fltkMutex.Unlock();
}


template <class _dataType>
void clientTask<_dataType>::Run(void)
{
    if (this->UIOpened()) {
        ProcessQueuedEvents();

        // check if toggle requested in UI
        fltkMutex.Lock();
        {
            if (UI.VoidRequested) {
                CMN_LOG_CLASS_RUN_VERBOSE << "Run: VoidRequested" << std::endl;
                this->VoidServer();
                UI.VoidRequested = false;
            }
            
            if (UI.WriteRequested) {
                CMN_LOG_CLASS_RUN_VERBOSE << "Run: WriteRequested" << std::endl;
                this->WriteServer(_dataType(UI.WriteValue->value()));
                UI.WriteRequested = false;
            }
            
            if (UI.ReadRequested) {
                CMN_LOG_CLASS_RUN_VERBOSE << "Run: ReadRequested" << std::endl;
                _dataType data;
                this->ReadServer(data);
                UI.ReadValue->value((double)data);
                UI.ReadRequested = false;
            }
            
            if (UI.QualifiedReadRequested) {
                CMN_LOG_CLASS_RUN_VERBOSE << "Run: QualifiedReadRequested" << std::endl;
                _dataType data;
                this->QualifiedReadServer(_dataType(UI.WriteValue->value()), data);
                UI.QualifiedReadValue->value(data);
                UI.QualifiedReadRequested = false;
            }
            Fl::check();
        }
        fltkMutex.Unlock();
    }
}


template class clientTask<double>;
template class clientTask<mtsDouble>;

/*
  Author(s):  Anton Deguet
  Created on: 2009-08-10

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/