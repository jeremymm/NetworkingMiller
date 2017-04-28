//***************************************************************************//
// Reliable Protocol Class Implementation
//
// Created: July 4, 2009
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
#include <iostream>
//***************************************************************************//

//***************************************************************************//
// MST Technologies Includes
#include <MultithreadedMiller.hpp>
//***************************************************************************//

//***************************************************************************//
// Local Includes
#include "ReliablePacket.hpp"
#include "RTNPacket.hpp"
#include "ReliableProtocol.hpp"
#include "PacketArchive.hpp"
#include "HeartbeatRequestThread.hpp"
#include "HeartbeatResponseThread.hpp"
//***************************************************************************//

//***************************************************************************//
#define MAX_NUMBER_OF_RETRANSMITS 5
#define MAX_NUMBER_OF_OUTSTANDING_PACKETS MAX_NUMBER_OF_RETRANSMITS
//***************************************************************************//

//***************************************************************************//
using namespace std;
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  template class ReliableProtocol<ReliablePacket>;
  template class ReliableProtocol<RTNPacket>;

  //*************************************************************************//
  // Start of ReliableProtocol implementation
  //-------------------------------------------------------------------------//
  template <class T>
  ReliableProtocol<T>::ReliableProtocol(class UDP<T>* pUDP) 
  {
    this->mpPacketArchiver = new PacketArchiverThread<T>(pUDP);
    this->mpHeartBeatRequestThread = new HeartbeatRequestThread<T>(pUDP);
    this->mpHeartBeatResponseThread = new HeartbeatResponseThread<T>(pUDP);
    this->mpMutex = new Mutex();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  ReliableProtocol<T>::~ReliableProtocol()
  {
    try
    {
      // Delete the threads first since we passed a pointer to an message buffer
      // that this destructor will delete after these threads have been cleaned up.
      if(this->mpHeartBeatRequestThread)
        delete this->mpHeartBeatRequestThread;
      if(this->mpHeartBeatResponseThread)
        delete this->mpHeartBeatRequestThread;

      this->mpHeartBeatRequestThread = NULL;
      this->mpHeartBeatResponseThread = NULL;

      if(this->mpPacketArchiver)
        delete this->mpPacketArchiver;

      if(this->mpMutex)
        delete this->mpMutex;
    }
    catch(...)
    {
    }
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool ReliableProtocol<T>::StartThreads()
  {
    bool bReturnValue = false;
    if(this->mpHeartBeatRequestThread->Start() && 
      this->mpHeartBeatResponseThread->Start() &&
      this->mpPacketArchiver->Start())
      bReturnValue = true;
    return bReturnValue;      
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool ReliableProtocol<T>::StopThreads()
  {
    bool bReturnValue = false;
    if(this->mpHeartBeatRequestThread->Stop() && 
      this->mpHeartBeatResponseThread->Stop() &&
      this->mpPacketArchiver->Stop())
      bReturnValue = true;
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool ReliableProtocol<T>::HasPacketsToRetransmit() const
  {
    bool bReturnValue = false;
    this->mpMutex->Lock();
    if(this->mdeqSendQueue.size())
      bReturnValue = true;  
    this->mpMutex->Unlock();

    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  T& ReliableProtocol<T>::RetransmitQueueFront()
  {
    return this->mdeqSendQueue.front();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool ReliableProtocol<T>::PopRetransmitQueue()
  {
    bool bReturnValue = false;
    this->mpMutex->Lock();
    if(this->mdeqSendQueue.size())
    {
      this->mdeqSendQueue.pop_front();
      bReturnValue = true;
    }
    this->mpMutex->Unlock();
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool ReliableProtocol<T>::SubmitPacket(const T& rPacket)
  {
    bool bReturnValue = false;

    this->mpMutex->Lock();
    map<EndPoint, unsigned long>::iterator itrSequenceMap;
    itrSequenceMap = this->mmapSequenceNumbers.find(rPacket.Source());
    if(itrSequenceMap == this->mmapSequenceNumbers.end())
    {
      pair<map<EndPoint, unsigned long>::iterator, bool> pairResult;
      pairResult = this->mmapSequenceNumbers.insert(make_pair(rPacket.Source(), rPacket.GetSequence()));
      if(pairResult.second)
        itrSequenceMap = pairResult.first;
    }

    if(rPacket.IsHeartbeatRequest())
    {
      bReturnValue = this->mpHeartBeatResponseThread->SubmitRequest(rPacket);
    }
    else if(rPacket.IsHeartbeatResponse())
    {
      bReturnValue = this->mpHeartBeatRequestThread->SubmitResponse(rPacket);
    }
    else if(rPacket.IsNAK())
    {
      bReturnValue = this->ProcessNAK(rPacket);
    }
    
    if((itrSequenceMap != this->mmapSequenceNumbers.end()) && (rPacket.GetSequence() > (itrSequenceMap->second + 1)))
    {
      unsigned ulStart = (itrSequenceMap->second + 1);
      unsigned ulStop = rPacket.GetSequence();
      if(ulStop - ulStart > MAX_NUMBER_OF_RETRANSMITS)
        ulStop = ulStart + MAX_NUMBER_OF_RETRANSMITS;

      bool bError = false;
      while(ulStart < ulStop&& !bError)
      {
        bError = !this->SubmitNAK(ulStart, rPacket.Source());
        ulStart++;
      }
      if(!bError)
        bReturnValue = true;
      
    }
    else
      itrSequenceMap->second = rPacket.GetSequence();
    this->mpMutex->Unlock();

    
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool ReliableProtocol<T>::PreparePacket(T& rPacket)
  {
    bool bReturnValue = true;
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool ReliableProtocol<T>::ProcessNAK(const T& rNAKPacket)
  {
    bool bReturnValue = false;
    const T* const pPacket = this->mpPacketArchiver->GetArchivedPacket(rNAKPacket);
    if(pPacket)
    {
      this->mdeqSendQueue.push_back(*pPacket);
      bReturnValue = true;
    }
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool ReliableProtocol<T>::SubmitNAK(unsigned long ulSequenceNumber, const class EndPoint& rDestination)
  {
    bool bReturnValue = false;
    T* pNAKPacket = new T();
    if(pNAKPacket->GenerateNAK(ulSequenceNumber, rDestination))
    {
      this->mdeqSendQueue.push_back(*pNAKPacket);
      delete pNAKPacket;
      bReturnValue = true;
    }
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool ReliableProtocol<T>::Monitor(class EndPoint& rEndPoint)
  {
    return this->mpHeartBeatRequestThread->AddEndPoint(rEndPoint);
  }
  //-------------------------------------------------------------------------//
  // End of class ReliableProtocol implementation
  //*************************************************************************//
} // End of namespace MST
//***************************************************************************//
