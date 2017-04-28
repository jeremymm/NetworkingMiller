//***************************************************************************//
// TCPClient Class Interface
//
// Created: Feb 21, 2006
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

#ifndef MST_TCP_CLIENT_HPP
#define MST_TCP_CLIENT_HPP

//***************************************************************************//
// System Includes
#include <string>
//***************************************************************************//

//***************************************************************************//
// Local Includes
#include "TCP.hpp"
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //*************************************************************************//
  class TCPClient : public TCP
  {
  private:
    //***********************************************************************//
    /// This bool is set true when this object has completed a connection 
    /// once this connection is severed or closes this bool is set to false.
    bool mbConnected;
    //***********************************************************************//
  protected:
  public:
    //***********************************************************************//
    //-----------------------------------------------------------------------//
    TCPClient();
    virtual ~TCPClient();
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    // The below methods takes a port and host name in a char* and 
    long _cdecl Connect(const class EndPoint& rEndPoint, bool bBlock = true);
    // attempts to connect to that remote host.
    // Parameter 1: Port you wish to connect to in host order (little endian for x86).
    // Parameter 2: The host name or numerical address of the remote host. The char*
    //              must be null terminated.
    // Parameter 3: Set this bool to false if you do not want to block on this method
    // Return Value: This method will return 1 on success and 0 on failure.
    virtual long _cdecl Connect(unsigned short usPort, const char* cpHostName,   bool bBlock = true);
    // The below methods takes a port and host name in a std::string and 
    // attempts to connect to that remote host.
    // Parameter 1: Port you wish to connect to in host order (little endian for x86).
    // Parameter 2: The host name or numerical address of the remote host.
    // Parameter 3: Set this bool to false if you do not want to block on this method.
    // Return Value: This method will return 1 on success and 0 on failure.
    virtual long _cdecl Connect(unsigned short usPort, const std::string& strHostName, bool bBlock = true);
    // The below methods takes a port and address in a unsigned long and 
    // attempts to connect to that remote host. The unsigned long IPv4 address.
    // must be in host order (little endian for x86).
    // Parameter 1: Port you wish to connect to in host order.
    // Parameter 2: The IPv4 Address of the remote host.
    // Parameter 3: Set this bool to false if you do not want to block on this method.
    // Return Value: This method will return 1 on success and 0 on failure.
    virtual long _cdecl Connect(unsigned short usPort, unsigned long ulIP, bool bBlock = true);
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    // This method will return the state of this objects connection.
    // Return Value: This method will return true if this object has established 
    //               a connection, otherwise it will return false.
    bool _cdecl Connected() const {return mbConnected;}
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    // This method will disconnect and close the connection to and remote host
    // if such a connection exists.
    void _cdecl Disconnect();
    //-----------------------------------------------------------------------//
    //***********************************************************************//
  }; // End of class TCPClient : public TCP
  //*************************************************************************//
} // End of namespace MST 
//***************************************************************************//

#endif MST_TCP_CLIENT_HPP
