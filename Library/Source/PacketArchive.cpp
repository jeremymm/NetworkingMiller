//***************************************************************************//
// PacketArchive Class Interface
//
// Created: July 1, 2008
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
using namespace std;
//***************************************************************************//

//***************************************************************************//
// Local Includes
#include "ReliablePacket.hpp"
#include "RTNPacket.hpp"
#include "PacketArchive.hpp"
//***************************************************************************//

//***************************************************************************//
using namespace std;
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //*************************************************************************//
  // Start of implementation for class ArchivedPacket
  
  //-------------------------------------------------------------------------//
  // Explicit template instantiation for class ArchivedPacket
  template class ArchivedPacket<ReliablePacket>;
  template class ArchivedPacket<RTNPacket>;
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  ArchivedPacket<T>::ArchivedPacket()
  {
    Create();
  }
  //-------------------------------------------------------------------------//
  
  //-------------------------------------------------------------------------//
  template <class T>
  ArchivedPacket<T>::~ArchivedPacket()
  {
    Destroy();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  ArchivedPacket<T>::ArchivedPacket(const ArchivedPacket& rSource)
  {
    Create();
    Clone(rSource);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  void ArchivedPacket<T>::Create()
  {
    this->mpPacket =  new T();
    this->mpMutex = new Mutex();
    
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  void ArchivedPacket<T>::Clone(const ArchivedPacket& rSource)
  {
    *this->mpPacket = *rSource.mpPacket;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  void ArchivedPacket<T>::Destroy()
  {
    try
    {
      this->mpMutex->Lock();
      if(this->mpPacket)
        delete this->mpPacket;
      this->mpMutex->Unlock();

      delete this->mpMutex;

    }
    catch (...)
    {
    }
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool ArchivedPacket<T>::operator < (const ArchivedPacket<T>& rRHS) const
  {
    bool bReturnValue = false;
    this->mpMutex->Lock();
    if(this->mpPacket->Destination() < rRHS.mpPacket->Destination())
      bReturnValue = true;
    else if((this->mpPacket->Destination() == rRHS.mpPacket->Destination()) &&
      (this->mpPacket->GetSequence() < rRHS.mpPacket->GetSequence()))
      bReturnValue = true;
    this->mpMutex->Unlock();
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  ArchivedPacket<T>& ArchivedPacket<T>::operator=(const ArchivedPacket<T>& rRHS)
  {
    this->mpMutex->Lock();
    if(&rRHS != this)
    {
      this->Clone(rRHS);
    }
    this->mpMutex->Unlock();
    return *this;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool ArchivedPacket<T>::SetPacket(const T& rMessaage)
  {
    bool bReturnValue = false;
    this->mpMutex->Lock();
    *this->mpPacket = rMessaage;
    this->mpMutex->Unlock();
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  const T* const ArchivedPacket<T>::GetPacket() const
  {
    return this->mpPacket;
  }
  //-------------------------------------------------------------------------//
  // End of implementation for class ArchivedPacket
  //*************************************************************************//

  //*************************************************************************//
  // Start of implementation for the class PacketArchiverThread

  //-------------------------------------------------------------------------//
  // Explicit template instantiation for class PacketArchive
  template class PacketArchiverThread<ReliablePacket>;
  template class PacketArchiverThread<RTNPacket>;
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  PacketArchiverThread<T>::PacketArchiverThread(class UDP<T>* pUDP)
  {
    this->mpUDP = pUDP;
    this->mulMaxArchivedPackets = 16;
    this->mpMutex = new Mutex();
    Thread::SetRoutine(ArchiverThread, this);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  PacketArchiverThread<T>::~PacketArchiverThread()
  {
    if(this->mpMutex)
      delete this->mpMutex;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  void PacketArchiverThread<T>::ArchiverThread(void* vpParameters)
  {
    class PacketArchiverThread* pThis = static_cast<PacketArchiverThread*>(vpParameters);
    pThis->SignalStarted();

    T* pPacket = new T();
    while(pThis->Continue())
    {
      pThis->mpUDP->WaitOnOutgoing();
      pThis->mpMutex->Lock();
      while(pThis->mpUDP->CheckOutgoing())
      {
        pThis->mpUDP->PopOutgoingPacket(pPacket);
        if(!pPacket->IsNAK() && !pPacket->IsHeartbeatResponse() && !pPacket->IsHeartbeatRequest())
          pThis->AddPacket(*pPacket);

      }
      pThis->mpMutex->Unlock();
    }
    delete pPacket;
    pThis->SignalStopped();
    pThis->End();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool PacketArchiverThread<T>::AddPacket(const T& rPacket)
  {
    bool bReturnValue = false;

    ArchivedPacket<T> pArchivedPacket;
    pArchivedPacket.SetPacket(rPacket);

    std::pair< set< ArchivedPacket<T>>::iterator, bool > pairInsert;

    pairInsert = this->mmapArchivedPacketsIndex.insert(pArchivedPacket);
    if(pairInsert.second)
    {
      if(this->mdArchivedPackets.size() + 1 > this->mulMaxArchivedPackets)
      {
        this->mmapArchivedPacketsIndex.erase(this->mdArchivedPackets.front());
        this->mdArchivedPackets.pop_front();
      }
      this->mdArchivedPackets.push_back(pairInsert.first);
      bReturnValue = true;
    }

    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  const T* const PacketArchiverThread<T>::GetArchivedPacket(const T& rNAKPacket)
  {
    this->mpMutex->Lock();
    ArchivedPacket<T> pArchivedPacket;
    T* pPacket = new T();
    *pPacket = rNAKPacket;
    pPacket->Destination(rNAKPacket.Source());
    pPacket->Source(rNAKPacket.Destination());
    unsigned long ulMissingSequnce;
    memcpy(&ulMissingSequnce, rNAKPacket.Payload(), sizeof(ulMissingSequnce));
    pPacket->SetSequence(ulMissingSequnce);
    pArchivedPacket.SetPacket(*pPacket);

    set<ArchivedPacket<T>>::const_iterator itrOutstandingPackets;

    itrOutstandingPackets = this->mmapArchivedPacketsIndex.find(pArchivedPacket);
    if(itrOutstandingPackets != this->mmapArchivedPacketsIndex.end())
    { 
      delete pPacket;

      this->mpMutex->Unlock();
      return itrOutstandingPackets->GetPacket();
    }
    delete pPacket;
    this->mpMutex->Unlock();
    return NULL;
  }
  //-------------------------------------------------------------------------//

  // End of implementation for the class PacketArchiverThread 
  //*************************************************************************//
} // End of namespace MST
//***************************************************************************//