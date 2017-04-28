//***************************************************************************//
// HTTP Class Interface
// 
// This class is to facilitate testing NetworkingMiller's HTTPServer class
//
// Created: July 20, 2012
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

#ifndef SERVER_HTTP_TEST
#define SERVER_HTTP_TEST

//***************************************************************************//
// Local Includes
#include "Test.hpp" // base class
//***************************************************************************//

//***************************************************************************//
const unsigned short HTTP_PORT = 8181;
//***************************************************************************//

//***************************************************************************//
class HTTPTest : public Test
{
private:
  void DisplayHelp();
  void DisplayMenu();

  void StartServerAndContinue();

public:
  HTTPTest();
  virtual ~HTTPTest();

  void Run();
};
//***************************************************************************//

#endif SERVER_HTTP_TEST