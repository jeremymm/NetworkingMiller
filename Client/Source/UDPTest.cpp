
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
#include "UDPTest.hpp"
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

const unsigned long MAX_NUMBER_OF_TRANSMISSIONS = 1000000UL;

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
      << "[1] Async Unreliable Auto Echo\r\n"
      << "[2] Sync Unreliable Auto Echo\r\n"
      << "[3] Async Reliable Auto Echo\r\n"
      << "[4] Sync Reliable Auto Echo\r\n"
      << "[5] Throttler Test\r\n"
      << "[H] Help\r\n"
      << "[B] Back\r\n"
      << ">> ";
    cin >> strInput;

    if(strInput == "1")
    {
      AsyncUnreliableAutoEcho();
    }
    else if(strInput == "2")
    {
      SyncUnreliableAutoEcho();
    }
    else if(strInput == "3")
    {
      AsyncReliableAutoEcho();
    }
    else if(strInput == "4")
    {
      SyncReliableAutoEcho();
    }
    else if(strInput == "5")
    {
      ThrottlerTest();
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
void UDPTest::SyncUnreliableAutoEcho()
{
  unsigned long ulNumberOfTransmissions = 0UL;
  unsigned long ulErrorTransmission = 0UL;
  class Packet* pSendPacket = new Packet();
  class Packet* pRecvPacket = new Packet();
  UDPClient<class Packet> oUDPClient;
  oUDPClient.Connect(gusServerPort, gcpServerIP);
  unsigned long ulPacketSize = pSendPacket->PayloadMaxSize();
  char* cpRandom = new char[ulPacketSize];
  while(ulNumberOfTransmissions < MAX_NUMBER_OF_TRANSMISSIONS)
  {

    for(unsigned long i = 0; i < ulPacketSize; i++)
    {
      cpRandom[i] = rand() % 255;
    }
    pSendPacket->SetPayload(cpRandom, ulPacketSize);

    oUDPClient.Send(pSendPacket);
    oUDPClient.WaitOnIncoming();
    oUDPClient.Recv(pRecvPacket);
    int iCompareResult = memcmp(pSendPacket->Payload(), pRecvPacket->Payload(), ulPacketSize);
    if(iCompareResult != 0)
    {
      cout << "****ERROR**** Echo error number: " << ulErrorTransmission << ".\r\n";
      ulErrorTransmission++;
    }
    else
      cout << "Successful Echo\r\n";

    ulNumberOfTransmissions++;
  }

  delete [] cpRandom;
  delete pSendPacket;
  delete pRecvPacket;

  cout << "Echoed " << 100.0F - (((float)ulErrorTransmission / (float)ulNumberOfTransmissions) * 100.0F) << "% Packets successfully.\r\n";
  oUDPClient.Close();
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void UDPTest::AsyncUnreliableAutoEcho()
{
  std::deque<class Packet> vOutStandingPacket;

  const unsigned long MAX_NUMBER_OF_OUTSTANDING_Packet = 512;
  unsigned long ulNumberOfTransmissions = 0UL;
  unsigned long ulErrorTransmission = 0UL;
  class Packet* pSendPacket = new Packet();
  class Packet* pRecvPacket = new Packet();
  UDPClient<class Packet> oUDPClient;
  oUDPClient.Connect(gusServerPort, gcpServerIP);
  unsigned long ulPacketSize = pSendPacket->PayloadMaxSize();
  char* cpRandom = new char[ulPacketSize];
  while(ulNumberOfTransmissions < MAX_NUMBER_OF_TRANSMISSIONS)
  {
    for(unsigned long i = 0; i < ulPacketSize; i++)
    {
      cpRandom[i] = rand() % 255;
      //cpRandom[i] = i % 16;
    }
    pSendPacket->SetPayload(cpRandom, ulPacketSize);

    oUDPClient.Send(pSendPacket);
    if(vOutStandingPacket.size() > MAX_NUMBER_OF_OUTSTANDING_Packet)
      vOutStandingPacket.pop_front();

    vOutStandingPacket.push_back(*pSendPacket);

    if(oUDPClient.Recv(pRecvPacket) > 0)
    {
      deque<class Packet>::iterator itrOutstandingVector;
      itrOutstandingVector = vOutStandingPacket.begin();
      bool bFound = false;
      while(!bFound && (itrOutstandingVector != vOutStandingPacket.end()))
      {
        int iCompareResult = memcmp(itrOutstandingVector->Payload(), pRecvPacket->Payload(), ulPacketSize);
        if(iCompareResult == 0)
        {
          bFound = true;
          vOutStandingPacket.erase(itrOutstandingVector);
          cout << "Successful Echo.\r\n";
        }
        else
          itrOutstandingVector++;
      }
      if(!bFound)
      {
        cout << "****ERROR**** Echo error number: " << ulErrorTransmission << ".\r\n";
        ulErrorTransmission++;
      }
    }

    ulNumberOfTransmissions++;
  }

  delete [] cpRandom;
  delete pSendPacket;
  delete pRecvPacket;

  cout << "Echoed " << 100.0F - (((float)ulErrorTransmission / (float)ulNumberOfTransmissions) * 100.0F) << "% Packets successfully.\r\n";
  oUDPClient.Close();
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void UDPTest::SyncReliableAutoEcho()
{
  unsigned long ulNumberOfTransmissions = 0UL;
  unsigned long ulErrorTransmission = 0UL;
  class ReliablePacket* pSendPacket = new ReliablePacket();
  class ReliablePacket* pRecvPacket = new ReliablePacket();
  ReliableUDPClient<ReliablePacket> oUDPClient;
  oUDPClient.Connect(gusServerPort, gcpServerIP);
  unsigned long ulPacketSize = pSendPacket->PayloadMaxSize();
  char* cpRandom = new char[ulPacketSize];
  while(ulNumberOfTransmissions < MAX_NUMBER_OF_TRANSMISSIONS)
  {

    for(unsigned long i = 0; i < ulPacketSize; i++)
    {
      cpRandom[i] = rand() % 255;
      cpRandom[i] = i % 0xFF;
    }
    pSendPacket->SetPayload(cpRandom, ulPacketSize);

    oUDPClient.Send(pSendPacket);
    oUDPClient.WaitOnIncoming();
    oUDPClient.Recv(pRecvPacket);
    int iCompareResult = memcmp(pSendPacket->Payload(), pRecvPacket->Payload(), ulPacketSize);
    if(iCompareResult != 0)
    {
      cout << "****ERROR**** Echo error number: " << ulErrorTransmission << ".\r\n";
      ulErrorTransmission++;
    }
    else
      cout << "Successful echo: " << ulNumberOfTransmissions << endl;
    ulNumberOfTransmissions++;
  }

  delete [] cpRandom;
  delete pSendPacket;
  delete pRecvPacket;

  cout << "Echoed " << 100.0F - (((float)ulErrorTransmission / (float)ulNumberOfTransmissions) * 100.0F) << "% Packets successfully.\r\n";
  oUDPClient.Close();
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void UDPTest::AsyncReliableAutoEcho()
{
  unsigned long ulNumberOfTransmissions = 0UL;
  unsigned long ulErrorTransmission = 0UL;
  class ReliablePacket* pSendPacket = new ReliablePacket();
  class ReliablePacket* pRecvPacket = new ReliablePacket();
  ReliableUDPClient<ReliablePacket> oUDPClient;
  oUDPClient.Connect(gusServerPort, gcpServerIP);
  unsigned long ulPacketSize = pSendPacket->PayloadMaxSize();

  unsigned long ulKiloBitsPerSecond = 0UL;
  cout << "Enter Throttled Rate (kbits/sec): ";
  cin >> ulKiloBitsPerSecond;

  oUDPClient.SetSendingRate(ulKiloBitsPerSecond);


  char* cpRandom = new char[ulPacketSize];
  while(ulNumberOfTransmissions < MAX_NUMBER_OF_TRANSMISSIONS)
  {

    for(unsigned long i = 0; i < ulPacketSize; i++)
    {
      cpRandom[i] = rand() % 255;
    }
    pSendPacket->SetPayload(cpRandom, ulPacketSize);

    oUDPClient.Send(pSendPacket);
    //oUDPClient.WaitOnIncoming();
    oUDPClient.Recv(pRecvPacket);
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
  oUDPClient.Close();
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void UDPTest::SyncUnreliableManualEcho()
{
  unsigned long ulNumberOfTransmissions = 0UL;
  unsigned long ulErrorTransmission = 0UL;
  class Packet* pSendPacket = new Packet();
  class Packet* pRecvPacket = new Packet();
  UDPClient<class Packet> oUDPClient;
  oUDPClient.Connect(gusServerPort, gcpServerIP);
  unsigned long ulPacketSize = pSendPacket->PayloadMaxSize();
  char* cpRandom = new char[ulPacketSize];
  while(ulNumberOfTransmissions < MAX_NUMBER_OF_TRANSMISSIONS)
  {
    for(unsigned long i = 0; i < ulPacketSize; i++)
    {
      cpRandom[i] = rand() % 255;
    }
    pSendPacket->SetPayload(cpRandom, ulPacketSize);

    oUDPClient.Send(pSendPacket);
    oUDPClient.WaitOnIncoming();
    oUDPClient.Recv(pRecvPacket);
    int iCompareResult = memcmp(pSendPacket->Payload(), pRecvPacket->Payload(), ulPacketSize);
    if(iCompareResult != 0)
    {
      cout << "****ERROR**** Echo error number: " << ulErrorTransmission << ".\r\n";
      ulErrorTransmission++;
    }
    else
      cout << "Successful Echo\r\n";

    ulNumberOfTransmissions++;
  }

  delete [] cpRandom;
  delete pSendPacket;
  delete pRecvPacket;

  cout << "Echoed " << 100.0F - (((float)ulErrorTransmission / (float)ulNumberOfTransmissions) * 100.0F) << "% Packets successfully.\r\n";
  oUDPClient.Close();
}
//---------------------------------------------------------------------------//


//---------------------------------------------------------------------------//
void UDPTest::SyncReliableManualEcho()
{
  unsigned long ulNumberOfTransmissions = 0UL;
  unsigned long ulErrorTransmission = 0UL;
  class ReliablePacket* pSendPacket = new ReliablePacket();
  class ReliablePacket* pRecvPacket = new ReliablePacket();
  ReliableUDPClient<ReliablePacket> oUDPClient;
  oUDPClient.Connect(gusServerPort, gcpServerIP);
  unsigned long ulPacketSize = pSendPacket->PayloadMaxSize();
  bool bRun = true;
  while(bRun)
  {

    string strPacket;
    cout << ">> ";
    cin >> strPacket;

    pSendPacket->SetPayload(strPacket.c_str(), (unsigned long)strPacket.size());

    oUDPClient.Send(pSendPacket);
    cout << "Packet Sent Waiting on reply.\r\n";
    oUDPClient.WaitOnIncoming(2000);
    if(oUDPClient.Recv(pRecvPacket) > 0)
    {
      cout << "Packet Received.\r\n";
      int iCompareResult = memcmp(pSendPacket->Payload(), pRecvPacket->Payload(), ulPacketSize);
      if(iCompareResult != 0)
      {
        cout << "****ERROR**** Packetd received does not match Packet sent.\r\n";
      }
    }
    else
    {
      cout << "****ERROR**** No echo returned.\r\n";
    }
  }

  delete pSendPacket;
  delete pRecvPacket;

  oUDPClient.Close();
}
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
void UDPTest::ThrottlerTest()
{
  std::deque<class Packet> vOutStandingPacket;

  const unsigned long MAX_NUMBER_OF_OUTSTANDING_Packet = 512;
  unsigned long ulNumberOfTransmissions = 0UL;
  unsigned long ulErrorTransmission = 0UL;
  class Packet* pSendPacket = new Packet();
  class Packet* pRecvPacket = new Packet();
  UDPClient<class Packet> oUDPClient;
  oUDPClient.Connect(gusServerPort, gcpServerIP);
  unsigned long ulPacketSize = pSendPacket->PayloadMaxSize();
  char* cpRandom = new char[ulPacketSize];

  unsigned long ulKiloBitsPerSecond = 0UL;
  cout << "Enter Throttled Rate (kbits/sec): ";
  cin >> ulKiloBitsPerSecond;

  oUDPClient.SetSendingRate(ulKiloBitsPerSecond);

  while(ulNumberOfTransmissions < MAX_NUMBER_OF_TRANSMISSIONS)
  {
    for(unsigned long i = 0; i < ulPacketSize; i++)
    {
      cpRandom[i] = rand() % 255;
      //cpRandom[i] = i % 16;
    }
    pSendPacket->SetPayload(cpRandom, ulPacketSize);

    oUDPClient.Send(pSendPacket);
    if(vOutStandingPacket.size() > MAX_NUMBER_OF_OUTSTANDING_Packet)
      vOutStandingPacket.pop_front();

    vOutStandingPacket.push_back(*pSendPacket);

    if(oUDPClient.Recv(pRecvPacket) > 0)
    {
      deque<class Packet>::iterator itrOutstandingVector;
      itrOutstandingVector = vOutStandingPacket.begin();
      bool bFound = false;
      while(!bFound && (itrOutstandingVector != vOutStandingPacket.end()))
      {
        int iCompareResult = memcmp(itrOutstandingVector->Payload(), pRecvPacket->Payload(), ulPacketSize);
        if(iCompareResult == 0)
        {
          bFound = true;
          vOutStandingPacket.erase(itrOutstandingVector);
          cout << "Successful Echo.\r\n";
        }
        else
          itrOutstandingVector++;
      }
      if(!bFound)
      {
        cout << "****ERROR**** Echo error number: " << ulErrorTransmission << ".\r\n";
        ulErrorTransmission++;
      }
    }

    ulNumberOfTransmissions++;
  }

  delete [] cpRandom;
  delete pSendPacket;
  delete pRecvPacket;

  cout << "Echoed " << 100.0F - (((float)ulErrorTransmission / (float)ulNumberOfTransmissions) * 100.0F) << "% Packets successfully.\r\n";
  oUDPClient.Close();
}
//---------------------------------------------------------------------------//
// End of UDPTest implementation
//***************************************************************************//