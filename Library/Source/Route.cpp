//***************************************************************************//
// Route Class Implementation
//
// Created: Mar 9, 2009
// By: Jeremy Michael Miller
//
// Copyright (c) 2006-2016 Jeremy Michael Miller. 
// Author: = "Jeremy Michael Miller"
// Copyright: = "Copyright 2005-2016, Networking Miller,  All rights reserved."
// Credits = ["Jeremy Michael Miller"]
// License: "Fair use v0.9"
// Version: "0.0.1"
// Maintainer: "Jeremy Michael Miller"
// Email: "maybe_later@mst.dnsalias.net"
// Status: "Alpha"
//***************************************************************************//

//***************************************************************************//
// System Includes
#include <assert.h>
//***************************************************************************//

//***************************************************************************//
// Local Includes
#include "EndPoint.hpp"
#include "Packet.hpp"
#include "Route.hpp"
#include "TCPClient.hpp"
#include "TCPServer.hpp"
#include "UDPClient.hpp"
#include "UDPServer.hpp"
//***************************************************************************//

//***************************************************************************//
using namespace std;
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //*************************************************************************//
  // Start of RouteStrategy implementation
  //-------------------------------------------------------------------------//
  RouteStrategy::RouteStrategy()
  {
    this->mpDestination = new EndPoint();
    this->mpSource = new EndPoint();
  } // End of RouteStrategy::RouteStrategy()
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  RouteStrategy::~RouteStrategy()
  {
    try
    {
      if(this->mpDestination)
        delete this->mpDestination;
      if(this->mpSource)
        delete this->mpSource;
    }
    catch(...)
    {
    }
  } // End of RouteStrategy::~RouteStrategy()
  //-------------------------------------------------------------------------//
  // End of RouteStrategy implementation
  //*************************************************************************//

  //*************************************************************************//
  // Start of RouteTCPStrategy implementation
  //-------------------------------------------------------------------------//
  RouteTCPStrategy::RouteTCPStrategy()
  {
    this->mpServer = new TCPServer();
    this->mpDestination = new EndPoint();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  RouteTCPStrategy::~RouteTCPStrategy()
  {
    try
    {
      if(this->mpServer)
      {
        if(this->mpServer->IsOpen())
          this->mpServer->Close();
        delete this->mpServer;
      }

      if(this->mpDestination)
      {
        delete this->mpDestination;
      }
    }
    catch(...)
    {
      
    }
    
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool RouteTCPStrategy::Open(const class EndPoint& rSource, const class EndPoint& rDestination)
  {
      bool bReturnValue = false;
      if(this->mpServer)
      {
        if(this->mpServer->Listen(rSource))
        {
          bReturnValue = this->mpServer->StartAcceptThread();
          if(bReturnValue)
          {
            *(this->mpDestination) = rDestination;
          }
          else
          {
            this->Close();
          }
        }
      }
      return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool RouteTCPStrategy::Close()
  {
      bool bReturnValue = false;
      if(this->mpServer)
      {
        if(this->mpServer->IsOpen())
          this->mpServer->Close();
        delete this->mpServer;
        this->mpServer = NULL;
        bReturnValue = true;
      }
      return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void RouteTCPStrategy::ThreadedRoutine(void* vpParameters)
  {
    class RouteTCPStrategy* pThis = (RouteTCPStrategy*)vpParameters;
    pThis->SignalStarted();
    class Packet* pPacket =  new Packet();

    while(pThis->Continue())
    {
      if(pThis->mpServer->WaitForNewDataOrNewConnection())
      {
        while(pThis->mpServer->Recv(pPacket))
        {
          map<class EndPoint, class TCPClient*>::iterator itrClientMap;
          itrClientMap = pThis->mmClients.find(pPacket->Source());
          if(itrClientMap != pThis->mmClients.end())
          {
            if(itrClientMap->second->Connected())
              itrClientMap->second->Send(*pPacket);
          }
          else
          {
            class TCPClient* pTempClient = new TCPClient();
            pTempClient->Connect(*(pThis->mpDestination));

            if(pTempClient->Connected())
            {
              pTempClient->Send(*pPacket);
              pThis->mmClients.insert(make_pair(pPacket->Source(), pTempClient));
            }
          }
        }
      }
    }

    delete pPacket;
    pThis->SignalStopped();
    return;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool RouteTCPStrategy::Start()
  {
    return true;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool RouteTCPStrategy::Stop()
  {
    return true;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  const class EndPoint& RouteTCPStrategy::Source() const
  {
    return *(this->mpSource);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  const class EndPoint& RouteTCPStrategy::Destination() const
  {
    return *(this->mpDestination);
  }
  //-------------------------------------------------------------------------//
  // End of RouteTCPStrategy implementation
  //*************************************************************************//

  //*************************************************************************//
  // Start of RouteUDPStrategy implementation
  //-------------------------------------------------------------------------//
  RouteUDPStrategy::RouteUDPStrategy()
  {
    this->mpServer = new UDPServer<class Packet>();
    this->mpDestination = new EndPoint();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  RouteUDPStrategy::~RouteUDPStrategy()
  {
    try
    {
      if(this->mpServer)
        delete this->mpServer;
      if(this->mpDestination)
        delete this->mpDestination;
    }
    catch(...)
    {
    }
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool RouteUDPStrategy::Open(const class EndPoint& rSource, const class EndPoint& rDestination)
  {
      bool bReturnValue = false;
      
      if(this->mpServer)
      {
        if(this->mpServer->Listen(rSource))
        {
          bReturnValue = true;
          *(this->mpDestination) = rDestination;
        }
        else
        {
          this->Close();
        }
      }
      return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool RouteUDPStrategy::Close()
  {
      bool bReturnValue = false;
      if(this->mpServer)
      {
        if(this->mpServer->IsOpen())
          this->mpServer->Close();
        delete this->mpServer;
        this->mpServer = NULL;
        bReturnValue = true;
      }
      return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void RouteUDPStrategy::ThreadedRoutine(void* vpParameters)
  {
    class RouteUDPStrategy* pThis = (RouteUDPStrategy*)vpParameters;
    pThis->SignalStarted();

    class Packet* pPacket = new Packet();
    while(pThis->Continue())
    {
      pThis->mpServer->WaitOnIncoming();
      while(pThis->mpServer->Recv(pPacket))
      {
        map<class EndPoint, class UDPClient<class Packet>*>::iterator itrClientMap;
        itrClientMap = pThis->mmClients.find(pPacket->Source());
        if(itrClientMap != pThis->mmClients.end())
        {
          itrClientMap->second->Send(pPacket);
        }
        else
        {
          class UDPClient<class Packet>* pTempClient = new UDPClient<class Packet>();
          pTempClient->Connect(*(pThis->mpDestination));

          pTempClient->Send(pPacket);
          pThis->mmClients.insert(make_pair(pPacket->Source(), pTempClient));
        }
      }
    }

    delete pPacket;
    pThis->SignalStopped();
    return;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool RouteUDPStrategy::Start()
  {
    return true;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool RouteUDPStrategy::Stop()
  {
    return true;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  const class EndPoint& RouteUDPStrategy::Source() const
  {
    return *(this->mpSource);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  const class EndPoint& RouteUDPStrategy::Destination() const
  {
    return *(this->mpDestination);
  }
  //-------------------------------------------------------------------------//
  // End of RouteUDPThread implementation
  //*************************************************************************//

  //*************************************************************************//
  // Start of Route implementation
  //-------------------------------------------------------------------------//
  Route::Route(bool bIsTCP)
  {
    if(bIsTCP)
      this->mpStrategy = new RouteTCPStrategy();
    else
      this->mpStrategy = new RouteUDPStrategy();
    assert(this->mpStrategy);
  } // End of Route::Route(bool bIsTCP)
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  Route::~Route()
  {
    try
    {
      if(this->mpStrategy)
        delete this->mpStrategy;
    }
    catch(...)
    {
    }
  } // End of Route::~Route()
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool Route::Open(const class EndPoint& rSource, const class EndPoint& rDestination)
  {
    return this->mpStrategy->Open(rSource, rDestination);
  } // End of bool Route::Open(const class EndPoint& rSource, const class EndPoint& rDestination)
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool Route::Close()
  {
    return this->mpStrategy->Close();
  } // End of bool Route::Close()
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool Route::Start()
  {
    return this->mpStrategy->Start();
  } // End of bool Route::Start()
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool Route::Stop()
  {
    return this->mpStrategy->Stop();
  } // End of bool Route::Stop()
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  const class EndPoint& Route::Source() const
  {
    return this->mpStrategy->Source();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  const class EndPoint& Route::Destination() const
  {
    return this->mpStrategy->Destination();
  }
  //-------------------------------------------------------------------------//
  // End of Route implementation
  //*************************************************************************//
} // End of namespace MST
//***************************************************************************//
