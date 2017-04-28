//***************************************************************************//
// EndPoint Class Interface
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

#ifndef MST_END_POINT_HPP
#define MST_END_POINT_HPP

//***************************************************************************//
namespace MST
{
  //*************************************************************************//
  class EndPoint
  {
  private:
    //***********************************************************************//
    unsigned long mulIP;
    unsigned short musPort;
    //***********************************************************************//
  public:
    //***********************************************************************//
    //-----------------------------------------------------------------------//
    EndPoint();
    EndPoint(const EndPoint& rRHS);
    //-----------------------------------------------------------------------//
    
    //-----------------------------------------------------------------------//
    bool operator < (const EndPoint& rRHS) const;
    bool operator == (const EndPoint& rRHS) const;
    bool operator != (const EndPoint& rRHS) const;
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    unsigned long _cdecl IP() const;
    unsigned short _cdecl Port() const;
    void _cdecl IP(unsigned long ulIP);
    void _cdecl Port(unsigned short usPort);
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    bool _cdecl VaildDestination() const;
    bool _cdecl VaildSource() const;
    //-----------------------------------------------------------------------//
    //***********************************************************************//
  }; // End of class EndPoint
  //*************************************************************************//
} // End of namespace MST
//***************************************************************************//

#endif MST_END_POINT_HPP
