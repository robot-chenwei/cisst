/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Min Yang Jung
  Created on: 2009-04-10

  (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsProxyBaseClient_h
#define _mtsProxyBaseClient_h

#include <cisstMultiTask/mtsProxyBaseCommon.h>

#include <cisstMultiTask/mtsExport.h>

/*!
  \ingroup cisstMultiTask

  This class is derived from mtsProxyBaseCommon and implements the basic 
  structure and functions for ICE proxy client.  The actual processing routine
  should be implemented by derived classes.

  Compared to mtsProxyBaseServer, this base class for Ice client proxy supports 
  a single connection to a server proxy because the current cisstMultiTask 
  design allows a required interface to connect to only one provided interface.
*/

template<class _proxyOwner>
class CISST_EXPORT mtsProxyBaseClient: public mtsProxyBaseCommon<_proxyOwner> 
{
protected:
    typedef mtsProxyBaseCommon<_proxyOwner> BaseType;

    /*! ICE Object */
    Ice::ObjectPrx ProxyObject;

    /*! Endpoint information used to connect to server. This information is
        feteched from the global component manager. */
    const std::string EndpointInfo;

    /*! Start proxy client. Gets called by user (application) */
    virtual bool StartProxy(_proxyOwner * proxyOwner) = 0;

    /*! Initialize Ice proxy client.  Called by StartProxy(). */
    virtual void IceInitialize(void);

    /*! Create ICE proxy client object */
    virtual void CreateProxy() = 0;

    /*! Called whenever server disconnection is detected */
    virtual bool OnServerDisconnect() = 0;

    /*! Remove ICE proxy client object */
    virtual void RemoveProxy() = 0;

    /*! Clean up ICE related resources */
    virtual void IceCleanup(void);

    // TODO: smmy This should be moved to derived classes
    /*! Stop and clean up proxy client */
    virtual void StopProxy(void);

    ///*! Shutdown the current session for graceful termination */
    //void ShutdownSession(const Ice::Current & current) {
    //    current.adapter->getCommunicator()->shutdown();
    //    BaseType::ShutdownSession();
    //}

public:
    /*! Constructor and destructor */
    mtsProxyBaseClient(const std::string & propertyFileName, const std::string & endpointInfo)
        : BaseType(propertyFileName, BaseType::PROXY_TYPE_CLIENT), EndpointInfo(endpointInfo)
    {}

    virtual ~mtsProxyBaseClient() {}
};

template<class _proxyOwner>
void mtsProxyBaseClient<_proxyOwner>::IceInitialize(void)
{
    try {
        BaseType::IceInitialize();

        // Create an Ice proxy using stringfied proxy information
        ProxyObject = this->IceCommunicator->stringToProxy(EndpointInfo);

        // If a proxy fails to be created, an exception is thrown.
        CreateProxy();

        InitSuccessFlag = true;

        ChangeProxyState(BaseType::PROXY_STATE_READY);

        IceLogger->trace("mtsProxyBaseClient", "Client proxy initialization success.");
    } catch (const Ice::ConnectionRefusedException & e) {
        if (IceLogger) {
            IceLogger->error("mtsProxyBaseClient: Connection refused. Check if server is running.");
            IceLogger->trace("mtsProxyBaseClient", e.what());
        } else {
            std::cout << "ERROR - mtsProxyBaseClient: Connection refused. Check if server is running." << std::endl;
            std::cout << "ERROR - mtsProxyBaseClient: " << e.what() << std::endl;
        }
    } catch (const Ice::Exception & e) {
        if (IceLogger) {
            IceLogger->error("mtsProxyBaseClient: Client proxy initialization error");
            IceLogger->trace("mtsProxyBaseClient", e.what());
        } else {
            std::cout << "ERROR - mtsProxyBaseClient: Client proxy initialization error." << std::endl;
            std::cout << "ERROR - mtsProxyBaseClient: " << e.what() << std::endl;
        }
    } catch (...) {
        if (IceLogger) {
            IceLogger->error("mtsProxyBaseClient: exception");
        } else {
            std::cout << "ERROR - mtsProxyBaseClient: exception" << std::endl;
        }
    }

    if (!InitSuccessFlag) {
        try {
            IceCommunicator->destroy();
        } catch (const Ice::Exception & e) {
            if (IceLogger) {
                IceLogger->error("mtsProxyBaseClient: Client proxy clean-up error");
                IceLogger->trace("mtsProxyBaseClient", e.what());
            } else {
                std::cerr << "ERROR - mtsProxyBaseClient: Client proxy clean-up error." << std::endl;
                std::cerr << "ERROR - mtsProxyBaseClient: " << e.what() << std::endl;
            }
        }
    }
}

template<class _proxyOwner>
void mtsProxyBaseClient<_proxyOwner>::IceCleanup(void)
{
    ChangeProxyState(BaseType::PROXY_STATE_FINISHING);

    InitSuccessFlag = false;

    RemoveProxy();
}

template<class _proxyOwner>
void mtsProxyBaseClient<_proxyOwner>::StopProxy(void)
{
    IceCleanup();

    if (IceCommunicator) {
        try {
            IceCommunicator->destroy();
            IceCommunicator = NULL;
            IceLogger->trace("mtsProxyBaseClient", "Proxy client clean-up success.");
        } catch (const Ice::Exception& e) {
            IceLogger->trace("mtsProxyBaseClient", "Proxy client clean-up failure.");
            IceLogger->trace("mtsProxyBaseClient", e.what());
        } catch (const char* msg) {
            IceLogger->error("mtsProxyBaseClient: Proxy client clean-up failure.");
            IceLogger->trace("mtsProxyBaseClient", msg);
        }
    }

    BaseType::IceCleanup();
}


#endif // _mtsProxyBaseClient_h
