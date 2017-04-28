//***************************************************************************//
// UDP Class Interface
//
// Created: Feb 20, 2006
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

#ifndef MST_UDP_HPP
#define MST_UDP_HPP

//***************************************************************************//
#include "Socket.hpp"
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //*************************************************************************//
  /// The UDP class a base class for all classes that use UDP based sockets
  template <class T>
  class UDP : public Socket<T>
  {
  private:
    bool mbBroadcastEnabled;
  protected:
    /// This method creates a operating system socket that can be used for UDP
    /// based classes.
    unsigned long CreateDescriptor();
    bool BroadcastEnabled() const;
  public:
    //***********************************************************************//
    //-----------------------------------------------------------------------//
    UDP();
    virtual ~UDP();
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    virtual bool _cdecl Broadcast(bool bEnable);
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    virtual long _cdecl Recv(T* const pPacket);
    virtual long _cdecl Send(T* const pPacket);
    //-----------------------------------------------------------------------//
    //***********************************************************************//
  }; // End of class UDP : public Socket
  //*************************************************************************//
} // End of namespace MST
//***************************************************************************//

#endif MST_UDP_HPP
