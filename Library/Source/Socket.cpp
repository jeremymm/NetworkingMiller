//***************************************************************************//
// Socket Class Implementation
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
#include <stdlib.h>
#include <float.h> 
#include <math.h>
#include <process.h>
#include <iostream>
#include <vector>
#include <string.h>
#ifdef WIN32
#include <Winsock2.h>
#endif WIN32
//***************************************************************************//

//***************************************************************************//
// MST Technologies Includes
#include <MultithreadedMiller.hpp>
//***************************************************************************//

//***************************************************************************//
// Local Includes
#include "Socket.hpp"
#include "Scheduler.hpp"
#include "Throttler.hpp"
#include "Packet.hpp"
#include "ReliablePacket.hpp"
#include "RTNPacket.hpp"
#include "PacketBuffer.hpp"
//***************************************************************************//

//***************************************************************************//
using namespace std;
//***************************************************************************//

//***************************************************************************//
// The size field in IP4 is 16 bits and the size includes the size of the IP4 
// header which is 20 bytes so the total payload for a IP4 packet is 65516 bytes
#define MAX_IP4_PACKET_SIZE 65516 //  2^16 - 20
//***************************************************************************//

//***************************************************************************//
#define DEFAULT_NUMBER_OF_PACKETS 256
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //*************************************************************************//
  template <class T>
  class SocketRecvThread : public MST::Thread
  {
  private:
    //***********************************************************************//
    long mSocket;
    class MST::Mutex* mpMutex;

    class PacketBuffer<T>* mpRecvBuffer;
    enum ConnectionType meConnectionType;

    static void  __cdecl RecvThread(void* vpParameters);
    long RecvWrapper(T* const pPacket);
    //***********************************************************************//
  public:
    //***********************************************************************//
    SocketRecvThread(unsigned long ulSocket, enum ConnectionType eType);
    ~SocketRecvThread();

    bool _cdecl PopPacket(T* const pPacket);
    bool _cdecl FrontPacket(T* const pPacket) const;
    bool _cdecl HasUnreadPackets() const;
    class MST::Event& _cdecl InsertionEvent() const;
    bool _cdecl Halt();
    bool _cdecl Resume();
    bool _cdecl SetBufferSize(unsigned long ulLength);
    //***********************************************************************//
  }; // End of class SocketRecvThread : public MST::Thread 
  //*************************************************************************//

  //*************************************************************************//
  template <class T>
  class SocketSendThread : public MST::Thread
  {
  private:
    //***********************************************************************//
    long mSocket;

    class MST::Mutex* mpMutex;

    class PacketBuffer<T>* mpSentBuffer;

    class EndPoint mDestinationEndPoint;
    class Scheduler<T>* mpScheduler;
    static void  __cdecl SendThread(void* vpParameters);
    unsigned long SendWrapper(const T& rPacket);
    //***********************************************************************//
  public:
    //***********************************************************************//
    SocketSendThread(long lSocket, const class EndPoint& rDestination);
    ~SocketSendThread();
    void _cdecl SetSendRate(unsigned long ulKiloBitsPerSecond);

    bool _cdecl SchedulePacket(const T& rPacket);

    bool _cdecl PopPacket(T* const pPacket);
    bool _cdecl FrontPacket(T* const pPacket) const;
    bool _cdecl HasUnreadPackets() const;
    class MST::Event& _cdecl InsertionEvent() const;
    bool _cdecl Halt();
    bool _cdecl Resume();
    //***********************************************************************//
  }; // End of class SocketSendThread : public MST::Thread 
  //*************************************************************************//


  template class SocketRecvThread<Packet>;
  template class SocketRecvThread<ReliablePacket>;
  template class SocketRecvThread<RTNPacket>;

  //-------------------------------------------------------------------------//
  template <class T>
  SocketRecvThread<T>::SocketRecvThread(unsigned long ulSocket, enum ConnectionType eType)
  {
    mSocket = ulSocket;
    meConnectionType = eType;
    this->mpMutex = new Mutex();

    mpRecvBuffer = new PacketBuffer<T>(DEFAULT_NUMBER_OF_PACKETS);
    Thread::SetRoutine(RecvThread, this);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  SocketRecvThread<T>::~SocketRecvThread()
  {
    try
    {
      if(this->mpRecvBuffer)
        delete mpRecvBuffer;

      if(this->mpMutex)
        delete this->mpMutex;
    }
    catch (...)
    {
    }
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  void __cdecl SocketRecvThread<T>::RecvThread(void* vpParameters)
  {
    //-----------------------------------------------------------------------//
    class SocketRecvThread* pThis = static_cast<SocketRecvThread*>(vpParameters);
    //-----------------------------------------------------------------------/

    pThis->SignalStarted();

    struct timeval oPoll;
    oPoll.tv_sec = 0;
    oPoll.tv_usec = 1000;

    unsigned long ulFlag = 1;
    ioctlsocket(pThis->mSocket, FIONBIO, &ulFlag);

    T* pPacket = new T;

    //-----------------------------------------------------------------------//
    while(pThis->Continue())
    {
      fd_set oReadDescriptorSet;
      fd_set oErrorDescriptorSet;
      FD_ZERO(&oReadDescriptorSet);
      FD_SET(pThis->mSocket, &oReadDescriptorSet);
      FD_ZERO(&oErrorDescriptorSet);
      FD_SET(pThis->mSocket, &oErrorDescriptorSet);

      select(0, &oReadDescriptorSet, 0, &oErrorDescriptorSet, &oPoll);
      if(FD_ISSET(pThis->mSocket, &oReadDescriptorSet))
      {
        pPacket->Clear();

        pThis->mpMutex->Lock();
        while(pThis->RecvWrapper(pPacket) > 0)
        {
          bool bPushResult = pThis->mpRecvBuffer->Push(*pPacket, true);
          assert(bPushResult);
        }
        pThis->mpMutex->Unlock();

        FD_ZERO(&oReadDescriptorSet);
        FD_SET(pThis->mSocket, &oReadDescriptorSet);
      }

      if(FD_ISSET(pThis->mSocket, &oErrorDescriptorSet))
      {
        /// \todo add error handling here.
        FD_ZERO(&oErrorDescriptorSet);
        FD_SET(pThis->mSocket, &oErrorDescriptorSet);
      }

    } // End of while(pParameters->bThreadRunning)

    delete pPacket;
    //-----------------------------------------------------------------------//

    pThis->SignalStopped();

    pThis->End();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool SocketRecvThread<T>::PopPacket(T* const pPacket)
  {
    bool bReturnValue = false;
    if(pPacket && this->Running())
    {
      if(this->mpMutex->Lock())
      {
        if(this->mpRecvBuffer->Length())
        {
          *pPacket = this->mpRecvBuffer->Front();
          this->mpRecvBuffer->Pop();
          bReturnValue = true;
        }
        this->mpMutex->Unlock();
      }
    }
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool SocketRecvThread<T>::FrontPacket(T* const pPacket) const
  {
    bool bReturnValue = false;
    if(pPacket && this->Running())
    {
      if(this->mpMutex->Lock())
      {
        if(this->mpRecvBuffer->Length())
        {
          *pPacket = this->mpRecvBuffer->Front();
          bReturnValue = true;
        }
        this->mpMutex->Unlock();
      }
    }
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool SocketRecvThread<T>::HasUnreadPackets() const
  {
    if(this->mpRecvBuffer->Length())
      return true;
    else
      return false;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool SocketRecvThread<T>::SetBufferSize(unsigned long ulLength)
  {
    this->mpRecvBuffer->Length(ulLength);
    return true;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  MST::Event& SocketRecvThread<T>::InsertionEvent() const 
  { 
    return mpRecvBuffer->InsertionEvent();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool SocketRecvThread<T>::Halt()
  {
    bool bReturnValue = false;
    this->mpMutex->Lock();
    bReturnValue = Thread::Pause();
    this->mpMutex->Unlock();
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool SocketRecvThread<T>::Resume()
  {
    bool bReturnValue = false;
    this->mpMutex->Lock();
    bReturnValue = Thread::Resume();
    this->mpMutex->Unlock();
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//
  //*************************************************************************//

  //*************************************************************************//
  template class SocketSendThread<Packet>;
  template class SocketSendThread<ReliablePacket>;
  template class SocketSendThread<RTNPacket>;

  //-------------------------------------------------------------------------//
  template <class T>
  SocketSendThread<T>::SocketSendThread(long oSocket, const class EndPoint& rDestination)
  {
    mSocket = oSocket;
    mpScheduler = new Scheduler<T>();
    mpSentBuffer = new PacketBuffer<T>(DEFAULT_NUMBER_OF_PACKETS);
    this->mDestinationEndPoint = rDestination;
    this->mpMutex = new Mutex();
    Thread::SetRoutine(SendThread, this);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  SocketSendThread<T>::~SocketSendThread()
  {
    try
    {
      if(this->mpScheduler)
        delete this->mpScheduler;

      if(this->mpSentBuffer)
        delete this->mpSentBuffer;

      if(this->mpMutex)
        delete this->mpMutex;
    }
    catch (...)
    {
    }
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool SocketSendThread<T>::PopPacket(T* const pPacket)
  {
    bool bReturnValue = false;
    if(pPacket && this->Running())
    {
      if(this->mpMutex->Lock())
      {
        if(this->mpSentBuffer->Length())
        {
          *pPacket = this->mpSentBuffer->Front();
          this->mpSentBuffer->Pop();
          bReturnValue = true;
        }
        this->mpMutex->Unlock();
      }
    }
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool SocketSendThread<T>::FrontPacket(T* const pPacket) const
  {
    bool bReturnValue = false;
    if(pPacket && this->Running())
    {
      if(this->mpMutex->Lock())
      {
        if(this->mpSentBuffer->Length())
        {
          *pPacket = this->mpSentBuffer->Front();
          bReturnValue = true;
        }
        this->mpMutex->Unlock();
      }
    }
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool SocketSendThread<T>::HasUnreadPackets() const
  {
    if(this->mpSentBuffer->Length())
      return true;
    else
      return false;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  MST::Event& SocketSendThread<T>::InsertionEvent() const
  {
    return this->mpSentBuffer->InsertionEvent();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool SocketSendThread<T>::Halt()
  {
    bool bReturnValue = false;
    this->mpMutex->Lock();
    bReturnValue = Thread::Pause();
    this->mpMutex->Unlock();
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool SocketSendThread<T>::Resume()
  {
    bool bReturnValue = false;
    this->mpMutex->Lock();
    bReturnValue = Thread::Resume();
    this->mpMutex->Unlock();
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  void SocketSendThread<T>::SetSendRate(unsigned long ulKiloBitsPerSecond)
  {
    this->mpScheduler->SetTargetRate(ulKiloBitsPerSecond * 1000);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  void SocketSendThread<T>::SendThread(void* vpParameters)
  {
    class SocketSendThread* pThis = static_cast<SocketSendThread*>(vpParameters);
    pThis->SignalStarted();

    while(pThis->Continue())
    {
      unsigned long ulSent = 0UL;
      float fWait = 0.0F;
      //---------------------------------------------------------------------//
      if(pThis->mpScheduler->WaitForReadyPacket(500))
      {
        pThis->mpScheduler->Update();
        while(pThis->mpScheduler->HasReadyPacket())
        {
          const T* const pPacket = pThis->mpScheduler->PopReadyPacket();

          pThis->mpMutex->Lock();
          if(pPacket)
          {
            ulSent = pThis->SendWrapper(*pPacket);
            if(ulSent != 0 && ulSent != ULONG_MAX)
              pThis->mpSentBuffer->Push(*pPacket, true);
          }
          pThis->mpMutex->Unlock();


          pThis->mpScheduler->Update();
        }
      }
      //---------------------------------------------------------------------//
    }
    pThis->SignalStopped();
    pThis->End();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool SocketSendThread<T>::SchedulePacket(const T& rPacket)
  {
    assert(rPacket.PayloadSize());
    return this->mpScheduler->SchedulePacket(rPacket);
  }
  //-------------------------------------------------------------------------//
  //*************************************************************************//

  //*************************************************************************//
  template class Socket<Packet>;
  template class Socket<ReliablePacket>;
  template class Socket<RTNPacket>;

#ifdef WIN32
  template <class T>
  bool Socket<T>::bWinSockStarted = false;
  template <class T>
  WSAData Socket<T>::mwsaData;
#endif WIN32

  //-------------------------------------------------------------------------//
  template <class T>
  Socket<T>::Socket()
  {
    Initialize(0UL, SOCKET_ORIENTATION_UNKNOWN);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  Socket<T>::Socket(enum ConnectionType eConnectionType)
  {
    Initialize(0UL, eConnectionType);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  Socket<T>::Socket(unsigned long ulDescriptor, enum ConnectionType eConnectionType)
  {
    Initialize(ulDescriptor, eConnectionType);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  Socket<T>::Socket(const Socket& rRHS)
  {
    *this = rRHS;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  // Destructor
  template <class T>
  Socket<T>::~Socket()
  {
    try
    {
      if(this->mpRecvThread)
      {
        if(this->mpRecvThread->Running())
          this->StopRecvThread();
        delete this->mpRecvThread;
      }
      if(this->mpSendThread)
      {
        if(this->mpSendThread->Running())
          this->StopSendThread();
        delete this->mpSendThread;
      }

      if(this->mpLocalSocketAddress)
        delete this->mpLocalSocketAddress;

      if(this->mpRemoteSocketAddress)
        delete this->mpRemoteSocketAddress;

    }
    catch(...)
    {
    }
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  void Socket<T>::Initialize(unsigned long ulDescriptor, enum ConnectionType eConnectionType)
  {
    //-----------------------------------------------------------------------//
    // If we are running on a windows operating system; we want to load up 
    // the Winsock2 library once and only once.
#ifdef WIN32
    long lResult;
    if(!bWinSockStarted)
    {
      lResult = WSAStartup(MAKEWORD(2,2), &mwsaData);
      assert(lResult == NO_ERROR);

      if(lResult == NO_ERROR)
        bWinSockStarted = true;
    }
#endif WIN32
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    // Clear socket
    mSocket     = ulDescriptor;

    musPauseTime  = 0;

    meConnectionType = eConnectionType;

    this->mpLocalSocketAddress = new sockaddr_in();
    this->mpRemoteSocketAddress = new sockaddr_in();

    // Clear socket setup information holders
    memset(mpLocalSocketAddress,  0L, sizeof(*(mpLocalSocketAddress)));
    memset(mpRemoteSocketAddress, 0L, sizeof(*(mpRemoteSocketAddress)));

    mpRecvThread = NULL;
    mpSendThread = NULL;
    //-----------------------------------------------------------------------//
  };
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  unsigned long Socket<T>::CreateDescriptor()
  {
    return 0L;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  // This method will shutdown both sending and receiving on the socket and then 
  // close the socket.
  template <class T>
  void Socket<T>::Close()
  {
    if(this->mpRecvThread && this->mpRecvThread->Running())
      this->StopRecvThread();
    if(this->mpSendThread && this->mpSendThread->Running())
      this->StopSendThread();

    if(this->mSocket)
    {
      int iShutdownResult = shutdown(this->mSocket, SD_BOTH);
      assert(iShutdownResult != SOCKET_ERROR);

      // NOTE: I think closesocket is Winsock2 specific
      int iCloseSocketResult = closesocket(this->mSocket);
      assert(iCloseSocketResult != SOCKET_ERROR);
    }

    // Set socket to zero so we know that it is not longer valid
    this->mSocket = 0L;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool Socket<T>::IsOpen() const
  {
    bool bReturnValue = false;
    if(this->mpRecvThread && this->mpSendThread)
    {
      if(this->mpRecvThread->Running() && 
        this->mpSendThread->Running() &&
        this->mSocket)
        bReturnValue = true;
    }
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  void Socket<T>::RemoteSocketAddress(const sockaddr_in& rRemoteAddress)
  {
    *(this->mpRemoteSocketAddress) = rRemoteAddress;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  const sockaddr_in& Socket<T>::RemoteSocketAddress() const
  {
    return *(this->mpRemoteSocketAddress);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  void Socket<T>::LocalSocketAddress(const sockaddr_in& rLocalAddress)
  {
    *(this->mpLocalSocketAddress) = rLocalAddress;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  const sockaddr_in& Socket<T>::LocalSocketAddress() const
  {
    return *(this->mpLocalSocketAddress);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool Socket<T>::SetRecvTimeout(unsigned long ulSeconds, unsigned long ulMillSeconds ) 
  {
    struct timeval oTime;
    oTime.tv_sec = static_cast<long>(ulSeconds);
    oTime.tv_usec = static_cast<long>(ulMillSeconds);
    int iResult = setsockopt(this->mSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&oTime, sizeof(oTime));
    /// \todo check errno
    if(!iResult)
      return true;
    return false;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool Socket<T>::SetSendTimeout(unsigned long ulSeconds, unsigned long ulMillSeconds ) 
  {
    struct timeval oTime;
    oTime.tv_sec = static_cast<long>(ulSeconds);
    oTime.tv_usec = static_cast<long>(ulMillSeconds);
    int iResult = setsockopt(this->mSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&oTime, sizeof(oTime));
    /// \todo check errno
    if(!iResult)
      return true;
    return false;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  long SocketRecvThread<T>::RecvWrapper(T* const pPacket)
  {
    long lReturnValue = 0;

    struct sockaddr_in oSourceAddr; // Do use if we need to report back this information
    int iSourceAddrLength = (int)sizeof(struct sockaddr_in);
    memset(oSourceAddr.sin_zero, 0L, sizeof(oSourceAddr.sin_zero));

    if(mSocket)
    {
      //---------------------------------------------------------------------//
      int iBytesRead                 = 0L;
      long lTotalBytesRead           = 0L;
      long lBytesLeft                = 0L;
      //---------------------------------------------------------------------//

      //---------------------------------------------------------------------//
      bool bUDPPacket = false;
      if(((this->meConnectionType & GENERIC_UDP_PACKET_TYPE) || (this->meConnectionType & RELIABLE_PACKET_TYPE)))
      {
        bUDPPacket = true;
      }
      //---------------------------------------------------------------------//

      //---------------------------------------------------------------------//
      char* cpPayload = new char[pPacket->PayloadMaxSize()];
      lBytesLeft = pPacket->PayloadMaxSize();
      do
      {
        if(bUDPPacket)
          iBytesRead = recvfrom(mSocket, cpPayload + lTotalBytesRead, lBytesLeft, 0, (struct sockaddr *)&oSourceAddr, &iSourceAddrLength);
        else
          iBytesRead = recv(mSocket, cpPayload + lTotalBytesRead, lBytesLeft, 0L);

        /// \todo Check for errors from recv() call
        DWORD error = GetLastError();

        if((error == ERROR_SUCCESS) && (iBytesRead > 0))
        {
          //-----------------------------------------------------------------//
          lBytesLeft -= iBytesRead;
          //-----------------------------------------------------------------//

          //-----------------------------------------------------------------//
          lTotalBytesRead += iBytesRead;
          //-----------------------------------------------------------------//

          if((lBytesLeft + lTotalBytesRead) > static_cast<long>(pPacket->PayloadSize()))
            lBytesLeft = pPacket->PayloadSize() - lTotalBytesRead;

        } // End of if(error == ERROR_SUCCESS)
        else
          break;

      }while(iBytesRead && (iBytesRead != -1) && (lTotalBytesRead <= static_cast<long>(pPacket->PayloadMaxSize())));

      if(lTotalBytesRead > 0)
      {
        if(bUDPPacket)
          pPacket->SetPayload(cpPayload, lTotalBytesRead, GENERIC_UDP_PACKET_TYPE);
        else
          pPacket->SetPayload(cpPayload, lTotalBytesRead, GENERIC_TCP_PACKET_TYPE);

        pPacket->Source(ntohl(oSourceAddr.sin_addr.s_addr), ntohs(oSourceAddr.sin_port));
        pPacket->Deserialize();
      }

      //---------------------------------------------------------------------//
      lReturnValue = lTotalBytesRead;
      //---------------------------------------------------------------------//

      delete [] cpPayload;
      //---------------------------------------------------------------------//

    } // End of if(mSocket)
    //-----------------------------------------------------------------------//

    return lReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  unsigned long SocketSendThread<T>::SendWrapper(const T& rPacket)
  {
    unsigned long ulTotalNumberOfBytesSent = 0;

    assert(rPacket.PayloadSize());

    bool bUDPPacket = false;
    bool bDestinationSet = false;
    struct sockaddr_in oDestinationAddr; // Do use if we need to report back this information
    int iDestinationAddrLength = (int)sizeof(struct sockaddr_in);

    if(((rPacket.Type() & GENERIC_UDP_PACKET_TYPE) || (rPacket.Type() & RELIABLE_PACKET_TYPE)))
    {
      oDestinationAddr.sin_family = AF_INET;  /// \todo investigate how this would work for other protocols. For example doing a sendto on a STREAM. If that is possible.
      if(!mDestinationEndPoint.IP() && !mDestinationEndPoint.Port() && rPacket.Destination().IP() && rPacket.Destination().Port())
      {
        oDestinationAddr.sin_addr.s_addr = htonl(rPacket.Destination().IP());
        oDestinationAddr.sin_port        = htons(rPacket.Destination().Port());
        bDestinationSet = true;
      }
      else if(mDestinationEndPoint.IP() && mDestinationEndPoint.Port() && !rPacket.Destination().IP() && !rPacket.Destination().Port())
      {
        oDestinationAddr.sin_addr.s_addr = htonl(mDestinationEndPoint.IP());
        oDestinationAddr.sin_port        = htons(mDestinationEndPoint.Port());
        bDestinationSet = true;
        //pPacket->Destination() = this->mRemoteEndPoint;
      }
      else if(mDestinationEndPoint.IP() && mDestinationEndPoint.Port() &&
        (mDestinationEndPoint.IP() == rPacket.Destination().IP()) && 
        (mDestinationEndPoint.Port() == rPacket.Destination().Port()))
      {
        oDestinationAddr.sin_addr.s_addr = htonl(mDestinationEndPoint.IP());
        oDestinationAddr.sin_port        = htons(mDestinationEndPoint.Port());
        bDestinationSet = true;
      }
      else
        assert(!"Problem with destination address");


      bUDPPacket = true;
    }

    int iNumberofBytesSent = 0;
    do
    {   
      if(bUDPPacket && bDestinationSet)
      {
        iNumberofBytesSent = sendto(mSocket, rPacket.Payload() + ulTotalNumberOfBytesSent, rPacket.PayloadSize() - ulTotalNumberOfBytesSent, 0L, (struct sockaddr *)&oDestinationAddr, iDestinationAddrLength);
      }
      else
      {
        iNumberofBytesSent = send(mSocket, rPacket.Payload() + ulTotalNumberOfBytesSent, rPacket.PayloadSize() - ulTotalNumberOfBytesSent, 0L);
      }

      /// \todo Check for errors from recv() call
      //DWORD error = GetLastError();

      //---------------------------------------------------------------------//
      if(iNumberofBytesSent > 0)
        ulTotalNumberOfBytesSent += static_cast<unsigned long>(iNumberofBytesSent);
      //---------------------------------------------------------------------//

    }while((ulTotalNumberOfBytesSent < rPacket.PayloadSize()) && (iNumberofBytesSent > 0));
    //-----------------------------------------------------------------------//

    return ulTotalNumberOfBytesSent;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool Socket<T>::SetAddresses()
  {
    bool bReturnValue = false;
    int iSize = sizeof(*(mpLocalSocketAddress));
    if(!getsockname(this->mSocket, (sockaddr*)this->mpLocalSocketAddress, &iSize))
    {
      mLocalEndPoint.IP(ntohl(this->mpLocalSocketAddress->sin_addr.s_addr));
      mLocalEndPoint.Port(ntohs(this->mpLocalSocketAddress->sin_port));
      mRemoteEndPoint.IP(ntohl(this->mpRemoteSocketAddress->sin_addr.s_addr));
      mRemoteEndPoint.Port(ntohs(this->mpRemoteSocketAddress->sin_port));
      bReturnValue = true;
    }
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  void Socket<T>::SetSendingRate(unsigned long ulKiloBitsPerSecond)
  {
    this->mpSendThread->SetSendRate(ulKiloBitsPerSecond);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool Socket<T>::StartRecvThread()
  {
    bool bReturnValue = false;
    if(this->mSocket && (this->mpRecvThread == NULL))
    {
      this->mpRecvThread = new SocketRecvThread<T>(static_cast<unsigned long>(this->mSocket), this->meConnectionType);
      assert(this->mpRecvThread);
      if(this->mpRecvThread)
      {
        bReturnValue = this->mpRecvThread->Start();
      }
    }
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool Socket<T>::StartSendThread()
  {
    bool bReturnValue = false;
    if(mSocket && (this->mpSendThread == NULL))
    {
      this->mpSendThread = new SocketSendThread<T>(this->mSocket, this->mRemoteEndPoint);
      assert(this->mpSendThread);
      if(this->mpSendThread)
      {
        bReturnValue = this->mpSendThread->Start();
      }
    }
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool Socket<T>::StartThreads()
  {
    bool bReturnValue = false;
    if(Socket::StartRecvThread() && Socket::StartSendThread())
      bReturnValue = true;
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool Socket<T>::StopRecvThread()
  {
    bool bReturnValue = false;
    if(this->mpRecvThread && this->mpRecvThread->Running())
    {
      bReturnValue = this->mpRecvThread->Stop();
      if(bReturnValue)
      {
        delete this->mpRecvThread;
        this->mpRecvThread = NULL;
      }

    }
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool Socket<T>::StopSendThread()
  {
    bool bReturnValue = false;
    if(this->mpSendThread && this->mpSendThread->Running())
    {
      bReturnValue = this->mpSendThread->Stop();
      if(bReturnValue)
      {
        delete this->mpSendThread;
        this->mpSendThread = NULL;
      }
    }
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  template <class T>
  MST::Event& Socket<T>::RecvThreadInsertionEvent()
  {
    return this->mpRecvThread->InsertionEvent();
  }

  //-------------------------------------------------------------------------//
  template <class T>
  bool Socket<T>::FrontIncomingPacket(T* const pPacket) const
  {
    return this->mpRecvThread->FrontPacket(pPacket);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool Socket<T>::FrontOutgoingPacket(T* const pPacket) const
  {
    return this->mpSendThread->FrontPacket(pPacket);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool Socket<T>::PopOutgoingPacket(T* const pPacket)
  {
    return this->mpSendThread->PopPacket(pPacket);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  void Socket<T>::SetBlocking(bool bBlock)
  {
    unsigned long ulBlock;
    if(!bBlock)
      ulBlock = 1;
    else
      ulBlock = 0;

#ifdef WIN32
    ioctlsocket(this->mSocket, FIONBIO, &ulBlock);
#endif WIN32
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  void Socket<T>::SetRecvBufferSize(unsigned long ulLength)
  {
    if(this->mpRecvThread)
      this->mpRecvThread->SetBufferSize(ulLength);
  }
  //-------------------------------------------------------------------------//


  //-------------------------------------------------------------------------//
  template <class T>
  long Socket<T>::Recv(T* const pPacket)
  {
    assert(pPacket);
    assert(this->mpRecvThread);
    long lReturnValue = -1;
    if(pPacket && this->mpRecvThread)
    {
      assert(this->mpRecvThread->Running());
      if(this->mpRecvThread->Running())
      {
        if(this->mpRecvThread->PopPacket(pPacket))
        {
          unsigned long ulPayloadSize = pPacket->PayloadSize();
          assert(ulPayloadSize < LONG_MAX);
          if(ulPayloadSize < LONG_MAX)
          {
            lReturnValue = static_cast<long>(ulPayloadSize);
          }
          else
          {
            lReturnValue = LONG_MAX;
          }
        }
      }
    }
    return lReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool Socket<T>::Send(T& rPacket)
  {
    bool bReturnValue = false;
    if(this->mpSendThread)
    {
      assert(this->mpSendThread->Running());
      if(this->mpSendThread->Running())
      {    
        if(!rPacket.Destination().VaildDestination())
          rPacket.Destination(Socket::mRemoteEndPoint);
        bReturnValue = this->mpSendThread->SchedulePacket(rPacket);
      }
    }
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool Socket<T>::WaitOnIncoming(unsigned long ulTimeout) const
  {
    bool bReturnValue = false;
    if(this->mpRecvThread)
    {
      if(this->mpRecvThread->InsertionEvent().WaitForSignal(ulTimeout) &&
        this->mpRecvThread->InsertionEvent().Reset())
        bReturnValue = true;
    }
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool Socket<T>::WaitOnOutgoing(unsigned long ulTimeout) const
  {
    bool bReturnValue = false;
    if(this->mpSendThread)
    {
      if(this->mpSendThread->InsertionEvent().WaitForSignal(ulTimeout) &&
        this->mpSendThread->InsertionEvent().Reset())
        bReturnValue = true;
    }
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool Socket<T>::ResetWaitOnIncoming()
  {
    bool bReturnValue = false;
    if(this->mpRecvThread)
    {
      bReturnValue = this->mpRecvThread->InsertionEvent().Reset();
    }
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool Socket<T>::ResetWaitOnOutgoing()
  {
    return this->mpSendThread->InsertionEvent().Reset();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  void Socket<T>::PauseIncoming()
  {
    this->mpRecvThread->Pause(ULONG_MAX);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  void Socket<T>::ResumeIncoming()
  {
    this->mpRecvThread->Resume();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  void Socket<T>::PauseOutgoing()
  {
    this->mpSendThread->Pause(ULONG_MAX);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  void Socket<T>::ResumeOutgoing()
  {
    this->mpSendThread->Resume();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool Socket<T>::CheckOutgoing() const
  {
    bool bReturnValue = false;
    if(this->mpSendThread)
      bReturnValue = this->mpSendThread->HasUnreadPackets();
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  // Simple = overload so that data members are correctly copied 
  template <class T>
  Socket<T>& Socket<T>::operator = (const Socket<T>& oRHS)
  {
    if(&oRHS == this)
      return *this;
    else
    {
      this->mSocket                  = oRHS.mSocket;
      *(this->mpRemoteSocketAddress) = *(oRHS.mpRemoteSocketAddress);
      *(this->mpLocalSocketAddress)  = *(oRHS.mpLocalSocketAddress);
      return *this;
    }
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool Socket<T>::operator < (const Socket<T>& rRHS) const
  {
    bool bReturnValue = false;
    if(this->mRemoteEndPoint < rRHS.mRemoteEndPoint)
      bReturnValue = true;
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//
} // End of namespace MST
//***************************************************************************//
