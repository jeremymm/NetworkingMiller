//***************************************************************************//
// DNS Class Implementation
//
// Created: Feb 20, 2006
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
#ifdef UNIX
#include <netdb.h>
#endif UNIX
#ifdef WIN32
#include <WinSock2.h>
#endif
#include <assert.h> // For assertions
#include <iostream> // For access to cerr
//***************************************************************************//

//***************************************************************************//
// Local Includes
#include "DNS.hpp"
//***************************************************************************//

//***************************************************************************//
using namespace std;
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //*************************************************************************//
  // Start of DNS implementation
  //-------------------------------------------------------------------------//
  class DNS* DNS::mpInstance = NULL;
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  DNS::DNS()
  {
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  DNS::~DNS()
  {
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  unsigned long DNS::ConvertNumericAddress(const char* cpNumericAddress) const
  {
    //-----------------------------------------------------------------------//
    // Since winsock is not IPv6 ready I must separate the the Windows
    // implementation from the UNIX implementation.
#ifdef UNIX
    unsigned long ulIPAddress = inet_pton(AF_INET, cpNumericAddress, &ulIPAddress);
#elif WIN32
    // Convert char* to unsigned long in network order (big endian).
    unsigned long ulIPAddress;
    if(strncmp(cpNumericAddress, "255.255.255.255", 15) == 0)
    {
      ulIPAddress = 0xFFFFFFFF;
    }
    else
    {
      ulIPAddress = inet_addr(cpNumericAddress);
      // If conversion was successful leave set a error condition.
      if(ulIPAddress == INADDR_NONE)
        ulIPAddress = 0;
    }
#endif
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    // If and error occurred during the conversion; display that error to the user
    // and throw and assertion.
    /// \todo better error handling.
    if(ulIPAddress == 0)
      cerr << "DNS failed to convert numeric address: " << cpNumericAddress << " to unsigned long\r\n";
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    // Return result.
    return ntohl(ulIPAddress);
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    // For IPv6 change family to AF_INET6 and convert method to handle longer than 4 byte addresses.
    //-----------------------------------------------------------------------//
  } // End of unsigned long DNS::ConvertNumericAddress(const char* cpNumericAddress)
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  // See comments for unsigned long DNS::ConvertNumericAddress(const char* cpNumericAddress)
  unsigned long DNS::ConvertNumericAddress(const std::string& strNumericAddress) const
  {
    // Get the const char* from the std::string and call
    // unsigned long DNS::ConvertNumericAddress(const char* cpNumericAddress)
    return this->ConvertNumericAddress(strNumericAddress.c_str());
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  char* DNS::ConvertNumericAddress(unsigned long ulIP) const
  {
    struct in_addr oAddress;
    oAddress.s_addr = (u_long)(htonl(ulIP));
    return inet_ntoa(oAddress);
    // For IPv6 use inet_ntop, windows does not have an implementation for inet_ntop.
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  std::string DNS::LocalHostName() const
  {
    string strReturnValue;
    char* cpBuffer = new char[255];
    if(gethostname(cpBuffer, 255) == 0)
      strReturnValue = cpBuffer;
    else
      strReturnValue = "";
    delete [] cpBuffer;
    return strReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  unsigned long DNS::LocalIPAddress() const
  {
    return Resolve(LocalHostName());
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  unsigned long DNS::Resolve(const char* cpHostName, unsigned short usLength) const
  {
    unsigned long ulReturnValue = 0UL;
    if(cpHostName && usLength)
    {
      //---------------------------------------------------------------------//
      // First get true, or corrected, length of the string.
#if _MSC_VER == 1400 // If we are compiling with VC8...
      const unsigned short cusMeasureLength    = (unsigned short)strnlen(cpHostName, usLength);
#else // Otherwise
      const unsigned short cusMeasureLength    = (unsigned short)strlen(cpHostName);
#endif
      const unsigned short cusLengthOfHostName =  cusMeasureLength < usLength ? cusMeasureLength : usLength;
      //---------------------------------------------------------------------//

      //---------------------------------------------------------------------//
      // Look for alpha character if none convert IP address.
      unsigned short i;
      for(i = 0; i < cusLengthOfHostName; i++)
      {
        if(isalpha(cpHostName[i]))
          break;
      }
      //---------------------------------------------------------------------//

      //---------------------------------------------------------------------//
      // Check to see if found an alpha char and if we did not call convert to 
      // parse IPAddress (i.e. 127.0.0.1) into unsigned long using convert.
      if((i == cusLengthOfHostName) && !isalpha(cpHostName[cusLengthOfHostName]))
      {
        ulReturnValue = this->ConvertNumericAddress(cpHostName);
      }
      else
      {
        //-------------------------------------------------------------------//
        struct hostent* pHostInformation;
        pHostInformation = gethostbyname(cpHostName);
        /// \todo check for errors. See section 11.3 in Stevens.
        //-------------------------------------------------------------------//

        if(pHostInformation)
        {
          memcpy(&ulReturnValue, pHostInformation->h_addr, sizeof(ulReturnValue));
        }
        else
        {
          ulReturnValue = 0L;
        }

        //-------------------------------------------------------------------//
        /// \todo setup methods for returning more than one address.
        ulReturnValue = static_cast<unsigned long>(ntohl(ulReturnValue));
        //-------------------------------------------------------------------//
      }
      //---------------------------------------------------------------------//
    }
    return ulReturnValue;
  } // unsigned long DNS::Resolve(const char* cpHostName, unsigned short usLength)
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  unsigned long DNS::Resolve(const std::string& strHostname) const
  {
    return this->Resolve(strHostname.c_str(), (unsigned short)strHostname.size());
  }
  //-------------------------------------------------------------------------//
  // End of DNS implementation
  //*************************************************************************//
} // End of namespace MST
//***************************************************************************//
