//***************************************************************************//
// RTN (Real Time Network) Transceiver Class Implementation
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

//***************************************************************************//
// System Includes
#include <cmath>
//***************************************************************************//

//***************************************************************************//
// MST Technologies includes
#include <MultithreadedMiller.hpp>
//***************************************************************************//

//***************************************************************************//
// Local Includes
#include "RTNTransceiver.hpp"
#include "UDPServer.hpp"
//***************************************************************************//

//***************************************************************************//
#define DEFAULT_CLIP_DISTANCE 10000.0F
//***************************************************************************//

//***************************************************************************//
using namespace std;
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //-------------------------------------------------------------------------//
  RTNTransceiver::RTNTransceiver()
  {
    this->mpMutex = new Mutex();
    this->mfFarthestClipDistance = DEFAULT_CLIP_DISTANCE;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  RTNTransceiver::~RTNTransceiver()
  {
    delete this->mpMutex;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void RTNTransceiver::SetClipDistance(float fDistance)
  {
    this->mfFarthestClipDistance = fDistance;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  long RTNTransceiver::Recv(class RTNPacket* const pPacket)
  {
    long lReturnValue = ReliableUDP::Recv(pPacket);
    if(lReturnValue > 0)
    {
      if(pPacket->IsFromViewer())
      {
        mpMutex->Lock();
        map<EndPoint, RTNEntityPosition>::iterator itrEntityLocationMap;
        itrEntityLocationMap = this->mmapLocationEndPointMap.find(pPacket->Source());
        RTNEntityPosition oTemp;
        oTemp.mfX = pPacket->X();
        oTemp.mfY = pPacket->Y();
        oTemp.mfZ = pPacket->Z();
        if(itrEntityLocationMap == this->mmapLocationEndPointMap.end())
        {
          this->mmapLocationEndPointMap.insert(make_pair(pPacket->Source(), oTemp));
        }
        else
        {
          itrEntityLocationMap->second = oTemp;
        }
        mpMutex->Unlock();
      }
    }
    return lReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  long RTNTransceiver::Send(class RTNPacket* const pPacket)
  {
    return ReliableUDP::Send(pPacket);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  long RTNTransceiver::Forward(class RTNPacket* const pPacket)
  {
    long lReturnValue = 0;
    vector<pair<enum PacketPriorityType, map<EndPoint, RTNEntityPosition>::iterator> > vPriorities;

    map<EndPoint, RTNEntityPosition>::iterator itrEntityLocationMap;
    mpMutex->Lock();
    itrEntityLocationMap = this->mmapLocationEndPointMap.begin();
    while(itrEntityLocationMap != this->mmapLocationEndPointMap.end())
    {
      if(pPacket->Source() != itrEntityLocationMap->first)
      {
        float fDiff = pPacket->X() - itrEntityLocationMap->second.mfX;
        float fXDiffSquared = fDiff * fDiff;

        fDiff = pPacket->Y() - itrEntityLocationMap->second.mfY;
        float fYDiffSquared = fDiff * fDiff;

        fDiff = pPacket->Z() - itrEntityLocationMap->second.mfZ;
        float fZDiffSquared = fDiff * fDiff;

        float fDistance = sqrt(fXDiffSquared + fYDiffSquared + fZDiffSquared);

        if(fDistance > this->mfMaxDistance)
          this->mfMaxDistance = fDistance;

        if(this->mfMaxDistance > this->mfFarthestClipDistance)
        {
          this->mfMaxDistance = this->mfFarthestClipDistance;
          fDistance = this->mfMaxDistance;
        }

        float fNormalizedDistance = fDistance / this->mfMaxDistance;

        if(fNormalizedDistance < 0.2)
        {
          vPriorities.push_back(make_pair(HIGHEST_PACKET_PRIORITY, itrEntityLocationMap));
        }
        else if(fDistance < 0.4)
        {
          vPriorities.push_back(make_pair(ABOVE_NORMAL_PACKET_PRIORITY, itrEntityLocationMap));
        }
        else if(fDistance < 0.6)
        {
          vPriorities.push_back(make_pair(NORMAL_PACKET_PRIORITY, itrEntityLocationMap));
        }
        else if(fDistance < 0.8)
        {
          vPriorities.push_back(make_pair(BELOW_NORMAL_PACKET_PRIORITY, itrEntityLocationMap));
        }
        else
        {
          vPriorities.push_back(make_pair(LOWEST_PACKET_PRIORITY, itrEntityLocationMap));
        }
        lReturnValue = 0UL;
      }

      itrEntityLocationMap++;
    }

    vector<pair<enum PacketPriorityType, map<EndPoint, RTNEntityPosition>::iterator> >::iterator itrPriorityVector;
    itrPriorityVector = vPriorities.begin();
    while(itrPriorityVector != vPriorities.end())
    {
      pPacket->Destination(itrPriorityVector->second->first);
      pPacket->Priority(itrPriorityVector->first);
      long lResult = this->Send(pPacket);
      if(lResult > 0)
        lReturnValue += lResult;
      itrPriorityVector++;
    }
    mpMutex->Unlock();
    return lReturnValue;
  }
  //-------------------------------------------------------------------------//
} // End of namespace MST
//***************************************************************************//