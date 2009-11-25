/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlStreamTypeConverter.h 75 2009-02-24 16:47:20Z adeguet1 $
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2007 

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlFilterStreamTypeConverter_h
#define _svlFilterStreamTypeConverter_h

#include <cisstStereoVision/svlStreamManager.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>

class CISST_EXPORT svlFilterStreamTypeConverter : public svlFilterBase
{
public:
    svlFilterStreamTypeConverter(svlStreamType inputtype, svlStreamType outputtype);
    virtual ~svlFilterStreamTypeConverter();

    void SetScaling(float ratio) { Scaling = ratio; }
    float GetScaling() { return Scaling; }
    void SetMono16ShiftDown(unsigned int shiftdown) { Mono16ShiftDown = shiftdown; }
    unsigned int GetMono16ShiftDown() { return Mono16ShiftDown; }

protected:
    virtual int Initialize(svlSample* inputdata);
    virtual int ProcessFrame(ProcInfo* procInfo, svlSample* inputdata);
    virtual int Release();

private:
    float Scaling;
    unsigned int Mono16ShiftDown;
};

#endif // _svlFilterStreamTypeConverter_h

