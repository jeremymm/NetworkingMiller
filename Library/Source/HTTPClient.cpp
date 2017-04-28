//***************************************************************************//
// HTTPClient Class Implementation
//
// Created: Mar 25, 2006
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
#include <assert.h> // For assertions
#include <fstream>  // For file streams
#include <sstream>  // For stringstreams
#include <iostream> // For cout only in debug. This can be removed
//***************************************************************************//

//***************************************************************************//
// Local Includes
#include "DNS.hpp"
#include "HTTPClient.hpp"
#include "Packet.hpp"
//***************************************************************************//

//***************************************************************************//
#define HTTP_SERVER_PORT 80
#define USER_AGENT_STRING "NetworkingMiller Library HTTPClient Class Version 0.02.11092006 www.imagespaceinc.com"
//***************************************************************************//

//***************************************************************************//
using namespace std;
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //*************************************************************************//
  // Start of HTTPClient Implementation
  //-------------------------------------------------------------------------//
  HTTPClient::HTTPClient()
  {
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  HTTPClient::~HTTPClient()
  {
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  long HTTPClient::Connect(const std::string& strHostName, unsigned short usPort, bool bBlock)
  {
    mstrHost = strHostName;
    return TCPClient::Connect(usPort, strHostName, bBlock);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  long HTTPClient::Connect(const char* cpHostName, unsigned short usPort, bool bBlock)
  {
    mstrHost = cpHostName;
    return TCPClient::Connect(usPort, cpHostName, bBlock); 
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  long HTTPClient::Connect(unsigned long ulIP, unsigned short usPort, bool bBlock)
  {
    mstrHost = DNS::Instance()->ConvertNumericAddress(ulIP);
    return TCPClient::Connect(usPort, ulIP, bBlock);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void HTTPClient::Disconnect()
  {
    TCPClient::Disconnect();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool HTTPClient::Get(const std::string& rstrPath, const std::string& rstrFileName, const std::string& rstrLocalFileName)
  {
    //-----------------------------------------------------------------------//
    // Make sure we are connected before we request at file.
    assert(Connected());
    if(Connected())
    {
      //---------------------------------------------------------------------//
      // First lets try to create the file what we will store the the data to.
      ofstream ofsOutput(rstrLocalFileName.c_str(), ios::out | ios::binary );
      // Throw and assert if the output file failed to open.
      assert(ofsOutput.is_open());
      //---------------------------------------------------------------------//

      //---------------------------------------------------------------------//
      if(ofsOutput.is_open())
      {
        //-------------------------------------------------------------------//
        // Prepare the HTTP command to send to the HTTP server.
        //stringstream strmCommand;
        HTTPRequest oRequest;
        oRequest.SetMethod(HTTP_REQUEST_METHOD_GET);
        oRequest.SetPath(rstrPath);
        oRequest.SetFile(rstrFileName);
        oRequest.GenerateString();

        // Send the command to the server
        class Packet* pPacket = new Packet();
        pPacket->SetPayload(oRequest.String().c_str(), static_cast<unsigned long>(oRequest.String().size()));
        this->Send(*pPacket);
        pPacket->Clear();
        //-------------------------------------------------------------------//

        //-------------------------------------------------------------------//
        // The below string is used to to hold data from the HTTP server 
        // temporarily until it can be parsed or written to a file.
        string strTemp;
        // This bool is used to see if the delimiter has been found when using 
        // the overload Read method below.yea
        bool bDelimiterFound = false;
        // The below unsigned long is used to hold the file size of the 
        // incoming file.
        unsigned long ulSizeOfIncomingFile = 0;
        //-------------------------------------------------------------------//

        TCPClient::WaitOnIncoming();

        //-------------------------------------------------------------------//
        /// \todo Get the maximum line size of the HTTP returned protocol and replace.
        // the 5096L magic number
        HTTPResponse oResponse;
        while(TCPClient::Recv(pPacket) > 0)
        {
          pPacket->FillString(strTemp);
          pPacket->Clear();
          
          oResponse.ParseString(strTemp);
          //-----------------------------------------------------------------//
        } // End of while(this->Read(strTemp, 5096L, "\r\n", bDelimiterFound) > 0)
        //-------------------------------------------------------------------//

        assert(!"DISABLED");
        ////-------------------------------------------------------------------//
        //// Now we can read the file in.  
        //long          lBytesRead       = 0L;
        //unsigned long ulTotalBytesRead = 0L;
        //while((lBytesRead = TCPClient::Recv(pPacket)) > 0)
        //{
        //  pPacket->FillString(strTemp);
        //  pPacket->Clear();

        //  ofsOutput.write(strTemp.c_str(), lBytesRead);
        //  strTemp.clear();

        //  ulTotalBytesRead += lBytesRead;        
        //  if(ulTotalBytesRead >= ulSizeOfIncomingFile)
        //    break;
        //}
        ////-------------------------------------------------------------------//

        //-------------------------------------------------------------------//
        // We are done reading from data from the HTTP server and writing it 
        // the output file so we can safely close the out file.
        ofsOutput.close();
        //-------------------------------------------------------------------//

        delete pPacket;

      } // End of if(ofsOutput.is_open())
      //---------------------------------------------------------------------//
    } // End of if(Connected())
    //-----------------------------------------------------------------------//
    return true;
  } // End of bool HTTPClient::Get(std::string& strPathAndFileName, std::string& strLocalFileName)
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool HTTPClient::Get(const char* cpPath, const char* cpFileName, const char* cpLocalFileName)
  {
    return Get(static_cast<string>(cpPath), static_cast<string>(cpFileName), static_cast<string>(cpLocalFileName));
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool HTTPClient::Get(const std::string& rstrPath, const std::string& rstrFileName, unsigned long ulOffset, unsigned long ulLength, std::string& rstrData)
  {
    //-----------------------------------------------------------------------//
    // Make sure we are connected before we request at file.
    assert(Connected());
    if(Connected())
    {
      //---------------------------------------------------------------------//
      // Prepare the HTTP command to send to the HTTP server.
      HTTPRequest oRequest;
      oRequest.SetMethod(HTTP_REQUEST_METHOD_GET);
      oRequest.SetPath(rstrPath);
      oRequest.SetFile(rstrFileName);
      oRequest.SetRange(ulOffset, ulLength);
      oRequest.GenerateString();

      // Send the command to the server
      class Packet* pPacket = new Packet();
      pPacket->SetPayload(oRequest.String().c_str(), static_cast<unsigned long>(oRequest.String().size()));
      TCPClient::Send(*pPacket);
      pPacket->Clear();
      //---------------------------------------------------------------------//

      //---------------------------------------------------------------------//
      /// \todo add timeout
      TCPClient::WaitOnIncoming();
      //---------------------------------------------------------------------//

      //---------------------------------------------------------------------//
      // Recv HTTP Response
      HTTPResponse oResponse;
      while(TCPClient::Recv(pPacket) > 0)
      {
        string strTemp;
        pPacket->FillString(strTemp);
        pPacket->Clear();

        oResponse.ParseString(strTemp);
        //-------------------------------------------------------------------//
      } // End of if(TCPClient::Recv(pPacket) > 0)
      //---------------------------------------------------------------------//
      

      //---------------------------------------------------------------------//
      // Now we can read the file in.  
      long          lBytesRead       = 0L;
      unsigned long ulTotalBytesRead = 0L;
      while((lBytesRead = TCPClient::Recv(pPacket)) > 0)
      {
        string strTemp;
        pPacket->FillString(strTemp);
        pPacket->Clear();

        HTTPResponse oResponse;
        oResponse.ParseString(strTemp);

        rstrData += strTemp;
        strTemp.clear();

        ulTotalBytesRead += lBytesRead;   
        // Fix
        if((ulTotalBytesRead >= oResponse.ContentLength()) || (ulTotalBytesRead >= oResponse.DataSize()))
          break;
      }
      //---------------------------------------------------------------------//

      delete pPacket;

      //---------------------------------------------------------------------//
    } // End of if(Connected())
    //-----------------------------------------------------------------------//
    return true;
  } // End of bool HTTPClient::Get(std::string& strPathAndFileName, unsigned long ulOffset, unsigned long ulLength, std::string& strData)
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  enum HTTPStatusCode HTTPClient::Post(std::vector<std::string> &vNames, std::vector<std::string> &vValues, std::string &strPathAndFileName)
  {
    //-----------------------------------------------------------------------//
    // Make sure we are connected before and that size of the names and values is
    // the same and that size of the names and values is not too long.
    assert(Connected());
    assert(vNames.size() == vValues.size());
    assert(vNames.size() <= USHRT_MAX);
    //-----------------------------------------------------------------------//
    assert(!"Disabled");

    ////-----------------------------------------------------------------------//
    //if(Connected() && (vNames.size() == vValues.size()) && (vNames.size() <= USHRT_MAX))
    //{
    //  //---------------------------------------------------------------------//
    //  // Prepare the HTTP command to send to the HTTP server.
    //  stringstream strmCommand;
    //  strmCommand << "POST " << strPathAndFileName << " HTTP/1.1\r\n";
    //  strmCommand << "User-Agent: " << USER_AGENT_STRING << "\r\n";
    //  strmCommand << "Accept: */*\r\n";
    //  strmCommand << "Host: " << DNS::Instance()->ConvertNumericAddress(Socket::mRemoteEndPoint.IP()) << "\r\n";
    //  strmCommand << "Connection: Keep-Alive\r\n";
    //  strmCommand << "Content-Type: application/x-www-form-urlencoded\r\n";
    //  //---------------------------------------------------------------------//

    //  //---------------------------------------------------------------------//
    //  //Calculate what the length will be 
    //  unsigned long ulTotalLength = 0L;
    //  for(unsigned short i = 0; i < vNames.size(); i++)
    //  {
    //    if(i < vNames.size() - 1)
    //      ulTotalLength += static_cast<unsigned long>((vNames[i].size() + vValues[i].size() + 2));
    //    else
    //      ulTotalLength += static_cast<unsigned long>((vNames[i].size() + vValues[i].size() + 1));
    //  } // End of for(unsigned short i = 0; i < vNames.size(); i++)
    //  //---------------------------------------------------------------------//

    //  //---------------------------------------------------------------------//
    //  // Write the length we calculated above to the command
    //  strmCommand << "Content-Length: " << ulTotalLength << "\r\n";
    //  strmCommand << "\r\n";
    //  //---------------------------------------------------------------------//

    //  //---------------------------------------------------------------------//
    //  for(unsigned short i = 0; i < vNames.size(); i++)
    //  {
    //    //-------------------------------------------------------------------//
    //    // replace any whitespace in the values with a '+'
    //    size_t k = vValues[i].find(" ");
    //    if(k != string::npos)
    //    {
    //      for(unsigned short j = (unsigned short)k; j < (unsigned short)vValues[i].size(); j++)
    //      {
    //        if(vValues[i][j] == ' ')
    //          vValues[i][j] = '+';
    //      }
    //    }
    //    //-------------------------------------------------------------------//

    //    //-------------------------------------------------------------------//
    //    // Write name and value to the command
    //    strmCommand << vNames[i] << "=" << vValues[i];
    //    //-------------------------------------------------------------------//

    //    //-------------------------------------------------------------------//
    //    // If there are move name/value pairs to add, insert a "&" to separate
    //    // the pairs.
    //    if(i < vNames.size() - 1)
    //      strmCommand << "&";
    //    //-------------------------------------------------------------------//
    //  } // End of for(unsigned short i = 0; i < vNames.size(); i++)
    //  //---------------------------------------------------------------------//

    //  //---------------------------------------------------------------------//
    //  // Close off the command
    //  strmCommand << "\r\n";
    //  //---------------------------------------------------------------------//

    //  //---------------------------------------------------------------------//
    //  // Send the command to the server
    //  /// \todo Check to make sure the whole command was sent.
    //  class Packet* pPacket = new Packet();
    //  pPacket->SetPayload(strmCommand.str().c_str(), static_cast<unsigned long>(strmCommand.str().size()));
    //  long lBytesSent = TCPClient::Send(*pPacket);
    //  pPacket->Clear();

    //  //---------------------------------------------------------------------//
    //  assert(lBytesSent == strmCommand.str().size());

    //  std::string strData;
    //  bool bDelimiterFound = false;
    //  long lResult = 0L;
    //  /// \todo replace magic number 2048L
    //  bool bFoundHTTP_OK = false;
    //  unsigned long ulContentLength = 0L;
    //  while((lResult = TCPClient::Recv(pPacket)) > 3)
    //  {
    //    pPacket->FillString(strData);
    //    pPacket->Clear();

    //    if(strData.find("\r\n") != string::npos)
    //      bDelimiterFound = true;
    //    else
    //      bDelimiterFound = false;

    //    /// \todo add boolean bit map to prevent repeated string compares
    //    if((strData.find("HTTP") != string::npos) && (strData.find("200") != string::npos))
    //    {
    //      bFoundHTTP_OK = true;
    //    }
    //    else if(strData.find("Content-Length") != string::npos)
    //    {
    //      long lIndex = (long)strData.find(": ");
    //      if(lIndex != string::npos)
    //      {
    //        // Temp solution to show atari this works
    //        stringstream sstrmTemp(strData);
    //        string strJunk;
    //        sstrmTemp >> strJunk;
    //        sstrmTemp >> ulContentLength;
    //      }
    //    }

    //    // ;
    //  }

    //  if(ulContentLength)
    //  {
    //    lResult = TCPClient::Recv(pPacket);

    //    pPacket->FillString(strData);
    //    pPacket->Clear();

    //    if(lResult == ulContentLength)
    //    {
    //      cerr << strData;
    //    }
    //  }

    //  //---------------------------------------------------------------------//
    //  if(bFoundHTTP_OK)
    //    return HTTP_OK;
    //  else
    //    return HTTP_BAD_REQUEST;
    //  //---------------------------------------------------------------------//

    //  delete pPacket;

    //  //---------------------------------------------------------------------//
    //} // End of if(Connected() && (vNames.size() == vValues.size()) && (vNames.size() <= USHRT_MAX))
    ////-----------------------------------------------------------------------//
    //else
      return HTTP_BAD_REQUEST; /// \todo correct this to use the proper value from the server.
    //-----------------------------------------------------------------------//
  } // End of bool HTTPClient::Post(std::vector<std::string> &vNames, std::vector<std::string> &vValues, std::string &strPathAndFileName)
  //-------------------------------------------------------------------------//
  // End of HTTPClient Implementation
  //*************************************************************************//
} // End of namespace MST
//***************************************************************************//