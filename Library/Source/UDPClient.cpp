//***************************************************************************//
// UDPClient Class Implementation
//
// Created: Mar 24, 2006
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
#include <assert.h> // For assertions
#ifdef WIN32
#include <WinSock2.h>
#endif
//***************************************************************************//

//***************************************************************************//
// Local Includes
#include "DNS.hpp"
#include "Packet.hpp"
#include "ReliablePacket.hpp"
#include "UDPClient.hpp"
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //*************************************************************************
  // Start of UDPClient implementation
  template class UDPClient<class Packet>;
  template class UDPClient<class ReliablePacket>;

  //-------------------------------------------------------------------------//
  template <class T>
  UDPClient<T>::UDPClient()
  {
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  UDPClient<T>::~UDPClient()
  {
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  long UDPClient<T>::Connect(const class EndPoint& rEndPoint)
  {
    return this->Connect(rEndPoint.Port(), rEndPoint.IP());
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  long UDPClient<T>::Connect(unsigned short usPort, const char* cpHostName)
  {
    unsigned long ulIP;
    if((ulIP = DNS::Instance()->Resolve(cpHostName)) != 0UL)
    {
      return this->Connect(usPort, ulIP);
    }
    return 0;
  }
  //-----------------------------------------------------------------------//

  //-----------------------------------------------------------------------//
  template <class T>
  long UDPClient<T>::Connect(unsigned short usPort, const std::string& strHostName)
  {
    unsigned long ulIP;
    if((ulIP = DNS::Instance()->Resolve(strHostName)) != 0UL)
    {
      return this->Connect(usPort, ulIP);
    }
    return 0;
  }
  //-----------------------------------------------------------------------//

  //-----------------------------------------------------------------------//
  template <class T>
  long UDPClient<T>::Connect(unsigned short usPort, unsigned long ulIP)
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
      if(UDP::BroadcastEnabled())
        this->mpRemoteSocketAddress->sin_addr.s_addr = htonl(INADDR_BROADCAST);
      else
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
    return lReturnValue;
  }
  //-----------------------------------------------------------------------//
  // End of UDPClient implementation
  //*************************************************************************
} // End of namespace MST
//***************************************************************************//
