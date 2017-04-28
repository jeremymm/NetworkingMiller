//***************************************************************************//
// Main() Implementation for NetworkingMiller Test Server
//
// Created: Mar 15, 2006
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
// System Includes
#include <iostream>
#include <string>
#include <sstream>
#include <signal.h>
#include <time.h>
//***************************************************************************//

//***************************************************************************//
// Local Includes
#include "Main.hpp"
#include "UDPTest.hpp"
#include "TCPTest.hpp"
#include "RTNTest.hpp"
#include "HTTPTest.hpp"
//***************************************************************************//

//***************************************************************************//
using namespace std;
//***************************************************************************//

//***************************************************************************//
/// This global bool will be used to signal to running tests if they 
/// should continue to run or not
bool gbTesting = true;
/// This global function is used to stop test. It is the signal 
/// handler for SIGINT (ctrl-c) signal.
void StopTests(int parameters)
{
  gbTesting = false;
}

char* gcpServerIP;
char* gcpServerPort;
unsigned short gusServerPort;

char* cpDefaultIP = "127.0.0.1";
char* cpDefaultPort = "12345";
//***************************************************************************//

//***************************************************************************//
int main(int argc, char** argv)
{
  //------------------------------------------------------------------------//
  cout << HEADER_STRING << " Version " << APP_VERSION << endl;
  cout << CONTACT_STRING;
  //------------------------------------------------------------------------//

  // Setup signal handler
  signal(SIGINT,StopTests);

  srand((unsigned int)time(NULL));

  if(argc >= 1)
    gcpServerIP = argv[1];
  else
    gcpServerIP = cpDefaultIP;

  if(argc >= 2)
    gcpServerPort = argv[2];
  else
    gcpServerPort = cpDefaultPort;

  stringstream sstmtemp;
  sstmtemp.str(gcpServerPort);
  sstmtemp >> gusServerPort;

  bool bRun = true;
  string strInput;
  class Test* pTest = NULL;

  //-------------------------------------------------------------------------//
  while(bRun)
  {
    strInput = "";
    cout << "\r\nMain Menu\r\n"
         << "[1] UDP Tests\r\n"
         << "[2] TCP Tests\r\n"
         << "[3] RTN Tests\r\n"
         << "[4] Bandwidth Tests\r\n"
         << "[5] Latency Tests\r\n"
         << "[6] Run HTTP Server\r\n"
         << "[H] Help\r\n"
         << "[Q] Quit\r\n"
         << ">> ";
    cin >> strInput;
    if(strInput == "1")
    {
      pTest = new UDPTest();
      pTest->Run();
    }
    else if(strInput == "2")
    {
      pTest = new TCPTest();
      pTest->Run();
    }
    else if(strInput == "3")
    {
      pTest = new RTNTest();
      pTest->Run();
    }
    else if(strInput == "4")
    {
      cout << "Bandwidth Tests not yet implemented\r\n";
    }
    else if(strInput == "5")
    {
      cout << "Bandwidth Tests not yet implemented\r\n";
    }
    else if(strInput == "6")
    {
      cout << "Bandwidth Tests not yet implemented\r\n";
    }
    else if(strInput == "H" || strInput == "h")
    {
      pTest = new HTTPTest();
      pTest->Run();
    }
    else if(strInput == "Q" || strInput == "q")
    {
      bRun = false;
    }
    else
    {
      cout << INVALID_INPUT_STRING;
    }

    if(pTest != NULL)
    {
      delete pTest;
      pTest = NULL;
    }

  } // End of while(bRun)
  //-------------------------------------------------------------------------//

  cout << "Thanks for testing, You are awesome\r\n";

  return 0;
} // End of int main(int argc, char** argv)
//***************************************************************************//
