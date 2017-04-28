//***************************************************************************//
// Packet Class Interface
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

#ifndef MST_PACKET_HPP
#define MST_PACKET_HPP

//*************************************************************************//
// System Includes
#include <string>
//*************************************************************************//

//***************************************************************************//
// MST Technologies Includes
#include <MultithreadedMiller.hpp>
//***************************************************************************//

//*************************************************************************//
// Local Includes
#include "EndPoint.hpp"
//*************************************************************************//

//*************************************************************************//
namespace MST
{
  //*************************************************************************//
  const unsigned long MAX_PACKET_SIZE = 4096;
  //*************************************************************************//

  //*************************************************************************//
  enum PacketType
  {
    UNKNOWN_PACKET_TYPE     = 0x00000000,
    GENERIC_UDP_PACKET_TYPE = 0x00000001,
    GENERIC_TCP_PACKET_TYPE = 0x00000002,
    RELIABLE_PACKET_TYPE    = 0x00000005
  };
  //*************************************************************************//

  //*************************************************************************//
  enum PacketPriorityType
  {
    HIGHEST_PACKET_PRIORITY          = 0,
    ABOVE_NORMAL_PACKET_PRIORITY     = 1,
    NORMAL_PACKET_PRIORITY           = 2,
    BELOW_NORMAL_PACKET_PRIORITY     = 3,
    LOWEST_PACKET_PRIORITY           = 4,
    NUMBER_OF_PACKET_PRIORITY_LEVELS = 5
  };
  //*************************************************************************//

  //*************************************************************************//
  /// This class as an item in buffer so that messages can be prioritized and 
  /// send to their intended destinations.
  class Packet
  {
  private:
    //***********************************************************************//
    //***********************************************************************//
  protected:
    //***********************************************************************//
    //-----------------------------------------------------------------------//
    class MST::Mutex* mpPacketMutex;
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    /// This enumeration describes the type of %Packet this is.  This assists the
    /// PacketBuffer in determining how to handle the %Packet
    enum PacketType meType;

    /// This is the priority of the message.  
    enum PacketPriorityType mePriority;

    /// This object holds the address of the destination for this message 
    class EndPoint mDesitination;
    /// This object holds the address of the source of this message 
    class EndPoint mSource;

    unsigned long mulPayloadSize;

    char* mcpBuffer;

    //-----------------------------------------------------------------------//
    void Create();
    void Destroy();
    //-----------------------------------------------------------------------//
  protected:
    //***********************************************************************//
    void Clone(const Packet& rSource);
    //***********************************************************************//
  public:
    //***********************************************************************//
    //-----------------------------------------------------------------------//
    Packet();
    Packet(const class Packet& rSource);
    virtual ~Packet();
    //-----------------------------------------------------------------------//

    //----------------------- Overloaded Operators --------------------------//
    bool operator < (const Packet& rRHS) const;
    Packet& operator=(const Packet& rRHS);
    //-----------------------------------------------------------------------//

    //----------------- Destination and Source Access Methods ---------------//
    const class EndPoint& _cdecl Source() const;
    void _cdecl Source(const class EndPoint& rEndPoint);
    void _cdecl Source(unsigned long ulIP, unsigned short usPort);

    const class EndPoint& _cdecl Destination() const;
    void _cdecl Destination(const class EndPoint& rEndPoint);
    void _cdecl Destination(unsigned long ulIP, unsigned short usPort);
    //-----------------------------------------------------------------------//

    //----------------- Type and Priority Access Methods --------------------//
    enum PacketType _cdecl Type() const { return this->meType; }
    void _cdecl Type(enum PacketType eType) { this->meType = eType; }
    enum PacketPriorityType _cdecl Priority() const { return this->mePriority; };
    void _cdecl Priority(enum PacketPriorityType ePriority) { this->mePriority = ePriority; };
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    // Payload is the full size of of the memory of this message including any possible
    // header information added by a derived message type.
    virtual const char* const _cdecl Payload() const;
    virtual unsigned long _cdecl PayloadSize() const;
    virtual unsigned long _cdecl PayloadMaxSize() const { return MAX_PACKET_SIZE; }
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    virtual bool _cdecl NeedsSequence() const { return false; }
    virtual void _cdecl SetSequence(unsigned long ulSequence) {}
    virtual unsigned long _cdecl GetSequence() const { return 0UL; }
    //-----------------------------------------------------------------------//

    //------------------------ Data Access Methods --------------------------//
    virtual bool _cdecl SetPayload(const char* const cpData, unsigned long ulSize, enum PacketType eType = UNKNOWN_PACKET_TYPE);
    void _cdecl Clear();
    //-----------------------------------------------------------------------//

    //----------------- Serialize and Deserialize Methods -------------------//
    // Should never be called outside of SendWrapper and RecvWarpper
    virtual unsigned long _cdecl Serialize();
    virtual unsigned long _cdecl Deserialize();
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    /// \todo Need to change this output parameter  so that we don't get heap 
    /// corruption in the event we port the code a DLL library that will used
    /// a separate heap than the executable.
    virtual void _cdecl FillString(std::string& rstrString);
    //-----------------------------------------------------------------------//
    //***********************************************************************//
  }; // End of class Packet
  //*************************************************************************//
} // End of namespace MST
//***************************************************************************//

#endif MST_PACKET_HPP
