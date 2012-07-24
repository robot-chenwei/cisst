/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsSafetyCoordinator.h 3034 2011-10-09 01:53:36Z adeguet1 $

  Author(s):  Min Yang Jung
  Created on: 2012-07-14

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

#ifndef _mtsSafetyCoordinator_h
#define _mtsSafetyCoordinator_h

#include "coordinator.h"

#include <cisstMultiTask/mtsGenericObject.h>

#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsSafetyCoordinator: public mtsGenericObject, public SF::Coordinator 
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    mtsSafetyCoordinator();
    ~mtsSafetyCoordinator();

    bool AddMonitor(const std::string & targetUID, const std::string & monitorJsonSpec);

    void ToStream(std::ostream & outputStream) const;
    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsSafetyCoordinator);

#endif // _mtsSafetyCoordinator_h
