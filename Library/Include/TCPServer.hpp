//***************************************************************************//
// TCPServer Class Interface
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

#ifndef MST_TCP_SERVER_HPP
#define MST_TCP_SERVER_HPP

//***************************************************************************//
// System Includes
#include <string>
#include <vector>
//***************************************************************************//

//***************************************************************************//
// Local Includes
#include "TCP.hpp"
#include "Packet.hpp"
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //*************************************************************************//
  class TCPServerAcceptThread : public MST::Thread
  {
  private:
    //***********************************************************************//
    unsigned long mulAcceptSocket;
    std::vector<class Socket<class Packet>*> mvAcceptedSockets;
    class MST::Mutex* mpAcceptedSocketsMutex;
    class MST::Event* mpNewConnectionEvent;
    static void  __cdecl AcceptThread(void* vpParameters);
    //***********************************************************************//
  public:
    //***********************************************************************//
    TCPServerAcceptThread();
    ~TCPServerAcceptThread();

    //-----------------------------------------------------------------------//
    /// These methods will accept a connection to this object
    /// This method must only be called after a Listen method has been called 
    /// and returned from without triggering a error.
    /// \return This method will return a Socket object when a connection
    ///         to this server is established. 
    class Socket<class Packet>* const _cdecl Accept();
    /// \overload class Socket Accept(unsigned long ulMask);
    /// \param [in] ulMask
    /// The unsigned long is an IP address in host order that this 
    /// %TCPServer will bind to.
    class Socket<class Packet>* const _cdecl Accept(unsigned long ulMask);
    /// \overload class Socket Accept(char* cpMask);
    /// \param [in] cpMask
    /// The c string is an IP address in normal "123.123.123.123" form that this 
    /// %TCPServer will bind to
    class Socket<class Packet>* const _cdecl Accept(const char* const cpMask);
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    unsigned long _cdecl ConnectedSocketCount();
    class Socket<class Packet>* const _cdecl SocketByIndex(unsigned long ulIndex);
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    class MST::Event& _cdecl NewConnectionEvent();
    //-----------------------------------------------------------------------//
    //***********************************************************************//
  };
  //*************************************************************************//

  //*************************************************************************//
  class TCPServer : public TCP
  {
  private:
    //***********************************************************************//
    class MST::Multiplexer* mpMultiplexer;
    //***********************************************************************//
  protected:
    //***********************************************************************//
    bool mbAcceptThreadRunning;
     
    class TCPServerAcceptThread* mpAcceptThread;
    //***********************************************************************//
  public:
    //***********************************************************************//
    //-----------------------------------------------------------------------//
    // Constructor
    TCPServer();
    // Destructor
    virtual ~TCPServer();
    //-----------------------------------------------------------------------//
    /// These methods will accept a connection to this object
    /// This method must only be called after a Listen method has been called 
    /// and returned from without triggering a error.
    /// \return This method will return a Socket object when a connection
    ///         to this server is established. 
    class Socket<class Packet>* const _cdecl Accept();
    /// \overload class Socket Accept(unsigned long ulMask);
    /// \param [in] ulMask
    /// The unsigned long is an IP address in host order that this 
    /// %TCPServer will bind to.
    class Socket<class Packet>* const _cdecl Accept(unsigned long ulMask);
    /// \overload class Socket Accept(char* cpMask);
    /// \param [in] cpMask
    /// The c string is an IP address in normal "123.123.123.123" form that this 
    /// %TCPServer will bind to
    class Socket<class Packet>* const _cdecl Accept(const char* const cpMask);
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    /// This method will start a thread that will wait for incoming connections 
    /// to be accepted.
    /// \return The method will return true if the thread was successfully started.
    ///         Otherwise it will return false.
    bool _cdecl StartAcceptThread();
    /// This method will stop the thread that is waiting for incoming connections 
    /// to be accepted.
    /// \return The method will return true if the thread was successfully started.
    ///         Otherwise it will return false.
    bool _cdecl StopAcceptThread();
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    /// This method will create and prepare the class to accept connections on 
    /// the listening socket this method manages. The Accept method should be called
    /// after this method.
    /// \param [in] usPort
    /// The host order (little endian on x86) port that this class will 
    /// accept new connections on.
    /// \return This method returns true if the listening port is successfully created.
    ///         Otherwise, this method returns false.
    bool _cdecl Listen(unsigned short usPort);
    /// \overload bool Listen(unsigned short usPort, unsigned short usBackLog)
    /// \param [in] usPort
    /// The host order (little endian on x86) port that this class will 
    /// accept new connections on.
    /// \param [in] usBackLog 
    /// Number of connection that will be backlogged on the listening socket.
    /// \return This returns true if the listening port is successfully created.
    ///         Otherwise, this method returns false.
    bool _cdecl Listen(unsigned short usPort, unsigned short usBackLog);
    bool _cdecl Listen(const class EndPoint& rLocal);
    /// \overload bool Listen(unsigned short usPort, std::string& strHostName);
    /// \param [in] usPort 
    /// The host order (little endian on x86) port that this class will 
    /// accept new connections on.
    /// \param [in] strHostName 
    /// The host name of the client that this class will accept connections from.
    /// \return This returns true if the listening port is successfully created.
    ///          Otherwise, this method returns false.
    bool _cdecl Listen(unsigned short usPort, const std::string& strHostName);
    /// \overload bool Listen(unsigned short usPort, unsigned long ulIP, unsigned short usBackLog)
    /// \param [in] usPort 
    /// The host order (little endian on x86) port that this class will 
    /// accept new connections on.
    /// \param [in] ulIP
    /// The host order (little endian on x86) IP that this class will 
    /// accept connections from.
    /// \param [in] usBackLog
    /// Number of connection that will be backlogged on the listening socket.
    /// \return This returns true if the listening port is successfully created.
    ///          Otherwise, this method returns false.
    bool _cdecl Listen(unsigned short usPort, unsigned long ulIP, unsigned short usBackLog);
    /// \overload bool Listen(unsigned short usPort, std::string& strHostName, unsigned short usBackLog)
    /// \param [in] usPort 
    /// The host order (little endian on x86) port that this class will 
    /// accept new connections on.
    /// \param [in] strHostName 
    /// The host name of the client that this class will accept
    /// connections from.
    /// \param [in] usBackLog
    /// Number of connection that will be backlogged on the listening socket.
    /// \return This returns true if the listening port is successfully created.
    ///         Otherwise, this method returns false.
    bool _cdecl Listen(unsigned short usPort, const std::string& strHostName, unsigned short usBackLog);
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    unsigned long _cdecl ConnectionCount() const;
    class Socket<class Packet>* const _cdecl GetConnectionByIndex(unsigned long ulIndex);
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    bool _cdecl WaitForNewDataOrNewConnection(unsigned long ulTimeout = 0xFFFFFFFF);
    //-----------------------------------------------------------------------//
    //***********************************************************************//
  }; // End of class TCPClient : public Socket
  //*************************************************************************//
} // End of namespace MST 
//***************************************************************************//

#endif MST_TCP_SERVER_HPP
