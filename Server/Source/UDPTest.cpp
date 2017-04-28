//***************************************************************************//
// UDPTest Class Implementation
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

#include "UDPLoss.hpp"

//***************************************************************************//
// System Includes
#include <iostream>
#include <string>
//***************************************************************************//

//***************************************************************************//
// MST Technologies Includes
#include <NetworkingMiller.hpp>
#include <MultithreadedMiller.hpp>
//***************************************************************************//

//***************************************************************************//
// Local Includes
#include "Main.hpp"
#include "UDPTest.hpp"
//***************************************************************************//

//***************************************************************************//
using namespace MST;
using namespace std;
//***************************************************************************//

//***************************************************************************//
// Start of UDPTest implementation
//---------------------------------------------------------------------------//
UDPTest::UDPTest()
{
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
UDPTest::~UDPTest()
{
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void UDPTest::Run()
{
  this->DisplayMenu();
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void UDPTest::DisplayHelp()
{
  /// \todo Implement
  cout << "Help not yet implemented\r\n";
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void UDPTest::DisplayMenu()
{
  unsigned long ulReturnValue;
  string strInput;

  bool bRun = true;
  //-------------------------------------------------------------------------//
  /// \todo all tests are not yet implemented
  while(bRun)
  {
    cout << "\r\nUDP Test Menu\r\n"
      << "[1] Async Unreliable Echo\r\n"
      << "[2] Sync Unreliable Echo\r\n"
      << "[3] Async Reliable Echo\r\n"
      << "[4] Sync Reliable Echo\r\n"
      << "[5] Run Drop Packets Relay\r\n"
      << "[H] Help\r\n"
      << "[B] Back\r\n"
      << ">> ";
    cin >> strInput;

    if(strInput == "1")
    {
      AsyncUnreliableEcho();
    }
    else if(strInput == "2")
    {
      SyncUnreliableEcho();
    }
    else if(strInput == "3")
    {
      AsyncReliableEcho();
    }
    else if(strInput == "4")
    {
      SyncReliableEcho();
    }
    else if(strInput == "5")
    {
      LosePackets();
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
void UDPTest::SyncUnreliableEcho()
{
  unsigned long ulNumberOfTransmissions = 0UL;
  class Packet* pRecvPacket = new Packet();
  UDPServer<class Packet>  oUDPServer;
  oUDPServer.Listen(UDP_SERVER_PORT);
  cout << "UDP Echo Server started on port: " << UDP_SERVER_PORT << ".\r\n";
  
  while(1)
  {
    oUDPServer.WaitOnIncoming();
    oUDPServer.Recv(pRecvPacket);
    pRecvPacket->Destination(pRecvPacket->Source().IP(), pRecvPacket->Source().Port());
    oUDPServer.Send(pRecvPacket);
    ulNumberOfTransmissions++;
    cout << "Echo number: " << ulNumberOfTransmissions << ".\r\n";
  }

  delete pRecvPacket;
  oUDPServer.Close();
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void UDPTest::AsyncUnreliableEcho()
{
  unsigned long ulNumberOfTransmissions = 0UL;
  class Packet* pRecvPacket = new Packet();
  UDPServer<class Packet>  oUDPServer;
  oUDPServer.Listen(UDP_SERVER_PORT);
  cout << "UDP Echo Server started on port: " << UDP_SERVER_PORT << ".\r\n";

  while(1)
  {

    if(oUDPServer.Recv(pRecvPacket) > 0)
    {
      pRecvPacket->Destination(pRecvPacket->Source());
      oUDPServer.Send(pRecvPacket);
      ulNumberOfTransmissions++;
      cout << "Echo number: " << ulNumberOfTransmissions << ".\r\n";
    }

    
  }
  return;
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void UDPTest::SyncReliableEcho()
{
  unsigned long ulNumberOfTransmissions = 0UL;
  class ReliablePacket* pRecvPacket = new ReliablePacket();
  ReliableUDPServer<ReliablePacket> oUDPServer;
  oUDPServer.Listen(UDP_SERVER_PORT);
  cout << "UDP Echo Server started on port: " << UDP_SERVER_PORT << ".\r\n";

  while(1)
  {
    oUDPServer.WaitOnIncoming();
    oUDPServer.Recv(pRecvPacket);
    pRecvPacket->Destination(pRecvPacket->Source().IP(), pRecvPacket->Source().Port());
    oUDPServer.Send(pRecvPacket);
    ulNumberOfTransmissions++;
    cout << "Echo number: " << ulNumberOfTransmissions << ".\r\n";
  }

  delete pRecvPacket;
  oUDPServer.Close();
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void UDPTest::AsyncReliableEcho()
{
  unsigned long ulNumberOfTransmissions = 0UL;
  class ReliablePacket* pRecvPacket = new ReliablePacket();
  ReliableUDPServer<ReliablePacket> oUDPServer;
  oUDPServer.Listen(UDP_SERVER_PORT);
  cout << "UDP Echo Server started on port: " << UDP_SERVER_PORT << ".\r\n";

  while(1)
  {
    oUDPServer.WaitOnIncoming();
    oUDPServer.Recv(pRecvPacket);
    pRecvPacket->Destination(pRecvPacket->Source().IP(), pRecvPacket->Source().Port());
    oUDPServer.Send(pRecvPacket);

    ulNumberOfTransmissions++;
    cout << "Echo number: " << ulNumberOfTransmissions << ".\r\n";
  }
}
//---------------------------------------------------------------------------//


//#include <netinet/in.h>

void UDPTest::LosePackets()

{
  LosePackets2();
}
//---------------------------------------------------------------------------//
// End of UDPTest implementation
//***************************************************************************//