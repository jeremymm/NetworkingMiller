//***************************************************************************//
// RTN (Real Time Network) Transceiver Class Interface
//
// Created: Aug 3, 2008
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

#ifndef MST_RTN_TRANSCEIVER_HPP
#define MST_RTN_TRANSCEIVER_HPP

//***************************************************************************//
// System Includes
#include <map>
//***************************************************************************//

//***************************************************************************//
// Local Includes
#include "RTNPacket.hpp"
#include "ReliableUDPServer.hpp"
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //***************************************************************************//
  class RTNEntityPosition
  {
  public:
    //***********************************************************************//
    float mfX;
    float mfY;
    float mfZ;
    //***********************************************************************//
  }; // End of class RTNEntityPosition
  //***************************************************************************//

  //*************************************************************************//
  class RTNTransceiver : public ReliableUDPServer<RTNPacket>
  {
  private:
    //***********************************************************************//
    MST::Mutex* mpMutex;
    std::map<class EndPoint, RTNEntityPosition> mmapLocationEndPointMap;

    float mfMaxDistance;
    float mfFarthestClipDistance;
    //***********************************************************************//
  public:
    //***********************************************************************//
    //-----------------------------------------------------------------------//
    RTNTransceiver();
    virtual ~RTNTransceiver();
    //-----------------------------------------------------------------------//

    void _cdecl SetClipDistance(float fDistance);

    //-----------------------------------------------------------------------//
    virtual long _cdecl Recv(class RTNPacket* const pPacket);
    virtual long _cdecl Send(class RTNPacket* const pPacket);
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    virtual long _cdecl Forward(class RTNPacket* const pPacket);
    //-----------------------------------------------------------------------//
    //***********************************************************************//
  }; // End of class RTNTransceiver
  //*************************************************************************//
} // End of namespace MST
//***************************************************************************//

#endif MST_RTN_TRANSCEIVER_HPP
