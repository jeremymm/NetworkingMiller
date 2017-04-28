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

#ifndef MST_RELIABLE_UDP_HPP
#define MST_RELIABLE_UDP_HPP

//***************************************************************************//
#include "UDP.hpp"
#include "ReliableProtocol.hpp"
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //*************************************************************************//
  template <class T>
  class ReliableUDP  : public UDP<T>
  {
  private:
  protected:
    ReliableProtocol<T>* mpReliableProtocol;
  public:
    ReliableUDP();
    virtual ~ReliableUDP();

    //-----------------------------------------------------------------------//
    virtual long _cdecl Recv(T* const pPacket);
    virtual long _cdecl Send(T* const pPacket);

    //-----------------------------------------------------------------------//
    virtual bool _cdecl WaitOnIncoming(unsigned long ulTimeout = ULONG_MAX);
    //-----------------------------------------------------------------------//
  };
  //*************************************************************************//
}
//***************************************************************************//

#endif MST_RELIABLE_UDP_HPP
