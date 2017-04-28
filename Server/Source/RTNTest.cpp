//***************************************************************************//
// RTNTest Class Implementation
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
// System Includes
#include <iostream>
#include <string>
//***************************************************************************//

//***************************************************************************//
// MST Technologies Includes
#include <NetworkingMiller.hpp>
//***************************************************************************//

//***************************************************************************//
// Local Includes
#include "Main.hpp"
#include "RTNTest.hpp"
//***************************************************************************//

//***************************************************************************//
using namespace MST;
using namespace std;
//***************************************************************************//

//***************************************************************************//
// Start of UDPTest implementation
//---------------------------------------------------------------------------//
RTNTest::RTNTest()
{
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
RTNTest::~RTNTest()
{
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void RTNTest::Run()
{
  this->DisplayMenu();
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void RTNTest::DisplayHelp()
{
  /// \todo Implement
  cout << "Help not yet implemented\r\n";
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void RTNTest::DisplayMenu()
{
  unsigned long ulReturnValue;
  string strInput;

  bool bRun = true;
  //-------------------------------------------------------------------------//
  /// \todo all tests are not yet implemented
  while(bRun)
  {
    cout << "\r\nRTN Test Menu\r\n"
      << "[1] Run Trans\r\n"
      << "[H] Help\r\n"
      << "[B] Back\r\n"
      << ">> ";
    cin >> strInput;

    if(strInput == "1")
    {
      RunTrans();
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
void RTNTest::RunTrans()
{
  RTNTransceiver* pTrans = new RTNTransceiver();
  RTNPacket* pPacket = new RTNPacket();
  pTrans->Listen(gusServerPort);

  unsigned long ulKiloBitsPerSecond = 0UL;
  cout << "Enter Throttled Rate (kbits/sec): ";
  cin >> ulKiloBitsPerSecond;

  pTrans->SetSendingRate(ulKiloBitsPerSecond);
  

  cout << "Started RTN Trans on " << gusServerPort << endl;
  while(1)
  {
    pTrans->WaitOnIncoming();
    pTrans->Recv(pPacket);
    if(pPacket->IsFromActor())
    {
      cout << "Forwarding ID: " << pPacket->ID() << " X: " << pPacket->X() << 
        " Y: " << pPacket->Y() << " Z: " << pPacket->Z() << endl;
      pTrans->Forward(pPacket);
    }
  }

  delete pTrans;
  delete pPacket;
}
//---------------------------------------------------------------------------//
//***************************************************************************//