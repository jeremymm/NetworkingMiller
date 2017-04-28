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

#ifndef MST_RTN_SUBSCRIBER_HPP
#define MST_RTN_SUBSCRIBER_HPP

//***************************************************************************//
// Local Includes
#include "ReliableUDPClient.hpp"
#include "RTNPacket.hpp"
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //*************************************************************************//
  class RTNSubscriber : public ReliableUDPClient<RTNPacket>
  {
  private:
    //***********************************************************************//
    //-----------------------------------------------------------------------//
    /// If subscriber actor state is set to true then corresponding instance is 
    /// acting on the network and therefore is thought of placing data on network
    /// for dissemination to other subscribers.
    bool mbIsActor;
    /// If subscriber viewer state is set to true then corresponding instance is 
    /// viewing network state and therefore is thought of as only viewing network
    /// data.
    bool mbIsViewer;
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    // Serialized
    /// Subscriber ID on the network
    unsigned long mulID;
    /// Subscriber location in 3 space. Note this may incorrectly assume this
    /// class will be used in a 3 space simulation. 
    /// An optimization this library will do is cull traffic based on spacial 
    /// proximity.
    /// \todo make this customizable 
    float mfX;
    float mfY;
    float mfZ;
    //-----------------------------------------------------------------------//
    //***********************************************************************//
  public:
    //***********************************************************************//
    //-----------------------------------------------------------------------//
    RTNSubscriber();
    virtual ~RTNSubscriber();
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    void _cdecl GetRTNPacket(unsigned long ulRemoteSubcriberID);
    virtual long _cdecl Recv(class RTNPacket* const pPacket);
    virtual long _cdecl Send(class RTNPacket* const pPacket);
    virtual long _cdecl Update();
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    unsigned long _cdecl ID() const { return this->mulID; }
    void _cdecl ID(unsigned long ulID) { this->mulID = ulID; }
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
    void _cdecl SetAsActor(bool bEnable);
    void _cdecl SetAsViewer(bool bEnable);
    bool _cdecl IsActor() const;
    bool _cdecl IsViewer() const;
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    // The below methods takes a port and host name in a char* and 
    // attempts to connect to that remote host.
    virtual long _cdecl Connect(const class EndPoint& rEndPoint);
    // Parameter 1: Port you wish to connect to in host order (little endian for x86).
    // Parameter 2: The host name or numerical address of the remote host. The char*
    //              must be null terminated.
    // Return Value: This method will return 1 on success and 0 on failure.
    virtual long _cdecl Connect(unsigned short usPort, const char* const cpHostName);
    // The below methods takes a port and host name in a std::string and 
    // attempts to connect to that remote host.
    // Parameter 1: Port you wish to connect to in host order (little endian for x86).
    // Parameter 2: The host name or numerical address of the remote host.
    // Return Value: This method will return 1 on success and 0 on failure.
    virtual long _cdecl Connect(unsigned short usPort, const std::string& strHostName);
    // The below methods takes a port and address in a unsigned long and 
    // attempts to connect to that remote host. The unsigned long IPv4 address.
    // must be in host order (little endian for x86).
    // Parameter 1: Port you wish to connect to in host order.
    // Parameter 2: The IPv4 Address of the remote host.
    // Return Value: This method will return 1 on success and 0 on failure.
    virtual long _cdecl Connect(unsigned short usPort, unsigned long ulIP);
    //-----------------------------------------------------------------------//
    //***********************************************************************//
  }; // End of class RTNSubscriber : public ReliableUDPClient<RTNPacket>
  //*************************************************************************//
} // End of namespace MST
//***************************************************************************//

#endif MST_RTN_SUBSCRIBER_HPP
