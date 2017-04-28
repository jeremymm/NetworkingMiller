//***************************************************************************//
// HeartbeatResponseThread Class Interface
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


#ifndef MST_HEARTBEAT_RESPONSE_THREAD_HPP
#define MST_HEARTBEAT_RESPONSE_THREAD_HPP

//***************************************************************************//
// System Includes
#include <deque>
//***************************************************************************//

#include <MultithreadedMiller.hpp>

//***************************************************************************//
// Local Includes
#include "ReliablePacket.hpp"
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //*************************************************************************//
  template <class T>
  class HeartbeatResponseThread : public MST::Thread
  {
  private:
    //***********************************************************************//
    MST::Mutex* mpMutex;
    MST::Event* mpNewPacketEvent;
    MST::Mutex* mpPacketBufferMutex;

    std::deque<T*> mdPacketBuffer;

    class UDP<T>* mpUDP;
    
    static void __cdecl ResponseThread(void* vpParameters);
    //***********************************************************************//
  public:
    //***********************************************************************//
    HeartbeatResponseThread(class UDP<T>* pUDP);
    ~HeartbeatResponseThread();

    bool _cdecl SubmitRequest(const T& rPacket);
    //***********************************************************************//
  }; // End of class HeartbeatResponseThread : public MST::Thread
  //*************************************************************************//
} // End of namespace MST
//***************************************************************************//

#endif MST_HEARTBEAT_RESPONSE_THREAD_HPP
