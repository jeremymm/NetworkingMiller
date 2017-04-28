//***************************************************************************//
// EndPoint Class Implementation
//
// Created: June 28, 2008
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
#include "EndPoint.hpp"
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //*************************************************************************//
  // Start of EndPoint Implementation
  //-------------------------------------------------------------------------//
  EndPoint::EndPoint()
  {
    mulIP = 0;
    musPort = 0;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  EndPoint::EndPoint(const EndPoint& rRHS)
  {
    mulIP = rRHS.mulIP;
    musPort = rRHS.musPort;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  unsigned long EndPoint::IP() const
  {
    return this->mulIP;

  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  unsigned short  EndPoint::Port() const
  {
    return this->musPort;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void  EndPoint::IP(unsigned long ulIP)
  {
    this->mulIP = ulIP;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void  EndPoint::Port(unsigned short usPort)
  {
    this->musPort = usPort;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool EndPoint::VaildDestination() const
  {
    bool bReturnValue = false;
    if(mulIP && musPort)
      bReturnValue = true;
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool EndPoint::VaildSource() const
  {
    bool bReturnValue = false;
    if((mulIP != 0xFFFFFFFF) && musPort)
      bReturnValue = true;
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool EndPoint::operator < (const EndPoint& rRHS) const
  {
    bool bReturnValue = false;
    if(this->mulIP < rRHS.mulIP)
      bReturnValue = true;
    else if(this->mulIP == rRHS.mulIP && this->musPort < rRHS.musPort)
      bReturnValue = true;
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool EndPoint::operator == (const EndPoint& rRHS) const
  {
    bool bReturnValue = false;
    if((this->musPort == rRHS.musPort) && 
      (this->mulIP == rRHS.mulIP))
      bReturnValue = true;
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool EndPoint::operator != (const EndPoint& rRHS) const
  {
    return !(*this == rRHS);
  }
  //-------------------------------------------------------------------------//
  // End of EndPoint Implementation
  //*************************************************************************//
} // End of namespace MST
//***************************************************************************//
