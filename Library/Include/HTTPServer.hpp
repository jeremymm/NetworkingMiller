//***************************************************************************//
// HTTPServer Class Interface
//
// Created: July 18, 2012
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

#ifndef MST_HTTP_SERVER_HPP
#define MST_HTTP_SERVER_HPP

//***************************************************************************//
// Local Includes
#include "HTTP.hpp"
#include "TCPServer.hpp"
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  class HTTPServer : public TCPServer, public HTTP
  {
  private:
    //-----------------------------------------------------------------------//
    HTTPServer(const HTTPServer&); // Disable copying

    std::wstring mwstrDocumentRoot;
    //-----------------------------------------------------------------------//
  public:
    //-----------------------------------------------------------------------//
    HTTPServer();
    ~HTTPServer();

    bool SetDocumentRoot();
    //-----------------------------------------------------------------------//
  };
}
//***************************************************************************//

#endif