//***************************************************************************//
// HTTPTest Class Implementation
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

//***************************************************************************//
#include "Main.hpp"
#include "HTTPTest.hpp"
//***************************************************************************//

//***************************************************************************//
// MST Technologies Includes
#include <NetworkingMiller.hpp>
#include <MultithreadedMiller.hpp>
//***************************************************************************//

//***************************************************************************//
// System Includes
#include <iostream>
#include <string>
//***************************************************************************//

//***************************************************************************//
using namespace MST;
using namespace std;
//***************************************************************************//

//***************************************************************************//
// Start of HTTPTest implementation
//---------------------------------------------------------------------------//
HTTPTest::HTTPTest()
{
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
HTTPTest::~HTTPTest()
{
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void HTTPTest::Run()
{
  this->DisplayMenu();
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void HTTPTest::DisplayHelp()
{
  /// \todo Implement
  cout << "Help not yet implemented\r\n";
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void HTTPTest::DisplayMenu()
{
  unsigned long ulReturnValue;
  string strInput;

  bool bRun = true;
  //-------------------------------------------------------------------------//
  /// \todo all tests are not yet implemented
  while(bRun)
  {
    cout << "\r\nHTTP Test Menu\r\n"
      << "[1] Start server and continue\r\n"
      << "[H] Help\r\n"
      << "[B] Back\r\n"
      << ">> ";
    cin >> strInput;

    if(strInput == "1")
    {
      cout << "Async Unreliable Echo not yet implemented\r\n";
    }
    else if(strInput == "2")
    {
      this->StartServerAndContinue();
    }
    else if(strInput == "H" || strInput == "h")
    {
      this->DisplayHelp();
    }
    else if(strInput == "B" || strInput == "b")
    {
      ulReturnValue = 0UL;
      bRun = false;
    }
    else
    {
      cout << INVALID_INPUT_STRING;
    }
  } // End of while(bRun)
  //-------------------------------------------------------------------------//

  return;
}
//---------------------------------------------------------------------------//


//---------------------------------------------------------------------------//
void HTTPTest::StartServerAndContinue()
{
  MST::HTTPServer* pNewHTTPServer = new HTTPServer();

  pNewHTTPServer->Accept();




  delete pNewHTTPServer;
}
//---------------------------------------------------------------------------//

// End of HTTPTest implementation
//***************************************************************************//