//***************************************************************************//
// UDP Class Implementation
//
// Created: Feb 20, 2006
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
#include <winsock2.h>
#endif
//***************************************************************************//

//***************************************************************************//
// Local Includes
#include "UDP.hpp"
#include "Packet.hpp"
#include "ReliablePacket.hpp"
#include "RTNPacket.hpp"
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //*************************************************************************//
  // Start of UDP implementation
  template class UDP<Packet>;
  template class UDP<ReliablePacket>;
  template class UDP<RTNPacket>;

  //-------------------------------------------------------------------------//
  template <class T>
  UDP<T>::UDP() : Socket(CONNECTIONLESS_ORIENTATED_SOCKET)
  {
    unsigned long ulResult = UDP::CreateDescriptor();
    assert(ulResult);
    mbBroadcastEnabled = false;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  UDP<T>::~UDP()
  {
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool UDP<T>::Broadcast(bool bEnable)
  {
    int iBroadcast = static_cast<int>(bEnable);
    bool bReturnValue = true;
    if(setsockopt(Socket::mSocket, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<char*>(&iBroadcast), sizeof(iBroadcast)) == -1)
      bReturnValue = false;
    else if(bEnable)
      mbBroadcastEnabled = true;
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  unsigned long UDP<T>::CreateDescriptor()
  {
    //-----------------------------------------------------------------------//
    // Create a UDP socket.
    Socket::mSocket = static_cast<unsigned long>(socket(AF_INET, SOCK_DGRAM, IPPROTO_IP));
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    if(this->mSocket != INVALID_SOCKET)
    {
      int iOptionValue = 1;
      int iSetSockOptResult = setsockopt(Socket::mSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&iOptionValue, sizeof(iOptionValue));
      if(iSetSockOptResult == SOCKET_ERROR)
      {
        int iCloseSocketResult = closesocket(Socket::mSocket);
        assert(iCloseSocketResult != SOCKET_ERROR);
        this->mSocket = 0UL;
      }
    }
    else
    {
      this->mSocket = 0UL;
    }
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    // Throw an assertion if we could not create the socket.
    assert(this->mSocket != 0UL);
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    return static_cast<unsigned long>(this->mSocket);
    //-----------------------------------------------------------------------//
  } // End of unsigned long UDP::CreateDescriptor()
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool UDP<T>::BroadcastEnabled() const
  {
    return this->mbBroadcastEnabled;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  long UDP<T>::Recv(T* pPacket)
  {
    long lReturnValue = -1L;
    pPacket->Type((PacketType)(pPacket->Type() | GENERIC_UDP_PACKET_TYPE));
    lReturnValue = Socket::Recv(pPacket);
    return lReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  long UDP<T>::Send(T* pPacket)
  {
    pPacket->Type((PacketType)(pPacket->Type() | GENERIC_UDP_PACKET_TYPE));
    pPacket->Source(Socket::mLocalEndPoint);
    return Socket::Send(*pPacket);
  }
  //-------------------------------------------------------------------------//
  // End of UDP implementation
  //*************************************************************************//
} // End of namespace MST
//***************************************************************************//
