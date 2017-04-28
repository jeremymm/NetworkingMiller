//***************************************************************************//
// HeartbeatRequestThread Class Interface
//
// Created: May 25, 2009
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

#ifndef MST_HEARTBEAT_REQUEST_THREAD_HPP
#define MST_HEARTBEAT_REQUEST_THREAD_HPP

//***************************************************************************//
// System Includes
#include <map>
#include <deque>
//***************************************************************************//

//***************************************************************************//
// MST Technologies Include
#include <MultithreadedMiller.hpp>
//***************************************************************************//

//***************************************************************************//
// Local Includes
#include "UDP.hpp"
#include "ReliablePacket.hpp"
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //*************************************************************************//
  template <class T>
  class ConnectionStatus
  {
    private:
      //*********************************************************************//
      class MST::Timer* mpTimer;
      
      double mdET;

      // This limits the size of the outstanding heartbeats map and deques used
      // for keeping track of stats.
      unsigned long mulMaximumOutstandingHeartbeats;

      // These are used to calculate the a running average ping time
      std::deque<double> mdeqPingSamples;
      double mdRunningAggergatePingTime;

      // This is queue that holds a bool that signals if a pack is lost (true)
      // or if it was returned.
      std::deque<bool> mdeqLostReturnedQueue;
      unsigned long mulLostHeartbeats;


      // first: sequence, second: ET 
      std::map<unsigned long, double> mmapOutStandingRequests;
      // this FIFO queue is used to select heartbeats for eviction.
      //std::deque<unsigned long> mdeqFIFOQueue;

      bool mbBroadcast;

      void DropLostRequest();
      //*********************************************************************//
    public:
      //*********************************************************************//
      ConnectionStatus();
      ~ConnectionStatus();

      unsigned long _cdecl Ping() const;
      float _cdecl Reliability() const;

      void HeartbeatRequestSent(const T& rPacket);
      void HeartbeatResponseRecv(const T& rPacket);
      //*********************************************************************//
  }; // End of class ConnectionStatus
  //*************************************************************************//

  //*************************************************************************//
  template <class T>
  class HeartbeatRequestThread : public MST::Thread
  {
  private:
    //***********************************************************************//
    class UDP<T>* mpUDP;

    static void __cdecl RequestThread(void* vpParameters);

    class MST::Timer* mpTimer;
    class MST::Mutex* mpMutex;
    class MST::Mutex* mpMonitoringListMutex;

    double mdET;

    unsigned long mulOutStandingRequests;

    std::map<class EndPoint, ConnectionStatus<T>*> mmapMonitoringList;
    //***********************************************************************//
  public:
    //***********************************************************************//
    HeartbeatRequestThread(class UDP<T>* pUDP);
    ~HeartbeatRequestThread();

    bool _cdecl SubmitResponse(const T& rPacket);
    bool _cdecl AddEndPoint(const class EndPoint& rEndPoint);
    //***********************************************************************//
  }; // End of class HeartbeatRequestThread : public MST::Thread
  //*************************************************************************//
} // End of namespace MST
//***************************************************************************//

#endif MST_HEARTBEAT_REQUEST_THREAD_HPP
