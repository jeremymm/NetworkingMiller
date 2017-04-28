//***************************************************************************//
// Route Class Interface
//
// Created: Mar 9, 2009
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

#ifndef MST_ROUTE_HPP
#define MST_ROUTE_HPP

//***************************************************************************//
// System Includes
#include <map>
//***************************************************************************//

//***************************************************************************//
// MST Technologies Includes
#include <MultithreadedMiller.hpp>
//***************************************************************************//

//***************************************************************************//
// Local Includes
#include "UDPClient.hpp"
#include "UDPServer.hpp"
#include "Packet.hpp"
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //*************************************************************************//
  class RouteStrategy
  {
  private:
    //***********************************************************************//
    //***********************************************************************//
  protected:
    //***********************************************************************//
    class EndPoint* mpDestination;
    class EndPoint* mpSource;
    //***********************************************************************//
  public:
    //***********************************************************************//
    RouteStrategy();
    virtual ~RouteStrategy();

    //-----------------------------------------------------------------------//
    virtual bool _cdecl Open(const class EndPoint& rSource, const class EndPoint& rDestination) = 0;
    virtual bool _cdecl Close() = 0;
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    virtual bool _cdecl Start() = 0;
    virtual bool _cdecl Stop() = 0;
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    virtual const class EndPoint& _cdecl Source() const = 0;
    virtual const class EndPoint& _cdecl Destination() const = 0;
    //-----------------------------------------------------------------------//
    //***********************************************************************//
  }; // End of class Route
  //*************************************************************************//

  //*************************************************************************//
  /// \brief RouteTCPStrategy is a threaded class that will send all data it receives 
  /// onto a given destination.  The class will not filter data. A new connection
  /// in this class will result in a new connection to the destination.
  class RouteTCPStrategy : public MST::Thread, public RouteStrategy
  {
  private:
    //***********************************************************************//
    class TCPServer* mpServer;
    std::map<class EndPoint, class TCPClient*> mmClients;
    static void _cdecl ThreadedRoutine(void* vpParameters);
    //***********************************************************************//
  public:
    //***********************************************************************//
    //-----------------------------------------------------------------------//
    RouteTCPStrategy();
    virtual ~RouteTCPStrategy();
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    bool _cdecl Open(const class EndPoint& rSource, const class EndPoint& rDestination);
    bool _cdecl Close();
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    bool _cdecl Start();
    bool _cdecl Stop();
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    const class EndPoint& _cdecl Source() const;
    const class EndPoint& _cdecl Destination() const;
    //-----------------------------------------------------------------------//
    //***********************************************************************//
  }; // End of class RouteTCPStrategy : public MST::Thread, public RouteStrategy
  //*************************************************************************//

  //*************************************************************************//
  /// \brief RouteUDPStrategy is a threaded class that will send all data it receives 
  /// onto a given destination.  The class will not filter data. A new connection
  /// in this class will result in a new connection to the destination.
  class RouteUDPStrategy : public MST::Thread, public RouteStrategy
  {
  private:
    //***********************************************************************//
    class UDPServer<class Packet>* mpServer;
    class EndPoint* mpDestination;
    std::map<class EndPoint, class UDPClient<class Packet>*> mmClients;
    static void _cdecl ThreadedRoutine(void* vpParameters);
    //***********************************************************************//
  public:
    //***********************************************************************//
    RouteUDPStrategy();
    virtual ~RouteUDPStrategy();

    //-----------------------------------------------------------------------//
    bool _cdecl Open(const class EndPoint& rSource, const class EndPoint& rDestination);
    bool _cdecl Close();
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    bool _cdecl Start();
    bool _cdecl Stop();
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    const class EndPoint& _cdecl Source() const;
    const class EndPoint& _cdecl Destination() const;
    //-----------------------------------------------------------------------//
    //***********************************************************************//
  }; // End of class RouteUDPStrategy : public MST::Thread, public RouteStrategy
  //*************************************************************************//

  //*************************************************************************//
  class Route
  {
  private:
    //***********************************************************************//
    class RouteStrategy* mpStrategy;
    //***********************************************************************//
  public:
    //***********************************************************************//
    Route(bool bIsTCP);
    virtual ~Route();

    //-----------------------------------------------------------------------//
    bool _cdecl Open(const class EndPoint& rSource, const class EndPoint& rDestination);
    bool _cdecl Close();
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    bool _cdecl Start();
    bool _cdecl Stop();
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    const class EndPoint& _cdecl Source() const;
    const class EndPoint& _cdecl Destination() const;
    //-----------------------------------------------------------------------//
    //***********************************************************************//
  }; // End of class Route
  //*************************************************************************//
} // End of namespace MST
//***************************************************************************//

#endif MST_ROUTE_HPP
