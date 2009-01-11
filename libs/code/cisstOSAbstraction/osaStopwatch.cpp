/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: osaStopwatch.cpp,v 1.4 2008/08/28 15:44:20 anton Exp $

  Author(s):  Ofri Sadowsky
  Created on: 2005-02-17

  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstOSAbstraction/osaStopwatch.h>

double osaStopwatch::GetTimeGranularity(void) const
{
    return this->TimeGranularity;
}
