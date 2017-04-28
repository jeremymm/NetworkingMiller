//***************************************************************************//
// TCPTest Class Interface
// 
// This class for network server based echo tests
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

#ifndef CLIENT_TCP_TEST
#define CLIENT_TCP_TEST

//***************************************************************************//
// Local Includes
#include "Test.hpp" // base class
//***************************************************************************//

//***************************************************************************//
const unsigned short TCP_SERVER_PORT = 12346;
//***************************************************************************//

//***************************************************************************//
class TCPTest : public Test
{
private:
  void DisplayHelp();
  void DisplayMenu();

  void SyncEcho();
public:
  TCPTest();
  virtual ~TCPTest();

  void Run();
};
//***************************************************************************//

#endif CLIENT_TCP_TEST