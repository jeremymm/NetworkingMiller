//***************************************************************************//
// Reliable Packet Class Implementation
//
// Created: May  16, 2009
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

// System Includes
#include <assert.h>

#include <MultithreadedMiller.hpp>

//***************************************************************************//
// Local Includes
#include "ReliablePacket.hpp"
//***************************************************************************//

//***************************************************************************//
#define NAK_MASK                0x00000001
#define HEARTBEAT_REQUEST_MASK  0x00000002
#define HEARTBEAT_RESPONSE_MASK 0x00000004
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //-------------------------------------------------------------------------//
  ReliablePacket::ReliablePacket() : Packet()
  {
    this->Create();
    this->meType = RELIABLE_PACKET_TYPE;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  ReliablePacket::ReliablePacket(const class ReliablePacket& rSource) : Packet(rSource)
  {
    this->Create();
    this->Clone(rSource);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  ReliablePacket::~ReliablePacket()
  {
    try
    {
      this->Destroy();
    }
    catch(...)
    {
      
    }
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void ReliablePacket::Create()
  {
    this->mulHeaderSize = sizeof(Packet::mulPayloadSize) + sizeof(this->mulSequence) + sizeof(this->mulMask);
    this->mulSequence = 0UL;
    this->mulMask = 0UL;
    this->mpReliablePacketMutex = new Mutex();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void ReliablePacket::Clone(const ReliablePacket& rSource)
  {
    this->mulMask = rSource.mulMask;
    this->mulSequence = rSource.mulSequence;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void ReliablePacket::Destroy()
  {
    if(this->mpReliablePacketMutex)
      delete this->mpReliablePacketMutex;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  ReliablePacket& ReliablePacket::operator=(const class ReliablePacket& rRHS)
  {
    this->mpReliablePacketMutex->Lock();
    if(&rRHS != this)
    {
      Packet::operator=(rRHS);
      this->Clone(rRHS);
    }
    this->mpReliablePacketMutex->Unlock();
    return *this;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void ReliablePacket::SetSequence(unsigned long ulSequence)
  {
    this->mpReliablePacketMutex->Lock();
    this->mulSequence = ulSequence;
    this->mpReliablePacketMutex->Unlock();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  unsigned long ReliablePacket::GetSequence() const
  {
    unsigned long ulReturnValue = 0UL;
    this->mpReliablePacketMutex->Lock();
    ulReturnValue = this->mulSequence;
    this->mpReliablePacketMutex->Unlock();
    return ulReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool ReliablePacket::SetHeader(const char* const cpData, unsigned long ulSize)
  {
    bool bReturnValue = false;
    this->mpReliablePacketMutex->Lock();
    assert(ulSize == this->HeaderSize());

    memcpy(this->mcpBuffer, cpData, ulSize);

    this->mpReliablePacketMutex->Unlock();
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  unsigned long ReliablePacket::HeaderSize() const
  {
    return this->mulHeaderSize;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool ReliablePacket::GenerateHeartbeatResponse()
  {
    bool bReturnValue = false;
    this->mpReliablePacketMutex->Lock();
    this->mulMask = HEARTBEAT_RESPONSE_MASK;
    this->mpReliablePacketMutex->Unlock();
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool ReliablePacket::GenerateHeartbeatRequest()
  {
    bool bReturnValue = true;
    this->mpReliablePacketMutex->Lock();
    this->mulMask = HEARTBEAT_REQUEST_MASK;
    this->mpReliablePacketMutex->Unlock();
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  bool ReliablePacket::GenerateNAK(unsigned long ulSequence, const class EndPoint& rDestination)
  {
    bool bReturnValue = true;
    this->mpReliablePacketMutex->Lock();
    this->mulMask = NAK_MASK;
    this->SetPayload((char*)&ulSequence, sizeof(ulSequence));
    this->mDesitination = rDestination;
    this->mpReliablePacketMutex->Unlock();
    return bReturnValue;
  }

  //-------------------------------------------------------------------------//
  unsigned long ReliablePacket::Deserialize()
  {
    unsigned long ulReturnValue = 0UL;
    unsigned long ulIndex = 0;
    this->mpReliablePacketMutex->Lock();

    ulIndex = Packet::Deserialize();

    // Deserialize the sequence number
    memcpy(&(this->mulSequence), Packet::mcpBuffer + ulIndex, sizeof(this->mulSequence));
    if(this->mulSequence > LONG_MAX)
      this->mulSequence = this->mulSequence - LONG_MAX;
    ulIndex += sizeof(this->mulSequence);

    // Deserialize the mask
    memcpy(&(this->mulMask), Packet::mcpBuffer + ulIndex, sizeof(this->mulMask));
    ulIndex += sizeof(this->mulMask);

    ulReturnValue = ulIndex;
    this->mpReliablePacketMutex->Unlock();
    return ulReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  unsigned long ReliablePacket::Serialize()
  {
    unsigned long ulReturnValue = 0UL;
    unsigned long ulIndex = 0;
    this->mpReliablePacketMutex->Lock();
    ulIndex = Packet::Serialize();

    // Serialize the sequence number
    memcpy(Packet::mcpBuffer + ulIndex, &(this->mulSequence), sizeof(this->mulSequence));
    ulIndex += sizeof(this->mulSequence);

    // Serialize the mask
    memcpy(Packet::mcpBuffer + ulIndex, &(this->mulMask), sizeof(this->mulMask));
    ulIndex += sizeof(this->mulMask);

    ulReturnValue = ulIndex;
    this->mpReliablePacketMutex->Unlock();
    return ulReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool ReliablePacket::IsHeartbeatRequest() const
  {
    bool bReturnValue = false;
    this->mpReliablePacketMutex->Lock();
    if(this->mulMask & HEARTBEAT_REQUEST_MASK)
      bReturnValue = true;
    this->mpReliablePacketMutex->Unlock();
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool ReliablePacket::IsHeartbeatResponse() const
  {
    bool bReturnValue = false;
    this->mpReliablePacketMutex->Lock();
    if(this->mulMask & HEARTBEAT_RESPONSE_MASK)
      bReturnValue = true;
    this->mpReliablePacketMutex->Unlock();
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool ReliablePacket::IsNAK() const
  {
    bool bReturnValue = false;
    this->mpReliablePacketMutex->Lock();
    if(this->mulMask & NAK_MASK)
      bReturnValue = true;
    this->mpReliablePacketMutex->Unlock();
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//
}
//***************************************************************************//


