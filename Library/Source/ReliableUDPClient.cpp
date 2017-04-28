//***************************************************************************//
// Reliable UDP Client Class Implementation
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

#include <MultithreadedMiller.hpp>

//***************************************************************************//
// Local Includes
#include "DNS.hpp"
#include "ReliablePacket.hpp"
#include "ReliableUDPClient.hpp"
#include "HeartbeatRequestThread.hpp"
#include "ReliablePacket.hpp"
#include "RTNPacket.hpp"
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  template class ReliableUDPClient<ReliablePacket>;
  template class ReliableUDPClient<RTNPacket>;

  //-------------------------------------------------------------------------//
  template <class T>
  ReliableUDPClient<T>::ReliableUDPClient()
  {
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  ReliableUDPClient<T>::~ReliableUDPClient()
  {
  }
  //-------------------------------------------------------------------------//

  //-----------------------------------------------------------------------//
  template <class T>
  long ReliableUDPClient<T>::Connect(const class EndPoint& rEndPoint)
  {
    return this->Connect(rEndPoint.Port(), rEndPoint.IP());
  }
  //-----------------------------------------------------------------------//

  //-----------------------------------------------------------------------//
  template <class T>
  long ReliableUDPClient<T>::Connect(unsigned short usPort, const char* const cpHostName)
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
  template <class T>
  long ReliableUDPClient<T>::Connect(unsigned short usPort, const std::string& strHostName)
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
  template <class T>
  long ReliableUDPClient<T>::Connect(unsigned short usPort, unsigned long ulIP)
  {
    long lReturnValue = 0;

    //-----------------------------------------------------------------------//
    unsigned long ulSocket = 0;
    if(Socket::mSocket == 0)
      ulSocket = UDP::CreateDescriptor();
    else
      ulSocket = static_cast<unsigned long>(Socket::mSocket);
    //-----------------------------------------------------------------------//

    if((ulSocket > 0) && (ulSocket < ULONG_MAX))
    {
      //---------------------------------------------------------------------//
      // Setup address to connect to.
      this->mpRemoteSocketAddress->sin_family = AF_INET;
      this->mpRemoteSocketAddress->sin_port = htons(usPort);
      this->mpRemoteSocketAddress->sin_addr.s_addr = htonl(ulIP);
      //---------------------------------------------------------------------//

      // Note that connect returns 0 on success and -1 on error.  The UDPClient::Connect 
      // method will return 1 on success and 0 on error
      int iConnectResult = connect(Socket::mSocket, (sockaddr*)this->mpRemoteSocketAddress, sizeof(*(this->mpRemoteSocketAddress)));
      if(iConnectResult == 0)
      {
        Socket::SetAddresses();
        //-------------------------------------------------------------------//
        // Start socket threads
        if(Socket::StartRecvThread() && Socket::StartSendThread())
          lReturnValue = 1;
        //-------------------------------------------------------------------//
      }
    }
 
    if(lReturnValue)
    {
      if(ReliableUDP::mpReliableProtocol->StartThreads())
      {
        class EndPoint oTempEndPoint;
        oTempEndPoint.IP(ulIP);
        oTempEndPoint.Port(usPort);
        ReliableUDP::mpReliableProtocol->Monitor(oTempEndPoint);
        lReturnValue = 1;
      }
    }
    return lReturnValue;
  }
  //-----------------------------------------------------------------------//
} // End of namespace MST
//***************************************************************************//