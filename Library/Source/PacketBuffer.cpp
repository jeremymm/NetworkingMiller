//***************************************************************************//
// PacketBuffer Class Implementation
//
// Created: Dec 13, 2006
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
#include <process.h>
//***************************************************************************//

//***************************************************************************//
// Local Includes
#include "Packet.hpp"
#include "ReliablePacket.hpp"
#include "RTNPacket.hpp"
#include "PacketBuffer.hpp"
//***************************************************************************//

//***************************************************************************//
#define DEFAULT_MAX_PACKETS 256UL
//***************************************************************************//

//***************************************************************************//
using namespace std;
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  template class PacketBuffer<Packet>;
  template class PacketBuffer<ReliablePacket>;
  template class PacketBuffer<RTNPacket>;

  //*************************************************************************//
  // Start of PacketBuffer implementation
  //-------------------------------------------------------------------------//
  template <class T>
  PacketBuffer<T>::PacketBuffer()
  {
    mulMaxPackets = DEFAULT_MAX_PACKETS;
    mpInternalMutex = new Mutex();
    mpInsertEvent = new Event();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  PacketBuffer<T>::PacketBuffer(unsigned long ulMaxPackets)
  {
    mulMaxPackets = ulMaxPackets;
    mpInternalMutex = new MST::Mutex();
    mpInsertEvent = new MST::Event();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  PacketBuffer<T>::~PacketBuffer()
  {
    try
    {
      while(this->Length())
        this->Pop();

      delete mpInternalMutex;
      delete mpInsertEvent;
    }
    catch(...)
    {
    }
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool PacketBuffer<T>::Lock()
  {
    return Lock(ULONG_MAX);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool PacketBuffer<T>::Lock(unsigned long ulTimeout)
  {
    return this->mpInternalMutex->Lock(ulTimeout);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  void PacketBuffer<T>::Unlock()
  {
    this->mpInternalMutex->Unlock();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool PacketBuffer<T>::WaitForInsertion(unsigned long ulTimeout)
  {
    return this->mpInsertEvent->WaitForSignal(ulTimeout);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  const T& PacketBuffer<T>::Front() const
  {
    return this->mdPackets.front();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  void PacketBuffer<T>::Pop()
  {
    this->mpInternalMutex->Lock();
    if(this->mdPackets.size())
    {
      this->mdPackets.pop_front();
    }
    this->mpInternalMutex->Unlock();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool PacketBuffer<T>::Push(const T& rPacket, bool bSignalInsertion)
  {
    bool bReturnValue = false;
    
    this->mpInternalMutex->Lock();
    while(this->mdPackets.size() > mulMaxPackets)
      this->mdPackets.pop_front();

    this->mdPackets.push_back(rPacket);

    if(bSignalInsertion)
      bReturnValue = this->mpInsertEvent->Signal();
    else
      bReturnValue = true;

    this->mpInternalMutex->Unlock();
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  unsigned long PacketBuffer<T>::Length() const 
  { 
    unsigned long ulReturnValue = 0UL;
    this->mpInternalMutex->Lock();
    ulReturnValue = static_cast<unsigned long>(this->mdPackets.size()); 
    this->mpInternalMutex->Unlock();
    return ulReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  void PacketBuffer<T>::Length(unsigned long ulLength) 
  { 
    this->mpInternalMutex->Lock();
    this->mulMaxPackets = ulLength;
    this->mpInternalMutex->Unlock();
    return;
  }
  //-------------------------------------------------------------------------//
  // End of PacketBuffer implementation
  //*************************************************************************//
} // End of namespace MST
//***************************************************************************//
