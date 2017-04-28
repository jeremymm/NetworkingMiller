//***************************************************************************//
// TCPServer Class Implementation
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
// System Includes
#include <assert.h> // For assertions
#include <iostream> // For access to cerr
#include <process.h> // For _beingthread
#ifdef WIN32
#include <WinSock2.h>
#endif
//***************************************************************************//

//***************************************************************************//
// ISI Includes
#include <MultithreadedMiller.hpp>
//***************************************************************************//

//***************************************************************************//
// Local Includes
#include "DNS.hpp"
#include "TCPServer.hpp"
//***************************************************************************//

//***************************************************************************//
using namespace std;
//***************************************************************************//

//***************************************************************************//
#ifdef WIN32
#define BACKLOG_DEFAULT 10
#else
#define BACKLOG_DEFAULT LISTENQ
#endif WIN32
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //-------------------------------------------------------------------------//
  TCPServerAcceptThread::TCPServerAcceptThread()
  {
    mpAcceptedSocketsMutex = new Mutex();
    mpNewConnectionEvent = new Event();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  TCPServerAcceptThread::~TCPServerAcceptThread()
  {
    try
    {
      if(this->mpAcceptedSocketsMutex)
        delete mpAcceptedSocketsMutex;
      if(this->mpNewConnectionEvent)
        delete mpNewConnectionEvent;
    }
    catch(...)
    {
    }
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void __cdecl TCPServerAcceptThread::AcceptThread(void* vpParameters)
  {
    //-----------------------------------------------------------------------//
    TCPServerAcceptThread* pThis = (TCPServerAcceptThread*)vpParameters;
    //-----------------------------------------------------------------------//

    pThis->SignalStarted();

      //-----------------------------------------------------------------------//
      while(pThis->Continue())
      {
        pThis->Accept();
      } // End of while(pThreadParameters->mbThreadRunning)
      //-----------------------------------------------------------------------//
      
    pThis->SignalStopped();
    return;
  } // End of void __cdecl TCPServer::AcceptThread(void* vpParameters)
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  class Socket<class Packet>* const TCPServerAcceptThread::Accept()
  {
    return this->Accept(0UL);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  class Socket<class Packet>* const TCPServerAcceptThread::Accept(unsigned long ulIP)
  {
    //-----------------------------------------------------------------------//
    // Create a socket object so that we can set it when a new connection 
    // occurs and return it contents to the callee.
    class Socket<class Packet>* pSocket = NULL;
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    // Call the accept system call to receive a new connection.
    int iRemoteAddressSize = sizeof(sockaddr_in);
    sockaddr_in oRemoteAddress;
    unsigned long ulDescriptor;
    if((ulDescriptor = static_cast<unsigned long>(accept(this->mulAcceptSocket, (sockaddr*)&oRemoteAddress, &iRemoteAddressSize))) == ULONG_MAX)
    {
      /// \todo Get errno.
      cerr << "TCP Server failed to accept connection\r\n";
    }
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    // Check connecting IP
    if((ulIP == 0) || (ulIP == ntohl(oRemoteAddress.sin_addr.s_addr)))
    {
      //---------------------------------------------------------------------//
      // Create a socket object so that we can set it when a new connection 
      // occurs and return it contents to the callee.
      /// \todo \bURGENT I need to do something with this socket. Right now its created but
      /// nothing is done with it.
      pSocket = new Socket<class Packet>(ulDescriptor, CONNECTION_ORIENTATED_SOCKET);
      pSocket->RemoteSocketAddress(oRemoteAddress);
      //---------------------------------------------------------------------//
      pSocket->SetAddresses();
      if(this->mpAcceptedSocketsMutex->Lock())
      {
        this->mvAcceptedSockets.push_back(pSocket);
        this->mpAcceptedSocketsMutex->Unlock();
      }

      this->mpNewConnectionEvent->Signal();
    }
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    // Return data about the new connection to the callee.
    return pSocket;
    //-----------------------------------------------------------------------//
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  class Socket<class Packet>* const TCPServerAcceptThread::Accept(const char* const cpIP)
  {
    return this->Accept(DNS::Instance()->ConvertNumericAddress(cpIP));
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  unsigned long TCPServerAcceptThread::ConnectedSocketCount()
  {
    this->mpAcceptedSocketsMutex->Lock();
    unsigned long ulReturnValue = static_cast<unsigned long>(this->mvAcceptedSockets.size());
    this->mpAcceptedSocketsMutex->Unlock();
    return ulReturnValue;

  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  class Socket<class Packet>* const TCPServerAcceptThread::SocketByIndex(unsigned long ulIndex)
  {
    class Socket<class Packet>* pReturnValue = NULL;
    this->mpAcceptedSocketsMutex->Lock();
    if(ulIndex < this->mvAcceptedSockets.size())
    {
      pReturnValue = this->mvAcceptedSockets[ulIndex];
    }
    this->mpAcceptedSocketsMutex->Unlock();
    return pReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  class MST::Event& TCPServerAcceptThread::NewConnectionEvent()
  {
    return *(this->mpNewConnectionEvent);
  }
  //-------------------------------------------------------------------------//
  // End of implementation for class TCPServerAcceptThread
  //*************************************************************************//

  //*************************************************************************//
  // Start of implementation for class TCPServer
  //-------------------------------------------------------------------------//
  TCPServer::TCPServer()
  {
    this->mpAcceptThread = new TCPServerAcceptThread();
    this->mpMultiplexer = new Multiplexer();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  TCPServer::~TCPServer()
  {
    try
    {
      if(this->mpAcceptThread)
      {
        if(this->mpAcceptThread->Running())
          this->mpAcceptThread->Stop();
        delete this->mpAcceptThread;
      }

      if(this->mpMultiplexer)
        delete mpMultiplexer;
    }
    catch(...)
    {
    }
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  class Socket<class Packet>* const TCPServer::Accept()
  {
    return this->mpAcceptThread->Accept();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  class Socket<class Packet>* const TCPServer::Accept(const char* const cpIP)
  {
    return this->mpAcceptThread->Accept(cpIP);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  class Socket<class Packet>* const TCPServer::Accept(unsigned long ulIP)
  {
    return this->mpAcceptThread->Accept(ulIP);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool TCPServer::StartAcceptThread()
  {
    return this->mpAcceptThread->Start();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool TCPServer::StopAcceptThread()
  {
    return this->mpAcceptThread->Stop();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool TCPServer::Listen(unsigned short usPort)
  {
    return Listen(usPort, INADDR_ANY, BACKLOG_DEFAULT);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool TCPServer::Listen(unsigned short usPort, unsigned short usBackLog)
  {
    return Listen(usPort, INADDR_ANY, usBackLog);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool TCPServer::Listen(const class EndPoint& rLocal)
  {
    bool bReturnValue = false;
    if(rLocal.VaildSource())
      bReturnValue = Listen(rLocal.Port(), rLocal.IP(), BACKLOG_DEFAULT);
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool TCPServer::Listen(unsigned short usPort, const std::string& strHostName)
  {
    return Listen(usPort, DNS::Instance()->Resolve(strHostName), BACKLOG_DEFAULT);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool TCPServer::Listen(unsigned short usPort, unsigned long ulIP, unsigned short usBackLog)
  {
    bool bReturnValue = false;
    //-----------------------------------------------------------------------//
    // Clear local address data before beginning to setup the listening socket.
    memset(this->mpLocalSocketAddress, 0L, sizeof(*(this->mpLocalSocketAddress)));
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    // Set the listening sockets parameters.
    // Internet connection.
    this->mpLocalSocketAddress->sin_family      = AF_INET;
    // Use first available IP address.
    this->mpLocalSocketAddress->sin_addr.s_addr = htonl(ulIP);
    // The connections will be on the port specified by the callee.
    this->mpLocalSocketAddress->sin_port        = htons(usPort);
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    // Bind that socket the local address parameters we just set.
    long lReturnValue;
    if((lReturnValue = bind(this->mSocket, (sockaddr*)this->mpLocalSocketAddress, sizeof(*(this->mpLocalSocketAddress)))) != 0)
    {
      // An error has occurred...
      /// \todo Get errno.
      cerr << "Failed to Bind TCP Server to IP: ANY Port: " << usPort << endl;
    } // End of if((lReturnValue = bind(this->mSocket, (sockaddr*)&this->mLocalSocketAddress, sizeof(this->mLocalSocketAddress))) < 0)
    //-----------------------------------------------------------------------//
    // If the bind system call succeeded then we must call the listen
    // system call.
    else if((lReturnValue = listen(this->mSocket, usBackLog)) != 0)
    {
      // An error has occurred...
      /// \todo Get errno.
      cerr << "TCP Server failed to Listen on IP: ANY Port: " << usPort << endl;
    } // End of else if((lReturnValue = listen(this->mSocket, BACKLOG_DEFAULT)) < 0)
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    // Throw an assertion if an error has occurred
    assert(lReturnValue == 0);
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    // If the return value is 0 then we had no trouble otherwise return false.
    if(lReturnValue == 0)
    {
      //---------------------------------------------------------------------//
      // Start socket threads
      if(this->mpMultiplexer->AddHandle(this->mpAcceptThread->NewConnectionEvent().Handle()) &&
        this->mpMultiplexer->AddHandle(&Socket::RecvThreadInsertionEvent()) &&
        Socket::StartRecvThread() && Socket::StartSendThread())
        bReturnValue = true;
      //---------------------------------------------------------------------// 
    }

    return bReturnValue;
    //-----------------------------------------------------------------------//
  } // End of bool TCPServer::Listen(unsigned short usPort, unsigned long ulIP, unsigned short usBackLog)
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool TCPServer::Listen(unsigned short usPort, const std::string& strHostName, unsigned short usBackLog)
  {
    return Listen(usPort, DNS::Instance()->Resolve(strHostName), usBackLog);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  unsigned long TCPServer::ConnectionCount() const
  {
    return this->mpAcceptThread->ConnectedSocketCount();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  class Socket<class Packet>* const TCPServer::GetConnectionByIndex(unsigned long ulIndex)
  {
    return this->mpAcceptThread->SocketByIndex(ulIndex);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool TCPServer::WaitForNewDataOrNewConnection(unsigned long ulTimeout)
  {
    return this->mpMultiplexer->WaitForTrigger(ulTimeout);
  }
  //-------------------------------------------------------------------------//
  // End of TCPServer Implementation
  //*************************************************************************//
} // End of namespace MST
//***************************************************************************//
