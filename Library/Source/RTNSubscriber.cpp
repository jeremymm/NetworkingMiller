//***************************************************************************//
// RTN (Real Time Network) Subscriber Class Implementation
//
// Created: Aug 3, 2008
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
#ifdef WIN32
#include <WinSock.h>
#endif
//***************************************************************************//

//***************************************************************************//
// Local Includes
#include "DNS.hpp"
#include "RTNSubscriber.hpp"
#include "RTNPacket.hpp"
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //-------------------------------------------------------------------------//
  RTNSubscriber::RTNSubscriber()
  {
    this->mulID = 0UL;
    this->mfX = 0.0F;
    this->mfY = 0.0F;
    this->mfZ = 0.0F;

    this->mbIsActor = false;
    this->mbIsViewer = false;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  RTNSubscriber::~RTNSubscriber()
  {
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  float RTNSubscriber::X() const
  {
    return this->mfX;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  float RTNSubscriber::Y() const
  {
    return this->mfY;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  float RTNSubscriber::Z() const
  {
    return this->mfZ;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void RTNSubscriber::X(float fX)
  {
    this->mfX = fX;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void RTNSubscriber::Y(float fY)
  {
    this->mfY = fY;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void RTNSubscriber::Z(float fZ)
  {
    this->mfZ = fZ;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void RTNSubscriber::SetPosition(float fX, float fY, float fZ)
  {
    this->mfX = fX;
    this->mfY = fY;
    this->mfZ = fZ;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  long RTNSubscriber::Recv(class RTNPacket* const pPacket)
  {
    long lReturnValue = 0;
    lReturnValue = ReliableUDP::Recv(pPacket);
    return lReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  long RTNSubscriber::Send(class RTNPacket* const pPacket)
  {
    long lReturnValue = 0;
    if(pPacket->IsHeartbeatRequest() || pPacket->IsHeartbeatResponse() || pPacket->IsNAK())
    {
      lReturnValue = ReliableUDP::Send(pPacket);
    }
    else
    {
      pPacket->SetPosition(this->mfX, this->mfY, this->mfZ);
      pPacket->ID(this->mulID);

      if(this->IsActor())
        pPacket->MarkFromActor();
      if(this->IsViewer())
        pPacket->MarkFromViewer();

      lReturnValue = ReliableUDP::Send(pPacket);
    }
    return lReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  long RTNSubscriber::Update()
  {
    unsigned long ulReturnValue = 0UL;
    class RTNPacket* pTemp = new RTNPacket();
    pTemp->SetPosition(this->mfX, this->mfY, this->mfZ);
    pTemp->ID(this->mulID);

    if(this->IsActor())
      pTemp->MarkFromActor();
    if(this->IsViewer())
      pTemp->MarkFromViewer();

    pTemp->SetPayload("1", 1);

    ulReturnValue = ReliableUDP::Send(pTemp);
    delete pTemp;
    return ulReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void RTNSubscriber::SetAsActor(bool bEnable)
  {
    this->mbIsActor = bEnable;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void RTNSubscriber::SetAsViewer(bool bEnable)
  {
    this->mbIsViewer = bEnable;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool RTNSubscriber::IsActor() const
  {
    return this->mbIsActor;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool RTNSubscriber::IsViewer() const
  {
    return this->mbIsViewer;
  }
  //-------------------------------------------------------------------------//

  //-----------------------------------------------------------------------//
  long RTNSubscriber::Connect(const class EndPoint& rEndPoint)
  {
    return this->Connect(rEndPoint.Port(), rEndPoint.IP());
  }
  //-----------------------------------------------------------------------//

  //-----------------------------------------------------------------------//
  long RTNSubscriber::Connect(unsigned short usPort, const char* const cpHostName)
  {
    unsigned long ulIP;
    if((ulIP = DNS::Instance()->Resolve(cpHostName)) != INADDR_NONE)
    {
      return this->Connect(usPort, ulIP);
    }
    return 0;
  }
  //-----------------------------------------------------------------------//

  //-----------------------------------------------------------------------//
  long RTNSubscriber::Connect(unsigned short usPort, const std::string& strHostName)
  { 
    unsigned long ulIP;
    if((ulIP = DNS::Instance()->Resolve(strHostName)) != INADDR_NONE)
    {
      return this->Connect(usPort, ulIP);
    }
    return 0;
  }
  //-----------------------------------------------------------------------//

  //-----------------------------------------------------------------------//
  long RTNSubscriber::Connect(unsigned short usPort, unsigned long ulIP)
  {
    long lReturnValue = ReliableUDPClient<RTNPacket>::Connect(usPort, ulIP);
    if(lReturnValue > 0)
    {
      // Possibly send connect message
    }
    return lReturnValue;
  }
  //-----------------------------------------------------------------------//
} // End of namespace MST
//***************************************************************************//
