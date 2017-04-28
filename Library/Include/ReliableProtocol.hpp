//***************************************************************************//
// Reliable Protocol Class Interface
//
// Created: July 4, 2009
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

#ifndef MST_RELIABLE_PROTOCOL_HPP
#define MST_RELIABLE_PROTOCOL_HPP

//***************************************************************************//
// Local Include
#include "UDP.hpp"
#include "PacketArchive.hpp"
#include "HeartbeatRequestThread.hpp"
#include "HeartbeatResponseThread.hpp"
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //*************************************************************************//
  template <class T>
  class ReliableProtocol
  {
  private:
    //***********************************************************************//
    //-----------------------------------------------------------------------//
    // Holds a backlog of packets in case we need to retransmit them.
    class PacketArchiverThread<T>* mpPacketArchiver;
    // Thread to send heartbeats letting others know we are still here
    class HeartbeatRequestThread<T>* mpHeartBeatRequestThread;
    class HeartbeatResponseThread<T>* mpHeartBeatResponseThread;

    std::deque<T> mdeqSendQueue;

    bool ProcessNAK(const T& rPacket);
    bool SubmitNAK(unsigned long ulSequenceNumber, const class EndPoint& rDestination);

    class UDP<T>* mpUDP;

    std::map<EndPoint, unsigned long> mmapSequenceNumbers;
    class MST::Mutex* mpMutex;
    //***********************************************************************//
  public:
    //***********************************************************************//
    ReliableProtocol(class UDP<T>* pUDP);
    ~ReliableProtocol();

    bool SubmitPacket(const T& rPacket);
    bool PreparePacket(T& rPacket);
    
    bool StartThreads();
    bool StopThreads();

    bool HasPacketsToRetransmit() const;
    T& RetransmitQueueFront();
    bool PopRetransmitQueue();

    bool Monitor(class EndPoint& rEndPoint);
    //***********************************************************************//
  }; // End of class ReliableProtocol
  //*************************************************************************//
} // End of namespace MST
//***************************************************************************//

#endif MST_RELIABLE_PROTOCOL_HPP
