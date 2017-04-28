
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

//***************************************************************************//
#include "Main.hpp"
#include "RTNTest.hpp"
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
#include <cmath>
//***************************************************************************//

//***************************************************************************//
using namespace MST;
using namespace std;
//***************************************************************************//

#define NUMBER_OF_SENDERS 6
//***************************************************************************//
// Start of RTNTest implementation
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
      << "[1] Run Senders\r\n"
      << "[B] Back\r\n"
      << ">> ";
    cin >> strInput;

    if(strInput == "1")
    {
      RunSenders();
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
void RTNTest::RunSenders()
{
  std::vector<class RTNSubscriber*> vSubcribers;
  std::vector<float> vRadius;
  std::vector<float> vAngVel;
  std::vector<float> vAngles;

  class RTNPacket* pPacket = new RTNPacket();
  pPacket->SetPayload("HELLO", sizeof("HELLO"));

  for(unsigned long ulIndex = 0UL; ulIndex < NUMBER_OF_SENDERS; ulIndex++)
  {
    vSubcribers.push_back(new RTNSubscriber());
  }

  // Connect
  for(unsigned long ulIndex = 0UL; ulIndex < NUMBER_OF_SENDERS; ulIndex++)
  {
    vSubcribers[ulIndex]->Connect(gusServerPort, gcpServerIP);
    vSubcribers[ulIndex]->SetAsActor(true);
    vSubcribers[ulIndex]->ID(ulIndex);
  }

  const float fRange = 1000.0F;
  const float fVelocity = 1.0F;

  // calculate radii
  for(unsigned long ulIndex = 0UL; ulIndex < NUMBER_OF_SENDERS; ulIndex++)
  {
    vRadius.push_back(fRange * (ulIndex + 1) / NUMBER_OF_SENDERS);
    vSubcribers[ulIndex]->X(vRadius[ulIndex]);
    vAngVel.push_back(fVelocity * (ulIndex + 1) / NUMBER_OF_SENDERS);
    vAngles.push_back(0.0F);
  }

  //vRadius[0] = 1000.0F;
  //vRadius[1] = 1000.0F;

  //vAngles[0] = 0.0F;
  //vAngles[1] = 3.14F;

  class Timer* pTimer = new Timer();
  
  while(1)
  {
    double dDT = pTimer->DT(TIMER_1);
    for(unsigned long ulIndex = 0UL; ulIndex < NUMBER_OF_SENDERS; ulIndex++)
    {
      vAngles[ulIndex] = vAngles[ulIndex] + (vAngVel[ulIndex] * (float)dDT);

      vSubcribers[ulIndex]->Y(sin(vAngles[ulIndex]) * vRadius[ulIndex]);
      vSubcribers[ulIndex]->X(cos(vAngles[ulIndex]) * vRadius[ulIndex]);

      pPacket->ID(ulIndex);
      pPacket->MarkFromActor();

      cout << "Sending ID: " << pPacket->ID() << " X: " << pPacket->X() << 
        " Y: " << pPacket->Y() << " Z: " << pPacket->Z() << endl;
      vSubcribers[ulIndex]->Send(pPacket);
    }
  }
  delete pPacket;
  delete pTimer;

  for(unsigned long ulIndex = 0UL; ulIndex < NUMBER_OF_SENDERS; ulIndex++)
  {
    delete vSubcribers[ulIndex];
  }

}
//---------------------------------------------------------------------------//

// End of RTNTest implementation
//***************************************************************************//