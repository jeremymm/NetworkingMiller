//***************************************************************************//
// Packet Class Implementation
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
#include <memory>
#include <string>
//***************************************************************************//

#include <MultithreadedMiller.hpp>
//***************************************************************************//
// Local Includes
#include "Packet.hpp"
//***************************************************************************//

//***************************************************************************//
using namespace std;
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //*************************************************************************//
  // Start of class Packet implementation
  //-------------------------------------------------------------------------//
  Packet::Packet()
  {
    Create();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  Packet::Packet(const class Packet& rSource)
  {
    Create();
    Clone(rSource);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  Packet::~Packet()
  {
    try
    {
      Destroy();
    }
    catch(...)
    {
    }
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void Packet::Create()
  {
    this->mpPacketMutex = new Mutex();

    this->mePriority = NORMAL_PACKET_PRIORITY;

    this->meType = UNKNOWN_PACKET_TYPE;

    this->mulPayloadSize = 0UL;

    this->mcpBuffer = new char[MAX_PACKET_SIZE];
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void Packet::Clone(const Packet& rSource)
  {
    this->meType           = rSource.meType;
    this->mePriority       = rSource.mePriority;

    this->mDesitination    = rSource.mDesitination;
    this->mSource          = rSource.mSource;

    this->mulPayloadSize   = rSource.mulPayloadSize;
    assert(rSource.mcpBuffer);
    if(rSource.mcpBuffer)
    {
      memcpy(this->mcpBuffer, rSource.mcpBuffer, this->mulPayloadSize);
    }
    //---------------------------------------------------------------------//
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void Packet::Destroy()
  {
    try
    {
      if(this->mpPacketMutex)
      {
        delete this->mpPacketMutex;
        this->mpPacketMutex = NULL;
      }

      if(this->mcpBuffer)
      {
        delete [] this->mcpBuffer;
        this->mcpBuffer = NULL;
      }
    }
    catch(...)
    {

    }
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool Packet::operator < (const class Packet& rRHS) const
  {
    bool bReturnValue = false;
    this->mpPacketMutex->Lock();
    if(this->mePriority < rRHS.mePriority)
      bReturnValue = true;
    else
      bReturnValue = false;
    this->mpPacketMutex->Unlock();
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  Packet& Packet::operator=(const class Packet& rRHS)
  {
    this->mpPacketMutex->Lock();
    if(&rRHS != this)
    {
      this->Clone(rRHS);
    }
    this->mpPacketMutex->Unlock();
    return *this;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  const class EndPoint& Packet::Source() const
  { 
    return this->mSource;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void Packet::Source(const class EndPoint& rEndPoint)
  { 
    this->mpPacketMutex->Lock();
    this->mSource = rEndPoint;
    this->mpPacketMutex->Unlock();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void Packet::Source(unsigned long ulIP, unsigned short usPort)
  { 
    this->mpPacketMutex->Lock();
    this->mSource.IP(ulIP); 
    this->mSource.Port(usPort);
    this->mpPacketMutex->Unlock();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  const class EndPoint& Packet::Destination() const
  { 
    return mDesitination;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void Packet::Destination(unsigned long ulIP, unsigned short usPort)
  { 
    this->mpPacketMutex->Lock();
    this->mDesitination.IP(ulIP);
    this->mDesitination.Port(usPort);
    this->mpPacketMutex->Unlock();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void Packet::Destination(const class EndPoint& rEndPoint)
  {
    this->mDesitination = rEndPoint;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  const char* const Packet::Payload() const
  {
    if(this->mcpBuffer)
      return this->mcpBuffer;
    else
      return NULL;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  unsigned long Packet::PayloadSize() const
  {
    return this->mulPayloadSize;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void Packet::Clear()
  {
    this->mpPacketMutex->Lock();

    if(this->mcpBuffer)
    {
      memset(this->mcpBuffer, 0UL, MAX_PACKET_SIZE);
    }

    this->mePriority = NORMAL_PACKET_PRIORITY;
    this->meType = UNKNOWN_PACKET_TYPE;
    this->mulPayloadSize = 0UL;

    this->mpPacketMutex->Unlock();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool Packet::SetPayload(const char* const cpData, unsigned long ulSize, enum PacketType eType)
  {
    bool bReturnValue = false;
    if(cpData)
    {
      this->mpPacketMutex->Lock();
      if(eType != UNKNOWN_PACKET_TYPE)
        meType = eType;

      memcpy(this->mcpBuffer, cpData, ulSize);
      bReturnValue = true;
      this->mulPayloadSize = ulSize;
      mePriority = NORMAL_PACKET_PRIORITY;
      this->mpPacketMutex->Unlock();
    }

    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  unsigned long Packet::Serialize()
  {
    unsigned long ulReturnValue = 0UL;
    this->mpPacketMutex->Lock();
    if(this->mcpBuffer)
    {

    }
    this->mpPacketMutex->Unlock();
    return ulReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  unsigned long Packet::Deserialize()
  {
    unsigned long ulReturnValue = 0UL;
    this->mpPacketMutex->Lock();
    if(this->mcpBuffer)
    {

    }
    this->mpPacketMutex->Unlock();
    return ulReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void Packet::FillString(std::string& rstrString)
  {
    this->mpPacketMutex->Lock();
    if(this->mcpBuffer)
    {
      rstrString.erase(rstrString.begin(), rstrString.end());
      rstrString.append(reinterpret_cast<char*>(this->mcpBuffer), this->mulPayloadSize);
    }
    this->mpPacketMutex->Unlock();
  }
  //-------------------------------------------------------------------------//
  // End of class Packet implementation
  //*************************************************************************//
} // End of namespace MST
//***************************************************************************//
