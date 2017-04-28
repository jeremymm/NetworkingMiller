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

//***************************************************************************//
// System Includes
#include <assert.h>
//***************************************************************************//

//***************************************************************************//
// MST Technologies Include
#include <MultithreadedMiller.hpp>
//***************************************************************************//

//***************************************************************************//
// Local Includes
#include "RTNPacket.hpp"
//***************************************************************************//

//***************************************************************************//
#define ACTOR_MASK         0x10000000
#define VIEWER_MASK        0x20000000
#define CONNECT_MASK       0x40000000
#define DISCONNECT_MASK    0x80000000
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //-------------------------------------------------------------------------//
  RTNPacket::RTNPacket() : ReliablePacket()
  {
    this->Create();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  RTNPacket::RTNPacket(const class RTNPacket& rSource) : ReliablePacket(rSource)
  {
    this->Create();
    this->Clone(rSource);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  RTNPacket::~RTNPacket()
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
  void RTNPacket::Create()
  {
    ReliablePacket::mulHeaderSize = sizeof(Packet::mulPayloadSize) + sizeof(ReliablePacket::mulSequence) + sizeof(ReliablePacket::mulMask)
      + sizeof(mulID) + sizeof(mfX) + sizeof(mfY) + sizeof(mfZ);
    this->mulID = 0L;
    this->mfX = 0.0F;
    this->mfY = 0.0F;
    this->mfZ = 0.0F;
    this->mpRTNPacketMutex = new Mutex();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void RTNPacket::Clone(const RTNPacket& rSource)
  {
    this->mulID = rSource.mulID;
    this->mfX = rSource.mfX;
    this->mfY = rSource.mfY;
    this->mfZ = rSource.mfZ;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void RTNPacket::Destroy()
  {
    if(this->mpRTNPacketMutex)
      delete this->mpRTNPacketMutex;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  RTNPacket& RTNPacket::operator=(const class RTNPacket& rRHS)
  {
    this->mpRTNPacketMutex->Lock();
    if(&rRHS != this)
    {
      ReliablePacket::operator=(rRHS);
      this->Clone(rRHS);
    }
    this->mpRTNPacketMutex->Unlock();
    return *this;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  unsigned long RTNPacket::ID() const
  {
    return this->mulID;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void RTNPacket::ID(unsigned long ulId)
  {
    this->mulID = ulId;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  float RTNPacket::X() const
  {
    return this->mfX;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  float RTNPacket::Y() const
  {
    return this->mfY;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  float RTNPacket::Z() const
  {
    return this->mfZ;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void RTNPacket::X(float fX)
  {
    this->mfX = fX;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void RTNPacket::Y(float fY)
  {
    this->mfY = fY;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void RTNPacket::Z(float fZ)
  {
    this->mfZ = fZ;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void RTNPacket::SetPosition(float fX, float fY, float fZ)
  {
    this->mpRTNPacketMutex->Lock();
    this->mfX = fX;
    this->mfY = fY;
    this->mfZ = fZ;
    this->mpRTNPacketMutex->Unlock();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void RTNPacket::MarkFromViewer()
  {
    this->mpRTNPacketMutex->Lock();
    this->mpReliablePacketMutex->Lock();
    ReliablePacket::mulMask |= VIEWER_MASK;
    this->mpReliablePacketMutex->Unlock();
    this->mpRTNPacketMutex->Unlock();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void RTNPacket::MarkFromActor()
  {
    this->mpRTNPacketMutex->Lock();
    this->mpReliablePacketMutex->Lock();
    ReliablePacket::mulMask |= ACTOR_MASK;
    this->mpReliablePacketMutex->Unlock();
    this->mpRTNPacketMutex->Unlock();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool RTNPacket::IsFromViewer()
  {
    bool bReturnValue = false;
    this->mpRTNPacketMutex->Lock();
    this->mpReliablePacketMutex->Lock();
    if(ReliablePacket::mulMask & VIEWER_MASK)
      bReturnValue = true;
    this->mpReliablePacketMutex->Unlock();
    this->mpRTNPacketMutex->Unlock();
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool RTNPacket::IsFromActor()
  {
    bool bReturnValue = false;
    this->mpRTNPacketMutex->Lock();
    this->mpReliablePacketMutex->Lock();
    if(ReliablePacket::mulMask & ACTOR_MASK)
      bReturnValue = true;
    this->mpReliablePacketMutex->Unlock();
    this->mpRTNPacketMutex->Unlock();
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void RTNPacket::GenerateConnectPacket()
  {
    this->mpRTNPacketMutex->Lock();
    this->mpReliablePacketMutex->Lock();
    ReliablePacket::mulMask |= CONNECT_MASK;
    this->mpReliablePacketMutex->Unlock();
    this->mpRTNPacketMutex->Unlock();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void RTNPacket::GenerateDisconnectPacket()
  {
    this->mpRTNPacketMutex->Lock();
    this->mpReliablePacketMutex->Lock();
    ReliablePacket::mulMask |= DISCONNECT_MASK;
    this->mpReliablePacketMutex->Unlock();
    this->mpRTNPacketMutex->Unlock();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  unsigned long RTNPacket::Deserialize()
  {
    unsigned long ulReturnValue = 0UL;
    unsigned long ulIndex = 0;
    this->mpRTNPacketMutex->Lock();

    ulIndex = ReliablePacket::Deserialize();

    memcpy(&(this->mulID), Packet::mcpBuffer + ulIndex, sizeof(this->mulID));
    ulIndex += sizeof(this->mulID);

    memcpy(&(this->mfX), Packet::mcpBuffer + ulIndex, sizeof(this->mfX));
    ulIndex += sizeof(this->mfX);

    memcpy(&(this->mfY), Packet::mcpBuffer + ulIndex, sizeof(this->mfY));
    ulIndex += sizeof(this->mfY);

    memcpy(&(this->mfZ), Packet::mcpBuffer + ulIndex, sizeof(this->mfZ));
    ulIndex += sizeof(this->mfZ);

    ulReturnValue = ulIndex;
    this->mpRTNPacketMutex->Unlock();
    return ulReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  unsigned long RTNPacket::Serialize()
  {
    unsigned long ulReturnValue = 0UL;
    unsigned long ulIndex = 0;
    this->mpRTNPacketMutex->Lock();
    ulIndex = ReliablePacket::Serialize();

    memcpy(Packet::mcpBuffer + ulIndex, &(this->mulID), sizeof(this->mulSequence));
    ulIndex += sizeof(this->mulID);

    memcpy(Packet::mcpBuffer + ulIndex, &(this->mfX), sizeof(this->mfX));
    ulIndex += sizeof(this->mfX);

    memcpy(Packet::mcpBuffer + ulIndex, &(this->mfY), sizeof(this->mfY));
    ulIndex += sizeof(this->mfY);

    memcpy(Packet::mcpBuffer + ulIndex, &(this->mfZ), sizeof(this->mfZ));
    ulIndex += sizeof(this->mfZ);

    ulReturnValue = ulIndex;
    this->mpRTNPacketMutex->Unlock();
    return ulReturnValue;
  }
  //-------------------------------------------------------------------------//
}
//***************************************************************************//


