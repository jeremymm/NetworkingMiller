//***************************************************************************//
// TCPTest Class Implementation
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

//***************************************************************************//
#include "Main.hpp"
#include "TCPTest.hpp"
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
// Start of TCPTest implementation
//---------------------------------------------------------------------------//
TCPTest::TCPTest()
{
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
TCPTest::~TCPTest()
{
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void TCPTest::Run()
{
  this->DisplayMenu();
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void TCPTest::DisplayHelp()
{
  /// \todo Implement
  cout << "Help not yet implemented\r\n";
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void TCPTest::DisplayMenu()
{
  unsigned long ulReturnValue;
  string strInput;

  bool bRun = true;
  //-------------------------------------------------------------------------//
  /// \todo all tests are not yet implemented
  while(bRun)
  {
    cout << "\r\nTCP Test Menu\r\n"
      << "[1] Async Echo\r\n"
      << "[2] Sync Echo\r\n"
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
      SyncEcho();
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
void TCPTest::SyncEcho()
{
  unsigned long ulNumberOfTransmissions = 0UL;
  class Packet* pRecvPacket = new Packet();
  TCPServer oTCPServer;
  oTCPServer.Listen(TCP_SERVER_PORT);
  cout << "TCP Echo Server started on port: " << TCP_SERVER_PORT << ".\r\n";
  cout << "Waiting for connection...\r\n";

  Socket<class Packet>* oSocket = oTCPServer.Accept();
  
  while(ulNumberOfTransmissions < 100000 && oSocket->Recv(pRecvPacket))
  {
    pRecvPacket->Destination(pRecvPacket->Source().IP(), pRecvPacket->Source().Port());
    oSocket->Send(*pRecvPacket);
    ulNumberOfTransmissions++;
    cout << "Echo number: " << ulNumberOfTransmissions << ".\r\n";
  }

  delete pRecvPacket;

  oSocket->Close();
  oTCPServer.Close();

}
//---------------------------------------------------------------------------//

// End of TCPTest implementation
//***************************************************************************//