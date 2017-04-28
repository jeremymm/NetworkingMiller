//***************************************************************************//
// UDPTest Class Interface
// 
// This class is for network client based echo tests
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

#ifndef CLIENT_UDP_TEST
#define CLIENT_UDP_TEST

//***************************************************************************//
// Local Includes
#include "Test.hpp" // base class
//***************************************************************************//

//***************************************************************************//
const unsigned short UDP_SERVER_PORT = 12345;
//***************************************************************************//

//***************************************************************************//
class UDPTest : public Test
{
private:
  void DisplayHelp();
  void DisplayMenu();

  void SyncUnreliableAutoEcho();
  void AsyncUnreliableAutoEcho();

  void SyncReliableAutoEcho();
  void AsyncReliableAutoEcho();

  void SyncUnreliableManualEcho();
  void SyncReliableManualEcho();

  void ThrottlerTest();
public:
  UDPTest();
  ~UDPTest();

  void Run();
};
//***************************************************************************//

#endif CLIENT_UDP_TEST
