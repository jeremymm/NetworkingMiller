//***************************************************************************//
// HeartbeatRequestThread Class Implementation
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
// System Includes
#include <assert.h>
//***************************************************************************//

//***************************************************************************//
// MST Technologies Includes
#include <MultithreadedMiller.hpp>
//***************************************************************************//

//***************************************************************************//
// Local Includes
#include "HeartbeatRequestThread.hpp" // Interface to this implementation
#include "ReliableProtocol.hpp"
#include "ReliablePacket.hpp"
#include "RTNPacket.hpp"
//***************************************************************************//

//***************************************************************************//
#define HEART_BEAT_PACE 1000 // in millisecond
#define MAXIMUM_NUMBER_OF_OUTSTANDING_HEARTBEATS 16
//***************************************************************************//

//***************************************************************************//
using namespace std;
//***************************************************************************//

//***************************************************************************//
namespace MST
{

  template class HeartbeatRequestThread<ReliablePacket>;
  template class HeartbeatRequestThread<RTNPacket>;

  template class ConnectionStatus<ReliablePacket>;
  template class ConnectionStatus<RTNPacket>;

  //-------------------------------------------------------------------------//
  template <class T>
  ConnectionStatus<T>::ConnectionStatus()
  {
    this->mulMaximumOutstandingHeartbeats = MAXIMUM_NUMBER_OF_OUTSTANDING_HEARTBEATS;
    this->mpTimer = new Timer();
    this->mdET = 0.0;
    this->mulLostHeartbeats = 0UL;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  ConnectionStatus<T>::~ConnectionStatus()
  {
    delete this->mpTimer;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  unsigned long ConnectionStatus<T>::Ping() const
  {
    unsigned ulReturnValue = 0UL;
    if(this->mdeqPingSamples.size())
    {
      ulReturnValue = static_cast<unsigned long>((this->mdRunningAggergatePingTime / (double)this->mdeqPingSamples.size()) * 1000.0);
    }

    return ulReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  float ConnectionStatus<T>::Reliability() const
  {
    float fReturnValue = 1.0F;
    if(this->mdeqLostReturnedQueue.size())
    {
      fReturnValue -= (static_cast<float>(this->mulLostHeartbeats) / static_cast<float>(this->mdeqLostReturnedQueue.size()));
    }
    return fReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  void ConnectionStatus<T>::DropLostRequest()
  {
    //unsigned long ulEvict = this->mdeqFIFOQueue.front();
    //this->mdeqFIFOQueue.pop_front();
    //map<unsigned long, double>::iterator itrOutstandingRequestMap;
    //itrOutstandingRequestMap = this->mmapOutStandingRequests.find(ulEvict);
    //assert(itrOutstandingRequestMap != this->mmapOutStandingRequests.end());
    //if(itrOutstandingRequestMap != this->mmapOutStandingRequests.end())
    //{
    //  this->mmapOutStandingRequests.erase(itrOutstandingRequestMap);
    //  if(this->mdeqLostReturnedQueue.size() >= this->mulMaximumOutstandingHeartbeats)
    //  {
    //    if(this->mdeqLostReturnedQueue.front())
    //      mulLostHeartbeats--;
    //    this->mdeqLostReturnedQueue.pop_front();
    //  }
    //  this->mdeqLostReturnedQueue.push_back(true);
    //  mulLostHeartbeats++;
    //}
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  void ConnectionStatus<T>::HeartbeatRequestSent(const T& rPacket)
  {
    this->mdET += this->mpTimer->DT(TIMER_1);
    //if(this->mdeqFIFOQueue.size() >= this->mulMaximumOutstandingHeartbeats)
    //{
    //  this->DropLostRequest();
    //}
    this->mmapOutStandingRequests.insert(make_pair(rPacket.GetSequence(), this->mdET));
    //this->mdeqFIFOQueue.push_back(rPacket.GetSequence());
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  void ConnectionStatus<T>::HeartbeatResponseRecv(const T& rPacket)
  {
    this->mdET += this->mpTimer->DT(TIMER_1);
    map<unsigned long, double>::iterator itrOutstandingRequestMap;
    itrOutstandingRequestMap = this->mmapOutStandingRequests.find(rPacket.GetSequence());
    if(itrOutstandingRequestMap != this->mmapOutStandingRequests.end())
    {
      double dPing = this->mdET - itrOutstandingRequestMap->second;
      if(this->mdeqPingSamples.size() > this->mulMaximumOutstandingHeartbeats)
      {
        this->mdRunningAggergatePingTime -= this->mdeqPingSamples.front();
        this->mdeqPingSamples.pop_front();
      }

      this->mdeqPingSamples.push_back(dPing);
      this->mdRunningAggergatePingTime += dPing;

      if(this->mdeqLostReturnedQueue.size() >= this->mulMaximumOutstandingHeartbeats)
      {
        if(this->mdeqLostReturnedQueue.front())
          mulLostHeartbeats--;
      }
      this->mdeqLostReturnedQueue.push_back(false);
      mulLostHeartbeats--;

      this->mmapOutStandingRequests.erase(itrOutstandingRequestMap);
    }
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  HeartbeatRequestThread<T>::HeartbeatRequestThread(class UDP<T>* pUDP)
  {
    assert(pUDP);
    this->mpUDP = pUDP;
    this->mpTimer = new Timer();
    this->mpMutex = new MST::Mutex();
    this->mpMonitoringListMutex = new MST::Mutex();

    Thread::SetRoutine(RequestThread, this);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  HeartbeatRequestThread<T>::~HeartbeatRequestThread()
  {
    delete this->mpTimer;
    delete this->mpMutex;
    delete this->mpMonitoringListMutex;
    this->mpUDP = NULL;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool HeartbeatRequestThread<T>::SubmitResponse(const T& rPacket)
  {
    bool bReturnValue = false;
    this->mpMonitoringListMutex->Lock();
    map<EndPoint, ConnectionStatus<T>*>::iterator itrMonitorMap;
    itrMonitorMap = this->mmapMonitoringList.find(rPacket.Source());
    if(itrMonitorMap != this->mmapMonitoringList.end())
    {
      itrMonitorMap->second->HeartbeatResponseRecv(rPacket);
    }
    this->mpMonitoringListMutex->Unlock();
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  void HeartbeatRequestThread<T>::RequestThread(void* vpParameters)
  {
    class HeartbeatRequestThread* pThis = static_cast<HeartbeatRequestThread*>(vpParameters);

    pThis->SignalStarted();

    T* pPacket = new T();

    while(pThis->Continue())
    {
      map<EndPoint, ConnectionStatus<T>*>::iterator itrMonitorMap;
      pThis->mpMonitoringListMutex->Lock();
      itrMonitorMap = pThis->mmapMonitoringList.begin();
      while(itrMonitorMap != pThis->mmapMonitoringList.end())
      {
        pPacket->Destination(itrMonitorMap->first);
        pPacket->GenerateHeartbeatRequest();
        pThis->mpUDP->Send(pPacket);
        itrMonitorMap->second->HeartbeatRequestSent(*pPacket);
        itrMonitorMap++;
      }
      pThis->mpMonitoringListMutex->Unlock();

      pThis->Pause(HEART_BEAT_PACE);
    }

    delete pPacket;

    pThis->SignalStopped();
    pThis->End();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool HeartbeatRequestThread<T>::AddEndPoint(const class EndPoint& rEndPoint)
  {
    bool bReturnValue = false;
    this->mpMonitoringListMutex->Lock();
    map<EndPoint, ConnectionStatus<T>*>::iterator itrMonitorMap;
    itrMonitorMap = this->mmapMonitoringList.find(rEndPoint);
    if(itrMonitorMap == this->mmapMonitoringList.end())
    {
      this->mmapMonitoringList.insert(make_pair(rEndPoint, new ConnectionStatus<T>()));
      bReturnValue = true;
    }
    this->mpMonitoringListMutex->Unlock();
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//
} // End of namespace MST
//***************************************************************************//
