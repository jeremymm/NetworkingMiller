//***************************************************************************//
// Reliable UDP Server Class Implementation
//
// Created: May 16, 2009
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
#ifdef WIN32
	#include <WinSock2.h>
#endif
//***************************************************************************//

//***************************************************************************//
// MST Technologies Includes
#include <MultithreadedMiller.hpp>
//***************************************************************************//

//***************************************************************************//
// Local Includes
#include "ReliableProtocol.hpp"
#include "ReliableUDP.hpp"
#include "RTNPacket.hpp"
//***************************************************************************//

//***************************************************************************//
#define MAX_NUMBER_OF_OUTSTANDING_PACKETS 256
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  template class ReliableUDP<ReliablePacket>;
  template class ReliableUDP<RTNPacket>;

  //-------------------------------------------------------------------------//
  template <class T>
  ReliableUDP<T>::ReliableUDP()
  {
    this->mpReliableProtocol = new ReliableProtocol<T>(this);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  ReliableUDP<T>::~ReliableUDP()
  {
    delete this->mpReliableProtocol;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  long ReliableUDP<T>::Recv(T* const pPacket)
  {
    long lReturnValue = -1;

    lReturnValue = UDP::Recv(pPacket);
    mpReliableProtocol->SubmitPacket(*pPacket);
    if(pPacket->IsHeartbeatRequest() || pPacket->IsHeartbeatResponse() || pPacket->IsNAK())
    {
      pPacket->Clear();
      lReturnValue = 0UL;
    }
    
    while(mpReliableProtocol->HasPacketsToRetransmit())
    {
      UDP::Send(&(mpReliableProtocol->RetransmitQueueFront()));
      mpReliableProtocol->PopRetransmitQueue();
    }

    return lReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  long ReliableUDP<T>::Send(T* const pPacket)
  {
    long lReturnValue = -1;
    lReturnValue = UDP::Send(pPacket);
    return lReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool ReliableUDP<T>::WaitOnIncoming(unsigned long ulTimeout)
  {
    bool bReturnValue = false;
    class Timer* pTimer = new Timer();
    unsigned long ulTimeLeft = ulTimeout;
    pTimer->DT(TIMER_1);
    T* pPacket = new T();
    bool bRealPacketFound = false;
    while(!bRealPacketFound && (bReturnValue = Socket::WaitOnIncoming(ulTimeLeft)))
    {
      while(Socket::FrontIncomingPacket(pPacket))
      {
        if(pPacket->IsHeartbeatRequest() || pPacket->IsHeartbeatResponse() || pPacket->IsNAK())
        {
          ReliableUDP::Recv(pPacket);
          if(ulTimeLeft != INFINITE)
          {
            unsigned long ulWaitTime = static_cast<unsigned long>((pTimer->DT(TIMER_1) / 1000.0));
            // detect overflow
            if((ulTimeLeft - ulWaitTime) >= ulTimeLeft)
              ulTimeLeft = 0UL;
            else
              ulTimeLeft -= ulWaitTime;
          }
        }
        else
        {
          bRealPacketFound = true;
          break;
        }
      }
      pPacket->Clear();
      Socket::ResetWaitOnIncoming();
    }
    Socket::ResetWaitOnIncoming();

    if(bRealPacketFound)
      bReturnValue = true;
    delete pPacket;
    delete pTimer;
    
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//
} // End of namespace MST
//***************************************************************************//