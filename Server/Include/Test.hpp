//***************************************************************************//
// Test Class Interface
// 
// This class for network server based tests
//
// Created: June 17, 2008
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

#ifndef SERVER_TEST_HPP
#define SERVER_TEST_HPP

//***************************************************************************//
class Test
{
private:
public:
  Test();
  virtual ~Test();

  virtual void Run() = 0;
}; // End of class Test
//***************************************************************************//

#endif SERVER_TEST_HPP