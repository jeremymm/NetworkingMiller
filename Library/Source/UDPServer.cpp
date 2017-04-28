//***************************************************************************//
// UDPServer Class Implementation
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
#include <iostream> // For access to cerr
#ifdef WIN32
#include <Winsock2.h>
#endif
//***************************************************************************//

//***************************************************************************//
// Local Includes
#include "Packet.hpp"
#include "UDPServer.hpp"
//***************************************************************************//

//***************************************************************************//
using namespace std;
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //*************************************************************************//
  // Start of UDPServer implementation
  template class UDPServer<class Packet>;
  template class UDPServer<class ReliablePacket>;

  //-------------------------------------------------------------------------//
  template <class T>
  UDPServer<T>::UDPServer()
  {
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  UDPServer<T>::~UDPServer()
  {
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool UDPServer<T>::Listen(unsigned short usPort)
  {
    bool bReturnValue = false;
    
    class EndPoint oEndPoint;
    oEndPoint.IP(INADDR_ANY);
    oEndPoint.Port(usPort);
    
    if(oEndPoint.VaildSource())
      bReturnValue = this->Listen(oEndPoint);

    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool UDPServer<T>::Listen(const class EndPoint& rLocal)
  {
    bool bReturnValue = false;
    if(rLocal.VaildSource())
    {
      this->mpLocalSocketAddress->sin_family      = AF_INET;
      this->mpLocalSocketAddress->sin_addr.s_addr = htonl(INADDR_ANY);
      this->mpLocalSocketAddress->sin_port        = htons(rLocal.Port());

      //-----------------------------------------------------------------------//
      // Bind that socket the local address parameters we just set.
      long lReturnValue;
      if((lReturnValue = bind(this->mSocket, (sockaddr*)this->mpLocalSocketAddress, sizeof(*(this->mpLocalSocketAddress)))) != 0)
      {
        // An error has occurred...
        /// \todo Get errno.
        cerr << "Failed to Bind UDP Server to IP: ANY Port: " << rLocal.Port() << endl;
      } // End of if((lReturnValue = bind(this->mSocket, (sockaddr*)&this->mLocalSocketAddress, sizeof(this->mLocalSocketAddress))) < 0)
      /// \todo Investigate why I have a listen here ?!?!?!
      //else if((lReturnValue = listen(this->mSocket, BACKLOG_DEFAULT)) != 0)
      //{
      // An error has occurred...
      /// \todo Get errno.
      //  cerr << "Failed to listen in UDP Server to IP: ANY Port: " << usPort << endl;
      //}
      //-----------------------------------------------------------------------//

      //-----------------------------------------------------------------------//
      if(lReturnValue == 0)
      {
        Socket::SetAddresses();

        //---------------------------------------------------------------------//
        // Start socket threads
        if(Socket::StartRecvThread() && Socket::StartSendThread())
          bReturnValue = true;
        //---------------------------------------------------------------------//
      }
      //-----------------------------------------------------------------------//
    }
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//
  // End of UDPServer implementation
  //*************************************************************************//
} // End of namespace MST
//***************************************************************************//
