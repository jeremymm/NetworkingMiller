//***************************************************************************//
// HTTP Class Implementation
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
#include <sstream>
//***************************************************************************//

//***************************************************************************//
// Local Includes
#include "DNS.hpp"
#include "HTTP.hpp"
//***************************************************************************//

//***************************************************************************//
#define USER_AGENT_STRING "nMotor Library Version 0.02.11092006 www.imagespaceinc.com"
//***************************************************************************//

using namespace std;

//***************************************************************************//
namespace MST
{
  //*************************************************************************//
  // Start of HTTPRequest implementation
  //-------------------------------------------------------------------------//
  HTTPRequest::HTTPRequest()
  {
    this->mstrRequest = "";
    this->mulRangeOffset = 0UL;
    this->mulRangeLength = 0UL;
    this->meMethod = HTTP_REQUEST_METHOD_UNKNOWN;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  HTTPRequest::~HTTPRequest()
  {
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void HTTPRequest::SetMethod(HTTPRequestMethod eMethod)
  {
    this->meMethod = eMethod;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void HTTPRequest::SetPath(const std::string& rstrPath)
  {
    if(rstrPath.size())
    {
      this->mstrPath = "";

      // Ensure leading '/'
      if(*(rstrPath.begin()) != '/')
      {
        this->mstrPath = "/";
      }

      this->mstrPath += rstrPath;

      // Ensure trailing '/'
      if(*(this->mstrPath.end() - 1) != '/')
      {
        this->mstrPath += "/";
      }
    }
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void HTTPRequest::SetFile(const std::string& rstrFile)
  {
    if(rstrFile.size())
      this->mstrFile = rstrFile;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void HTTPRequest::SetHost(const EndPoint& rLocalEndPoint)
  {
    this->mLocalEndpoint = rLocalEndPoint;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void HTTPRequest::SetRange(unsigned long ulOffset, unsigned long ulLength)
  {
    this->mulRangeOffset = ulOffset;
    this->mulRangeLength = ulLength;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void HTTPRequest::GenerateString()
  {
    if(this->meMethod != HTTP_REQUEST_METHOD_UNKNOWN)
    {
      stringstream strmCommand;

      //---------------------------------------------------------------------//
      // Set method type
      if(this->meMethod == HTTP_REQUEST_METHOD_GET)
        strmCommand << "GET " << this->mstrPath << this->mstrFile << " HTTP/1.1\r\n";
      //---------------------------------------------------------------------//

      //---------------------------------------------------------------------//
      // Set host if possible
      if(this->mLocalEndpoint.VaildSource())
        strmCommand << "Host: " << DNS::Instance()->ConvertNumericAddress(this->mLocalEndpoint.IP()) << "\r\n";
      //---------------------------------------------------------------------//

      //---------------------------------------------------------------------//
      // Set user agent
      strmCommand << "User-Agent: " << USER_AGENT_STRING << "\r\n";
      //---------------------------------------------------------------------//

      //---------------------------------------------------------------------//
      // If we have a desired range then add it to the request
      if(this->mulRangeLength != 0UL)
        strmCommand << "Range:bytes=" << this->mulRangeOffset << '-' << this->mulRangeOffset + this->mulRangeLength  << "\r\n";
      else if(this->mulRangeOffset != 0UL)
        strmCommand << "Range:bytes=" << this->mulRangeOffset << '-' << "\r\n";
      //---------------------------------------------------------------------//

      //---------------------------------------------------------------------//
      // close request
      strmCommand << "\r\n";
      //---------------------------------------------------------------------//

      //---------------------------------------------------------------------//
      // Set the request string for this instance
      this->mstrRequest = strmCommand.str();
      //---------------------------------------------------------------------//
    } // End of if(this->meMethod != HTTP_REQUEST_METHOD_UNKNOWN)
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  const std::string& HTTPRequest::String() const
  {
    return this->mstrRequest;
  }
  //-------------------------------------------------------------------------//
  //-------------------------------------------------------------------------//
  // End of HTTPRequest implementation
  //*************************************************************************//


  //*************************************************************************//
  // Start of HTTPResponse implementation
  //-------------------------------------------------------------------------//
  HTTPResponse::HTTPResponse()
  {
    this->mulParsedIndex = 0UL;
    this->mulDataSize = 0UL;
    this->mulContentLength = 0UL;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  HTTPResponse::~HTTPResponse()
  {
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  unsigned long HTTPResponse::ContentLength() const
  {
    return this->mulContentLength;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  char* const HTTPResponse::Data()
  {
    return NULL;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  unsigned long HTTPResponse::DataSize() const
  {
    return this->mulDataSize;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void HTTPResponse::GenerateString()
  {
    this->mulParsedIndex = 0UL;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void HTTPResponse::ParseString(const std::string& rstrResponse)
  {
    if(rstrResponse.size())
    {
      stringstream sstrmResponse(rstrResponse);
      string strLine;
      while(getline(sstrmResponse, strLine))
      {
        if(strLine.find("HTTP") != string::npos)
        {
          size_t ulIndex = strLine.find(" ");
          if(ulIndex != string::npos)
          {
            string strHTTPVersion = strLine.substr(0, ulIndex);
            strLine.erase(0, ulIndex + 1);
            ulIndex = strLine.find(" ");
            if(ulIndex != string::npos)
            {
              stringstream ssmTemp(strLine.substr(0, ulIndex));
              unsigned long ulStatsCode;
              ssmTemp >> ulStatsCode;
              string strStatusMessage;
              strStatusMessage = strLine.substr(ulIndex + 1);
            }
          }
        }
        else
        {
          if(strLine.find("Accept-Ranges") != string::npos)
          {

          }
          else if(strLine.find("Age") != string::npos)
          {

          }
          else if(strLine.find("Allow") != string::npos)
          {

          }
          else if(strLine.find("Cache-Control") != string::npos)
          {

          }
          else if(strLine.find("Content-Encoding") != string::npos)
          {

          }
          else if(strLine.find("Content-Language") != string::npos)
          {

          }
          else if(strLine.find("Content-Length") != string::npos)
          {
            size_t ulIndex = strLine.find(":");
            if(ulIndex != string::npos)
            {
              this->mulParsedIndex += ulIndex;
              stringstream sstmTemp(strLine.substr(ulIndex));
              this->mulParsedIndex += sstmTemp.str().size();
              sstmTemp >> this->mulContentLength;
            }
          }
          else if(strLine.find("Content-Location") != string::npos)
          {

          }
          else if(strLine.find("Content-Disposition") != string::npos)
          {

          }
          else if(strLine.find("Content-MD5") != string::npos)
          {

          }
          else if(strLine.find("Content-Range") != string::npos)
          {

          }
          else if(strLine.find("Date") != string::npos)
          {

          }
          else if(strLine.find("ETag") != string::npos)
          {

          }
          else if(strLine.find("Expires") != string::npos)
          {

          }
          else if(strLine.find("Last-Modified") != string::npos)
          {

          }
          else if(strLine.find("Location") != string::npos)
          {

          }
          else if(strLine.find("Pragma") != string::npos)
          {

          }
          else if(strLine.find("Proxy-Authenticate") != string::npos)
          {

          }
          else if(strLine.find("Refresh") != string::npos)
          {

          }
          else if(strLine.find("Retry-After") != string::npos)
          {

          }
          else if(strLine.find("Server") != string::npos)
          {

          }
          else if(strLine.find("Set-Cookie") != string::npos)
          {

          }
          else if(strLine.find("Trailer") != string::npos)
          {

          }
          else if(strLine.find("Transfer-Encoding") != string::npos)
          {

          }
          else if(strLine.find("Vary") != string::npos)
          {

          }
          else if(strLine.find("Via") != string::npos)
          {

          }
          else if(strLine.find("Warning") != string::npos)
          {

          }
          else if(strLine.find("WWW-Authenticate") != string::npos)
          {

          }
          else if(strLine.find("X-") != string::npos)
          {
            // Ignore all.
          }
          else
          {
            // Unknown field
          }

        } // End of else for if(bFirstLine)
        //-------------------------------------------------------------------//
      } // End of while(getline(sstrmResponse, strLine))
      //---------------------------------------------------------------------//
    } // End of if(this->mstrResponse.size())
  } // End of void HTTPResponse::ParseString()
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void HTTPResponse::String(const std::string& rstrResponse)
  {
    if(rstrResponse.size())
    {
      this->mstrResponse = rstrResponse;
    }
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  const std::string& HTTPResponse::String() const
  {
    return this->mstrResponse;
  }
  //-------------------------------------------------------------------------//
  // End of HTTPResponse implementation
  //*************************************************************************//

  //*************************************************************************//
  // Start of HTTP implementation
  //-------------------------------------------------------------------------//
  HTTP::HTTP()
  {
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  HTTP::~HTTP()
  {
  }
  //-------------------------------------------------------------------------//
  // End of HTTP implementation
  //*************************************************************************//
} // End of namespace MST
//***************************************************************************//
