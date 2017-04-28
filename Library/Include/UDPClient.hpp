//***************************************************************************//
// UDP Client Class Interface
//
// Created: Mar 24, 2006
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

#ifndef MST_UDP_CLIENT_HPP
#define MST_UDP_CLIENT_HPP

//***************************************************************************//
#include "UDP.hpp"
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //*************************************************************************//
  template <class T>
  class UDPClient : public UDP<T>
  {
  private:
  protected:
  public:
    //***********************************************************************//
    //-----------------------------------------------------------------------//
    // Constructor
    UDPClient();
    // Destructor
    virtual ~UDPClient();
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
  }; // End of class UDPClient : public UDP
  //*************************************************************************//
} // End of namespace MST
//***************************************************************************//

#endif MST_UDP_CLIENT_HPP
