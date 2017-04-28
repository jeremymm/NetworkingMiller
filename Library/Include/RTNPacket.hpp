//***************************************************************************//
// RTN (Real Time Network) Subscriber Class Interface
//
// Created: Aug 3, 2008
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

#ifndef MST_RTN_PACKET_HPP
#define MST_RTN_PACKET_HPP

//***************************************************************************//
// Local Include
#include "ReliablePacket.hpp"
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //*************************************************************************//
  class RTNPacket : public ReliablePacket
  {
  private:
    //***********************************************************************//
    unsigned long mulID;
    float mfX;
    float mfY;
    float mfZ;
    
    void Create();
    void Destroy();
    //***********************************************************************//
  protected:
    //***********************************************************************//
    class MST::Mutex* mpRTNPacketMutex;
    void Clone(const class RTNPacket& rSource);
    //***********************************************************************//
  public:
    //***********************************************************************//
    //-----------------------------------------------------------------------//
    RTNPacket();
    RTNPacket(const class RTNPacket& rSource);
    virtual ~RTNPacket();
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    RTNPacket& operator=(const RTNPacket& rRHS);
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    virtual unsigned long _cdecl Serialize();
    virtual unsigned long _cdecl Deserialize();
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    unsigned long _cdecl ID() const;
    void _cdecl ID(unsigned long ulId);
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    float _cdecl X() const;
    float _cdecl Y() const;
    float _cdecl Z() const;

    void _cdecl X(float fX);
    void _cdecl Y(float fY);
    void _cdecl Z(float fZ);

    void _cdecl SetPosition(float fX, float fY, float fZ);
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    void _cdecl MarkFromViewer();
    void _cdecl MarkFromActor();

    bool _cdecl IsFromViewer();
    bool _cdecl IsFromActor();
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    void _cdecl GenerateConnectPacket();
    void _cdecl GenerateDisconnectPacket();
    //-----------------------------------------------------------------------//
    //***********************************************************************//
  }; // End of class RTNPacket : public ReliablePacket
  //*************************************************************************//
} // End of namespace MST
//***************************************************************************//

#endif MST_RTN_PACKET_HPP
