//***************************************************************************//
// TCPClient Class Implementation
//
// Created: Feb 21, 2006
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
#include <assert.h> // For assertions
#include <iostream> // For access to cerr
#ifdef WIN32
#include <winsock2.h>
#endif
//***************************************************************************//

//***************************************************************************//
#include "DNS.hpp"
#include "TCPClient.hpp"
//***************************************************************************//

//***************************************************************************//
#define NON_BLOCKING_CONNECT_TIMEOUT 1
//***************************************************************************//

//***************************************************************************//
using namespace std;
//***************************************************************************//

//***************************************************************************//
namespace MST
{
    
  //*************************************************************************//
  // Start of TCPClient implementation
  //-------------------------------------------------------------------------//
  TCPClient::TCPClient()
  {
    //-----------------------------------------------------------------------//
    // Set connection to false since we obviously have not established a connection.
    mbConnected = false;
    //-----------------------------------------------------------------------//
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  TCPClient::~TCPClient()
  {
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  long TCPClient::Connect(const class EndPoint& rEndPoint, bool bBlock)
  {
    return this->Connect(rEndPoint.Port(), rEndPoint.IP(), bBlock);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  long TCPClient::Connect(unsigned short usPort, const char* cpHostName,  bool bBlock)
  {
    unsigned long ulIP;
    if((ulIP = DNS::Instance()->Resolve(cpHostName)) != INADDR_NONE)
    {
      return this->Connect(usPort, ulIP, bBlock);
    }
    return 0;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  long TCPClient::Connect(unsigned short usPort, const std::string& strHostName,  bool bBlock)
  {
    unsigned long ulIP;
    if((ulIP = DNS::Instance()->Resolve(strHostName)) != INADDR_NONE)
    {
      return this->Connect(usPort, ulIP, bBlock);
    }
    return 0;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  long TCPClient::Connect(unsigned short usPort, unsigned long ulIP, bool bBlock)
  {
    long lReturnValue = 0;

    //-----------------------------------------------------------------------//
    if(Socket::mSocket == 0)
      TCP::CreateDescriptor();
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    // Setup address to connect to.
    this->mpRemoteSocketAddress->sin_family = AF_INET;
    this->mpRemoteSocketAddress->sin_port = htons(usPort);
    this->mpRemoteSocketAddress->sin_addr.s_addr = htonl(ulIP);
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    fd_set fdread, fdwrite, fdexcept;
    timeval oSelectTimeout;
    if(!bBlock)
    {
      this->SetBlocking(false);
      FD_ZERO(&fdread);
      FD_ZERO(&fdwrite);
      FD_ZERO(&fdexcept);
      FD_SET(this->Descriptor(), &fdwrite);
      FD_SET(this->Descriptor(), &fdexcept);
      oSelectTimeout.tv_sec = NON_BLOCKING_CONNECT_TIMEOUT;
      oSelectTimeout.tv_usec = 0;
    }
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    // Attempt to connect the remote address.
    int iConnectResult = connect(Socket::mSocket, (sockaddr*)this->mpRemoteSocketAddress, sizeof(*(this->mpRemoteSocketAddress)));
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    if(!bBlock)
    {
      long lSelectResult = select(0, &fdread, &fdwrite, &fdexcept, &oSelectTimeout);

      // Set the socket to block again.
      this->SetBlocking(true);

      if(lSelectResult == 0 || FD_ISSET(this->Descriptor(), &fdexcept))
        iConnectResult = -1;
      else
        iConnectResult = 0;
    }
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    // If the connection was unsuccessful.
    if(iConnectResult == -1)
    {
      //---------------------------------------------------------------------//
      /// \todo Log an error message
      //---------------------------------------------------------------------//

      //---------------------------------------------------------------------//
      // Reset the remote address data type and return a zero to signify and error.
      memset(this->mpRemoteSocketAddress, 0L, sizeof(*(this->mpRemoteSocketAddress)));
      lReturnValue = 0;
      //---------------------------------------------------------------------//
    } // End of if(iConnectResult == -1)
    else
    {
      //---------------------------------------------------------------------//
      // Set the data members in the Socket base class since we just established a
      // connection. This will set the local and remote end point addresses
      // of this establish connection.
      Socket::SetAddresses();
      //---------------------------------------------------------------------//

      //---------------------------------------------------------------------//
      // Set the below bool to true to signal that this object has created.
      // a connection
      mbConnected = true;
      //---------------------------------------------------------------------//

      //---------------------------------------------------------------------//
      // Start socket threads
      if(Socket::StartRecvThread() && Socket::StartSendThread())
      {
        // Return 1 to signal that no error has occurred and that connection
        // was established.
        lReturnValue = 1;
      }
      //---------------------------------------------------------------------//


      //---------------------------------------------------------------------//
    } // End of else for if(iConnectResult == -1)
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    return lReturnValue;
    //-----------------------------------------------------------------------//
  } // End of long TCPClient::Connect(unsigned short usPort, unsigned long ulIP, bool bBlock)
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  // This method will disconnect and close the connection to and remote host
  // if such a connection exists.
  void TCPClient::Disconnect()
  {
    //-----------------------------------------------------------------------//
    // Set connection state appropriately.
    mbConnected = false;
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    // Close the socket.
    Socket::Close();
    //-----------------------------------------------------------------------//
  }
  //-------------------------------------------------------------------------//
  // End of TCPClient implementation
  //*************************************************************************//
} // End of namespace MST
//***************************************************************************//
