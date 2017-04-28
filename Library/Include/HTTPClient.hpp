//***************************************************************************//
// HTTPClient Class Interface
//
// Created: Mar 25, 2006
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

#ifndef MST_HTTP_CLIENT_HPP
#define MST_HTTP_CLIENT_HPP

//***************************************************************************//
// System Includes
#include <vector>
//***************************************************************************//

//***************************************************************************//
// Local Includes
#include "HTTP.hpp"
#include "TCPClient.hpp"
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //*************************************************************************//
  class HTTPClient : public TCPClient, public HTTP
  {
  private:
    //-----------------------------------------------------------------------//
    // This string is used to store the host name of the server we are 
    // communicating with.
    std::string mstrHost;
    //-----------------------------------------------------------------------//
  protected:
  public:
    //***********************************************************************//
    //-----------------------------------------------------------------------//
    // Constructor
    HTTPClient();
    // Destructor
    virtual ~HTTPClient();
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    // The below method accepts a host name to an HTTP server and will attempt 
    // to connect to that server.
    // Parameter 1: A std::string that holds the host name of the HTTP server.
    // Parameter 2: Default port number to most HTTP servers is 80.
    // Parameter 3: Set this bool to false if you do not want to block on this method
    // Return Value: If the connection was established the return value will be one
    //               if the connection failed the return value will be zero.
    long _cdecl Connect(const std::string& strHostName, unsigned short usPort = 80, bool bBlock = true);
    // The below method accepts a host name to an HTTP server and will attempt 
    // to connect to that server.
    // Parameter 1: A null terminated char* that holds the host name of the HTTP server.
    // Parameter 2: Default port number to most HTTP servers is 80.
    // Parameter 3: Set this bool to false if you do not want to block on this method
    // Return Value: If the connection succeeded the return value will be one
    //               if the connection failed the return value will be zero.
    long _cdecl Connect(const char* cpHostName, unsigned short usPort = 80, bool bBlock = true);
    // The below method accepts a host name to an HTTP server and will attempt 
    // to connect to that server.
    // Parameter 1: Is a network order IPv4 address in an unsigned long that holds 
    //              the IPv4 address of the HTTP server.
    // Parameter 2: Default port number to most HTTP servers is 80.
    // Parameter 3: Set this bool to false if you do not want to block on this method
    // Return Value: If the connection succeeded the return value will be one
    //               if the connection failed the return value will be zero.
    long _cdecl Connect(unsigned long ulIP, unsigned short usPort = 80, bool bBlock = true);
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    // The below method will close the connection to the HTTP server it is
    // connected to if such a connection exists.
    void _cdecl Disconnect();
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    // The below method attempts to get a file from a HTTP server that 
    // this object is connected to. 
    // Parameter 1: A std::string that holds the path and the filename of the
    //              the file this method will try to retrieve.
    // Parameter 2: A std::string that holds the name and path this method 
    //              will save the the received file in.
    // Return Value: The method will return true on success, otherwise it will
    //               it will return false.
    bool _cdecl Get(const std::string& rstrPath, const std::string& rstrFileName, const std::string& rstrLocalFileName);
    // The below method attempts to get a file from a HTTP server that 
    // this object is connected to. 
    // Parameter 1: A const char* that holds the path and the filename of the
    //              the file this method will try to retrieve.
    // Parameter 2: A const char* that holds the name and path this method 
    //              will save the the received file in.
    // Return Value: The method will return true on success, otherwise it will
    //               it will return false.
    bool _cdecl Get(const char* cpPath, const char* cpFileName, const char* cpLocalFileName);
    // The below method will attempt to get a port of the file requested from
    // the HTTP server.
    // Parameter 1: A std::string that holds the path and the filename of the
    //              the file this method will try to retrieve.
    // Parameter 2: A unsigned long that denotes the offset in bytes to start
    //              reading from the HTTP server.
    // Parameter 3: Is the amount of data that this method will attempt to read
    //              from position defined in Parameter 2.
    // Parameter 4: A std::string that will hold the data read from the file.
    // Return Value: The return value will be true when this method succeeds
    //               otherwise it will be false.
    bool _cdecl Get(const std::string& rstrPath, const std::string& rstrFileName, unsigned long ulOffset, unsigned long ulLength, std::string& strData); 
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    // The below method will send data to a HTTP server via a POST message
    // Parameter 1: Names of the data values in ASCII.
    // Parameter 2: Values in ASCII.
    // Parameter 3: Is the path and file name that the data values will 
    //              passed to. Example /cgi/parse.php
    // Return Value: The method will return true if the data was send successfully 
    //               otherwise it will return false.
    enum HTTPStatusCode _cdecl Post(std::vector<std::string> &vNames, std::vector<std::string> &vValues, std::string &strPathAndFileName);
    //-----------------------------------------------------------------------//
    //***********************************************************************//
  }; // End of class HTTPClient : public TCPClient, public HTTP
  //*************************************************************************//
} // End of namespace MST
//***************************************************************************//

#endif MST_HTTP_CLIENT_HPP
