//***************************************************************************//
// HeartbeatResponseThread Class Implementation
//
// Created: May  25, 2009
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
#include "UDP.hpp"
#include "HeartbeatResponseThread.hpp"
#include "ReliablePacket.hpp"
#include "RTNPacket.hpp"
//***************************************************************************//

//***************************************************************************//
#define MAX_PACKET_BUFFER_SIZE 1024
//***************************************************************************//

//***************************************************************************//
using namespace std;
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  template class HeartbeatResponseThread<ReliablePacket>;
  template class HeartbeatResponseThread<RTNPacket>;

  //-------------------------------------------------------------------------//
  template <class T>
  HeartbeatResponseThread<T>::HeartbeatResponseThread(class UDP<T>* pUDP)
  {
    this->mpMutex = new Mutex();
    this->mpNewPacketEvent = new Event();
    this->mpPacketBufferMutex = new Mutex();

    this->mpUDP = pUDP;
    Thread::SetRoutine(ResponseThread, this);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  HeartbeatResponseThread<T>::~HeartbeatResponseThread()
  {
    delete this->mpMutex;
    delete this->mpNewPacketEvent;
    delete this->mpPacketBufferMutex;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  void HeartbeatResponseThread<T>::ResponseThread(void* vpParameters)
  {
    class HeartbeatResponseThread* pThis = static_cast<HeartbeatResponseThread*>(vpParameters);

    pThis->SignalStarted();

    T* pPacket = new T();

    while(pThis->Continue())
    {
      pThis->mpNewPacketEvent->WaitForSignal();
      pThis->mpPacketBufferMutex->Lock();
      while(pThis->mdPacketBuffer.size())
      {
        *pPacket = *pThis->mdPacketBuffer.front();
        delete pThis->mdPacketBuffer.front();
        pThis->mdPacketBuffer.pop_front();
        pThis->mpPacketBufferMutex->Unlock();

        // Echo heart beat
        EndPoint oTempEndPoint = pPacket->Source();
        pPacket->Source(pPacket->Destination());
        pPacket->Destination(oTempEndPoint);
        pPacket->GenerateHeartbeatResponse();
        pThis->mpUDP->Send(pPacket); 
      }
      pThis->mpNewPacketEvent->Reset();
      pThis->mpPacketBufferMutex->Unlock();
    }

    delete pPacket;

    pThis->SignalStopped();
    pThis->End();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool HeartbeatResponseThread<T>::SubmitRequest(const T& rPacket)
  {
    bool bReturnValue = false;
    this->mpPacketBufferMutex->Lock();
    if(this->mdPacketBuffer.size() < MAX_PACKET_BUFFER_SIZE)
    {
      this->mdPacketBuffer.push_back(new T(rPacket));
      
      bReturnValue = true;
    }
    if(bReturnValue)
      this->mpNewPacketEvent->Signal();

    this->mpPacketBufferMutex->Unlock();
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//
} // End of namespace MST
//***************************************************************************//