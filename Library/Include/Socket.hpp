//***************************************************************************//
// Socket Class Interface
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

#ifndef MST_SOCKET_HPP
#define MST_SOCKET_HPP

//***************************************************************************//
// System Includes
#include <string>
//***************************************************************************//

//***************************************************************************//
// Local Includes
#include "EndPoint.hpp"
#include "Scheduler.hpp"
//***************************************************************************//

#define MST_SOCKET_VERSION 0.005F

//***************************************************************************//
namespace MST
{
  //*************************************************************************//
  enum ConnectionType
  {
    CONNECTION_ORIENTATED_SOCKET,
    CONNECTIONLESS_ORIENTATED_SOCKET,
    SOCKET_ORIENTATION_UNKNOWN
  };
  //*************************************************************************//

  //*************************************************************************//
  template <class T> class SocketRecvThread;
  template <class T> class SocketSendThread;
  //*************************************************************************//

  //*************************************************************************//
  template <class T>
  class Socket
  {
  private:
    //***********************************************************************//
    //-----------------------------------------------------------------------//
    #ifdef WIN32
    static bool bWinSockStarted;
    static struct WSAData mwsaData;
    #endif WIN32
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    unsigned short musPauseTime;
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    // Denotes the type of socket. IE UDP or TCP.
    enum ConnectionType meConnectionType;
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    class SocketRecvThread<T>* mpRecvThread;
    class SocketSendThread<T>* mpSendThread;
    //-----------------------------------------------------------------------//
  
    //-----------------------------------------------------------------------//
    void Initialize(unsigned long ulDescriptor, enum ConnectionType eConnectionType);
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    bool StopRecvThread();
    bool StopSendThread();
    //-----------------------------------------------------------------------//
    //***********************************************************************//
  protected:
    //***********************************************************************//
    //-----------------------------------------------------------------------//
    long mSocket;
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    struct sockaddr_in* mpLocalSocketAddress;
    struct sockaddr_in* mpRemoteSocketAddress;
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    class EndPoint mLocalEndPoint;
    class EndPoint mRemoteEndPoint;
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    bool ResetWaitOnIncoming();
    bool ResetWaitOnOutgoing();
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    bool StartRecvThread();
    bool StartSendThread();
    //-----------------------------------------------------------------------//

    class MST::Event& RecvThreadInsertionEvent();

    //-----------------------------------------------------------------------//
    virtual void _cdecl PauseIncoming();
    virtual void _cdecl ResumeIncoming();
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    virtual void _cdecl PauseOutgoing();
    virtual void _cdecl ResumeOutgoing();
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    virtual unsigned long CreateDescriptor();
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    /// The below method will set the receive timeout for all read/receive methods
    /// To disable timeout set parameter 1 and 2 to zero
    /// \param [in] ulSeconds
    /// The time in seconds the data must be received by
    /// \param [in] ulMillSeconds
    /// The time in milliseconds the data must be received by
    /// \return
    /// This method will return true when the timeout is successfully set, 
    /// otherwise it will return false.
    bool _cdecl SetRecvTimeout(unsigned long ulSeconds, unsigned long ulMillSeconds); 
    /// The below method will set the send timeout for all write/send methods
    /// To disable timeout set parameter 1 and 2 to zero
    /// \param [in] ulSeconds
    /// The time in seconds the data must be received by
    /// \param [in] ulMillSeconds
    /// The time in milliseconds the data must be received by
    /// \return
    /// This method will return true when the timeout is successfully set,
    /// otherwise it will return false
    bool _cdecl SetSendTimeout(unsigned long ulSeconds, unsigned long ulMillSeconds);
    //-----------------------------------------------------------------------//
    //***********************************************************************//
  public:
    //***********************************************************************//
    //-----------------------------------------------------------------------//
    Socket();
    Socket(enum ConnectionType eConnectionType);
    Socket(unsigned long ulDescriptor, enum ConnectionType eConnectionType);
    Socket(const Socket& rRHS);
    virtual ~Socket();
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    /// Simple assignment overload so that data members are correctly copied. 
    class Socket<T>& operator = (const Socket<T>& oRHS);
    bool operator < (const Socket<T>& rRHS) const;
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    bool _cdecl FrontIncomingPacket(T* const pPacket) const;
    bool _cdecl FrontOutgoingPacket(T* const pPacket) const;
    bool _cdecl PopOutgoingPacket(T* const pPacket);
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    /// This method will shutdown both sending and receiving on the socket 
    /// and then close the socket.
    void _cdecl Close();
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    bool _cdecl IsOpen() const;
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    void _cdecl SetBlocking(bool bBlock);
    void _cdecl SetRecvBufferSize(unsigned long ulLength);
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    /// This method will return if the internal socket descriptor has been created
    /// \return Returns true if the internal socket descriptor has been 
    ///          created otherwise it returns false 
    bool _cdecl Valid() const { return (mSocket ? true : false); }
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    void _cdecl SetSendingRate(unsigned long ulKiloBitsPerSecond);
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    unsigned long _cdecl Descriptor() { return static_cast<unsigned long>(this->mSocket); } const
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    unsigned short _cdecl LocalPort()  const { return this->mLocalEndPoint.Port();  }
    unsigned long  _cdecl LocalIP()    const { return this->mLocalEndPoint.IP();    }
    unsigned short _cdecl RemotePort() const { return this->mRemoteEndPoint.Port(); }
    unsigned long  _cdecl RemoteIP()   const { return this->mRemoteEndPoint.IP();   }
    void _cdecl RemoteSocketAddress(const sockaddr_in& rRemoteAddress);
    const sockaddr_in& _cdecl RemoteSocketAddress() const;
    void _cdecl LocalSocketAddress(const sockaddr_in& rRemoteAddress);
    const sockaddr_in& _cdecl LocalSocketAddress() const;
    bool _cdecl SetAddresses();
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    bool StartThreads();
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    /// This method will fill in a message with the next message in 
    /// in the incoming message buffer and then removed that message from
    /// that message buffer.
    /// \param [out] pPacket
    /// This parameter needs to be a pointer to instantiated message. The
    /// message will be overwritten if the message the callee will receive.
    /// \return
    /// This method returns the amount of received on success. Otherwise, the 
    /// method returns -1.
    virtual long _cdecl Recv(T* const pPacket);
    /// This method add a message to the outgoing message buffer.
    /// \param [in] rPacket
    /// The message to be sent.
    /// \return
    /// This method returns the amount of sent on success. Otherwise, the 
    /// method returns -1.
    virtual bool _cdecl Send(T& rPacket);
    //-----------------------------------------------------------------------//


    //-----------------------------------------------------------------------//
    /// This method will block until there is something to read from 
    /// incoming data stream or time has expired.  Once some data is available, 
    /// the method will unblock and the calling thread will continue.
    /// \param [in] ulTimeout
    /// This parameters is the maximum time this method will block in
    /// milliseconds
    /// \return 
    /// This method will return true if there is incoming data otherwise it will
    /// return false.  By returning false the method is indicating that no
    /// data has been received before the timeout trigger has been detected.
    virtual bool _cdecl WaitOnIncoming(unsigned long ulTimeout = ULONG_MAX) const;
    /// This method will block until something is sent from the
    /// outgoing data stream or time has expired.  Once some data is sent, 
    /// the method will unblock and the calling thread will continue.
    /// \param [in] ulTimeout
    /// This parameters is the maximum time this method will block in
    /// milliseconds
    /// \return 
    /// This method will return true if there was outgoing data otherwise it will
    /// return false.  By returning false the method is indicating that no
    /// data has been sent before the timeout trigger was been detected.
    virtual bool _cdecl WaitOnOutgoing(unsigned long ulTimeout = ULONG_MAX) const;
    /// This method will return true is there is something to read from the
    /// incoming data stream.
    /// \return 
    /// Will return true if there is data to read in the incoming stream
    /// otherwise it will return false.
    virtual bool _cdecl CheckOutgoing() const;
    //-----------------------------------------------------------------------//
    //***********************************************************************//
  }; // End of class Socket
  //*************************************************************************//
} // End of namespace MST
//***************************************************************************//
#endif MST_SOCKET_HPP
