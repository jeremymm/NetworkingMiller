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
      cout << "Async Echo not yet implemented\r\n";
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
  const unsigned long MAX_NUMBER_OF_TRANSMISSIONS = 100000UL;
  unsigned long ulNumberOfTransmissions = 0UL;
  unsigned long ulErrorTransmission = 0UL;
  class Packet* pSendPacket = new Packet();
  class Packet* pRecvPacket = new Packet();
  TCPClient oTCPClient;
  oTCPClient.Connect(gusServerPort, gcpServerIP);
  unsigned long ulPacketSize = pSendPacket->PayloadMaxSize();
  char* cpRandom = new char[ulPacketSize];
  while(ulNumberOfTransmissions < MAX_NUMBER_OF_TRANSMISSIONS)
  {
    for(unsigned long i = 0; i < ulPacketSize; i++)
    {
      cpRandom[i] = rand() % 255;
    }
    pSendPacket->SetPayload(cpRandom, ulPacketSize);

    oTCPClient.Send(*pSendPacket);
    oTCPClient.Recv(pRecvPacket);
    int iCompareResult = memcmp(pSendPacket->Payload(), pRecvPacket->Payload(), ulPacketSize);
    if(iCompareResult != 0)
    {
      cout << "****ERROR**** Echo error number: " << ulErrorTransmission << ".\r\n";
      ulErrorTransmission++;
    }
    ulNumberOfTransmissions++;
  }

  delete [] cpRandom;
  delete pSendPacket;
  delete pRecvPacket;

  cout << "Echoed " << 100.0F - (((float)ulErrorTransmission / (float)ulNumberOfTransmissions) * 100.0F) << "% Packets successfully.\r\n";
  oTCPClient.Close();
}
//---------------------------------------------------------------------------//

// End of TCPTest implementation
//***************************************************************************//