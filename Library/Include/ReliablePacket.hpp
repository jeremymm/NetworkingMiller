//***************************************************************************//
// ReliablePacket Class Interface
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

#ifndef MST_RELIABLE_PACKET_HPP
#define MST_RELIABLE_PACKET_HPP

//***************************************************************************//
// Local Includes
#include "Packet.hpp"
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //*************************************************************************//
  class ReliablePacket : public Packet
  {
  private:    
    //***********************************************************************//
    void Create();
    void Destroy();
    //***********************************************************************//
  protected:
    //***********************************************************************//
    //-----------------------------------------------------------------------//
    class MST::Mutex* mpReliablePacketMutex;
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    // Serialized
    unsigned long mulHeaderSize;
    unsigned long mulSequence;
    unsigned long mulMask;
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    void Clone(const class ReliablePacket& rSource);
    //-----------------------------------------------------------------------//
    //***********************************************************************//
  public:
    //***********************************************************************//
    //-----------------------------------------------------------------------//
    ReliablePacket();
    ReliablePacket(const class ReliablePacket& rSource);
    virtual ~ReliablePacket();
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    ReliablePacket& operator=(const ReliablePacket& rRHS);
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    virtual bool _cdecl NeedsSequence() const { return true; }
    virtual void _cdecl SetSequence(unsigned long ulSequence);
    virtual unsigned long _cdecl GetSequence() const;
    //-----------------------------------------------------------------------//

    virtual bool _cdecl SetHeader(const char* const cpData, unsigned long ulSize);
    unsigned long _cdecl HeaderSize() const;

    //-----------------------------------------------------------------------//
    virtual unsigned long _cdecl Serialize();
    virtual unsigned long _cdecl Deserialize();
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    bool _cdecl IsHeartbeatRequest() const;
    bool _cdecl IsHeartbeatResponse() const;
    bool _cdecl IsNAK() const;
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    bool _cdecl GenerateHeartbeatResponse();
    bool _cdecl GenerateHeartbeatRequest();
    bool _cdecl GenerateNAK(unsigned long ulSequence, const class EndPoint& rDestination);
    //-----------------------------------------------------------------------//
    //***********************************************************************//
  }; // End of class ReliablePacket : public Packet
  //*************************************************************************//
} // End of namespace MST
//***************************************************************************//

#endif MST_RELIABLE_PACKET_HPP
