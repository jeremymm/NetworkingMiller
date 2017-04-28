//***************************************************************************//
// TCP Class Interface
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

#ifndef MST_TCP_HPP
#define MST_TCP_HPP

//***************************************************************************//
// System Includes
#include <string>
//***************************************************************************//

//***************************************************************************//
#include "Socket.hpp"
#include "Packet.hpp"
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //*************************************************************************//
  class TCP : public Socket<class Packet>
  {
  private:
    //***********************************************************************//
    //***********************************************************************//
  protected:
    //***********************************************************************//
    unsigned long CreateDescriptor();
    //***********************************************************************//
  public:
    //***********************************************************************//
    //-----------------------------------------------------------------------//
    // Constructor
    // Simply creates a socket to be used with the TCP transport protocol.
    TCP();
    // Destructor
    virtual ~TCP();
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    /// The below method will attempt to send a file to a connected socket.
    /// NOTE: The maximum file size that can be sent is 2GB.
    /// \param [in] cpFileName 
    /// The name of the file you wish to send in a const char*
    /// \param [in] pSocket
    /// The socket the the file will be sent to.  This is useful if 
    /// you are running a server and are managing more than one 
    /// connection.  Client class should leave this parameter set to null.
    /// \return
    /// This method will return the number of bytes it sent.
    virtual unsigned long _cdecl SendFile(const char* const cpFileName,   class Socket* const pSocket = NULL);
    /// The below method will attempt to send a file to a connected socket.
    /// NOTE The maximum file size that can be sent is 2GB.
    /// \param [in] rstrFileName
    /// The name of the file you wish to send in a std::string.
    /// \param [in] pSocket
    /// The socket the the file will be sent to.  This is useful if 
    /// you are running a server and are managing more than one 
    /// connection.  Client class should leave this parameter set to null.
    /// Return Value: This method will return the number of bytes it sent
    virtual unsigned long _cdecl SendFile(const std::string& rstrFileName, class Socket* const pSocket = NULL);

    /// The below method will attempt to recv a file from a connected socket.
    /// NOTE: The maximum file size that can be received is 2GB.
    /// \param [in] cpFileName
    /// The name of the file you wish to have the data received from the network 
    /// stored to in a const char*.
    /// \param [in] pSocket
    /// The socket the the file will be received from.  This is useful if 
    /// you are running a server and are managing more than one 
    /// connection.  Client classes should leave this parameter set to null.
    /// \return
    /// This method will return the number of bytes it received from the network.
    virtual unsigned long _cdecl RecvFile(const char* const cpFileName, class Socket* const pSocket = NULL);
    /// The below method will attempt to recv a file from a connected socket.
    /// NOTE: The maximum file size that can be received is 2GB.
    /// \param [in] strFileName
    /// The name of the file you wish to have the data received from the network. 
    /// stored to in a std::string.
    /// \param [in] pSocket The socket the the file will be received from.  
    /// This is useful if you are running a server and are managing more than one 
    /// connection.  Client classes should leave this parameter set to null.
    /// \return
    /// This method will return the number of bytes it received from the network.
    virtual unsigned long _cdecl RecvFile(const std::string& strFileName, class Socket* const pSocket = NULL);
    //-----------------------------------------------------------------------//
    //***********************************************************************//
  }; // End of class TCP : public Socket<class Packet>
  //*************************************************************************//
} // End of namespace MST 
//***************************************************************************//

#endif MST_TCP_HPP
