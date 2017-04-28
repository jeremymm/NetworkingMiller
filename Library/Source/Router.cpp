//***************************************************************************//
// Router Class Implementation
//
// Created: Feb 21, 2009
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
// Local Includes
#include "Route.hpp"
#include "Router.hpp"
//***************************************************************************//

using namespace std;

//***************************************************************************//
namespace MST
{
  //-------------------------------------------------------------------------//
  Router::Router()
  {
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  Router::~Router()
  {
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  unsigned long Router::AddRoute(const class EndPoint& rSource, const class EndPoint& rDestination, bool bIsTCP)
  {
    unsigned long ulReturnValue = 0;

    class Route* pNewRoute = new Route(bIsTCP);
    if(pNewRoute->Open(rSource, rDestination))
    {
      this->mvRoutes.push_back(pNewRoute);
      ulReturnValue = static_cast<unsigned long>(this->mvRoutes.size() - 1);
      while(this->mmKeyRouteMap.find(ulReturnValue) != this->mmKeyRouteMap.end())
      {
        ulReturnValue++;
      }
      this->mmKeyRouteMap.insert(make_pair(ulReturnValue, pNewRoute));
    }  

    return ulReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool Router::RemoveRoute(unsigned long ulRouteKey)
  {
    bool bReturnValue = false;
    map<unsigned long, class Route*>::iterator itrKeyRouteMap;
    itrKeyRouteMap = this->mmKeyRouteMap.find(ulRouteKey);
    if(itrKeyRouteMap != this->mmKeyRouteMap.end())
    {
      delete itrKeyRouteMap->second;
      this->mmKeyRouteMap.erase(itrKeyRouteMap);
    }
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  bool Router::RemoveRoute(const class EndPoint& rSource)
  {
    bool bReturnValue = false;

    map<unsigned long, class Route*>::iterator itrKeyRouteMap = this->mmKeyRouteMap.begin();
    while(itrKeyRouteMap != this->mmKeyRouteMap.end() && !bReturnValue)
    {
      if(itrKeyRouteMap->second->Source() == rSource)
      {
        delete itrKeyRouteMap->second;
        this->mmKeyRouteMap.erase(itrKeyRouteMap);
        bReturnValue = true;
      }
    }
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  bool Router::Start()
  {
    return false;
  }

  bool Router::Stop()
  {
    return false;
  }

  bool Router::OpenPort(const class EndPoint& rSource, bool bIsTCP)
  {
    return false;
  }
}
//***************************************************************************//
