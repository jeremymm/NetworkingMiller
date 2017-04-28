//***************************************************************************//
// Reliable UDP Server Class Interface
//
// Created: May 16, 2009
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

#ifndef MST_RELIABLE_UDP_SERVER_HPP
#define MST_RELIABLE_UDP_SERVER_HPP

//***************************************************************************//
#include "ReliableUDP.hpp"
#include "UDPServer.hpp"

#include "ReliablePacket.hpp"
//***************************************************************************//

//***************************************************************************//
namespace MST
{

  //*************************************************************************//
  template <class T>
  class ReliableUDPServer  : public ReliableUDP<T>
  {
  private:
  protected:
  public:
    ReliableUDPServer();
    virtual ~ReliableUDPServer();

    virtual bool _cdecl Listen(unsigned short usPort);
    virtual bool _cdecl Listen(const class EndPoint& rLocal);    
  };
  //*************************************************************************//
}
//***************************************************************************//

#endif MST_RELIABLE_UDP_SERVER_HPP
