//***************************************************************************//
// DNS Class Interface
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

#ifndef MST_DNS_HPP
#define MST_DNS_HPP

//***************************************************************************//
/// System Includes
#include <string>
//***************************************************************************//

//***************************************************************************//
// Local Includes
#include "Socket.hpp"
//***************************************************************************//

//***************************************************************************//
/// This define describes the maximum name a host name can be for NetworkingMiller.
/// A host name is something as simpler as "server" or a set of domains and
/// sub domains. For example mail.gmail.com and maps.gmail.com
#define MAX_HOSTNAME_LENGTH 256
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //*************************************************************************//
  /// This class is an singleton and should be used for all DNS operations.
  /// This class is intended to be used a way to quickly resolve domain names
  /// in workable IP addresses
  class DNS // Singleton
  {
  private:
    //***********************************************************************//
    //-----------------------------------------------------------------------//
    static class DNS* mpInstance;
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    /// The constructor is private for singleton design pattern
    DNS();
    //-----------------------------------------------------------------------//
    //***********************************************************************//
  public:
    //***********************************************************************//
    //-----------------------------------------------------------------------//
    ~DNS();
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    /// Returns the instance to this class
    /// This is method is a requirement of the singleton design pattern.
    /// \return static pointer to the only instance allowed of this class.
    static class DNS* _cdecl Instance()
    {
      if(mpInstance == NULL)
        mpInstance = new DNS();
      return mpInstance;
    }
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    /// This method will convert a numeric IP address like "127.0.0.1" 
    /// into a unsigned long that is in host order (little endian of x86)
    /// \param cpNumericAddress [in]
    /// Is a null terminated char* that holds the numeric address.
    /// \return This method will either return a converted ip address in host order
    ///  or will return INADDR_NONE on error.
    unsigned long _cdecl ConvertNumericAddress(const char* cpNumericAddress) const;
    /// \overload unsigned long ConvertNumericAddress(std::string& strNumericAddress)
    /// \param strNumericAddress [in] 
    /// Is a std::string  that holds the numeric address.
    /// \return This method will either return a converted ip address in host order
    ///  or will return INADDR_NONE on error.
    unsigned long _cdecl ConvertNumericAddress(const std::string& strNumericAddress) const;
    /// The below method will convert an IP in host order to null terminated
    /// char*.
    /// \param ulIP [in]
    /// A IPv4 IP address in host order (x86 is little endian).
    /// \return This method will return null terminated char* that holds 
    /// the converted address. An example format would be 192.168.1.3.
    char* _cdecl ConvertNumericAddress(unsigned long ulIP) const;
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    std::string _cdecl LocalHostName() const;
    unsigned long _cdecl LocalIPAddress() const;
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    /// The below method will do an DNS look up to resolve the host name into 
    /// network order (big endian) IPv4 IP address.
    /// \param cpHostName [in]
    /// Is a null terminated char* that holds the domain name to resolve
    /// \param usLength [in]
    /// Is the The number of bytes in parameter 1 before the null.
    /// \return This will method will return and unsigned long, network order,
    /// (big endian) IPv4 address.
    unsigned long _cdecl Resolve(const char* const cpHostName, unsigned short usLength = MAX_HOSTNAME_LENGTH) const;
    /// \overload unsigned long Resolve(std::string& strHostname)
    /// \param strHostname [in]
    /// std::string that holds the domain name to resolve.
    /// \return This will method will return and unsigned long, network order,
    /// (big endian) IPv4 address.
    unsigned long _cdecl Resolve(const std::string& strHostname) const;
    //-----------------------------------------------------------------------//
    //***********************************************************************//
  }; // End of class DNS
  //*************************************************************************//
} // End of namespace MST
//***************************************************************************//

#endif MST_DNS_HPP
