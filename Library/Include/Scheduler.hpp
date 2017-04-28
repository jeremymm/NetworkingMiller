//***************************************************************************//
// Scheduler Class Interface
//
// Created: Oct 4, 2008
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

#ifndef MST_SCHEDULER_HPP
#define MST_SCHEDULER_HPP

//***************************************************************************//
// System Includes
#include <map>
#include <vector>
//***************************************************************************//

//***************************************************************************//
// MST Technologies Includes
#include <MultithreadedMiller.hpp>
//***************************************************************************//

//***************************************************************************//
// Local Includes
#include "EndPoint.hpp"
#include "PacketBuffer.hpp"
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //*************************************************************************//
  class SchedulerItem
  {
  private:
    //***********************************************************************//
    /// This member is how many times this items has been
    /// passed over by the scheduler by a higher priority item.
    /// once this member gets to zero 
    unsigned long mulPassOverCount;
    /// This member describes how high the count will be until the 
    /// the SchedulerItem is marked as starving.
    unsigned long mulStarvationThreshold;
    //***********************************************************************//
  public:
    //***********************************************************************//
    /// ScheduleItem constructor
    /// \param [in] ulStarvationThreshold
    /// This parameter determines how being passed over effects the 
    /// scheduled item.  The higher this parameter is the less effected
    /// this object will be when passed over.
    SchedulerItem(unsigned long ulStarvationThreshold);
    ~SchedulerItem();

    /// This method will mark this object as feed and will reset the 
    /// data members according to keep track of starvation statics. In this
    /// method case starvation will be set to 0.0.
    void _cdecl Feed();
    /// This method will mark that the SchedulerItem was passed over by 
    /// a higher priority item.  This method will update this object 
    /// starvation status.  Continued calls of this method without
    /// calling Feed will progress the starvation of this SchedulerItem
    /// to 1.0.
    void _cdecl PassOver();

    /// This method will return how starved this SchedulerItem is. 0.0 being
    /// completely not starved and 1.0 being completely starved.
    float _cdecl StarvationLevel() const;
    //***********************************************************************//
  }; // End of class SchedulerItem
  //*************************************************************************//

  //*************************************************************************//
  template <class T>
  class Scheduler
  {
  private:
    //***********************************************************************//
    //-----------------------------------------------------------------------//
    /// This member vector hold pointer to the message buffers that this
    /// scheduler is using to characterize messages passed on priority.
    std::vector<class PacketBuffer<T>*> mvPacketBuffers;
    /// This member vector holds pointer to SchedulerItems. Those 
    /// SchedulerItems contain state that is need to prevent starvation
    /// and to determine which message to process.
    std::vector<class SchedulerItem*> mvSchedulerItems;
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    /// This member will point to the message that is ready to be sent.
    /// Having this extra member will help isolate the read message from 
    /// the messages that are still waiting.
    T* mpReadyPacket;
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    class MST::Multiplexer* mpMultiplexer;
    class MST::Mutex* mpMutex;
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    class Throttler* mpThrottler;
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    /// This member will be true if the %Scheduler successfully transformed
    /// the state of waiting message into the ready state. 
    bool mbReadyPacketIsValid;
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    class MST::Timer* mpTimer;
    //-----------------------------------------------------------------------//

    std::map<EndPoint, unsigned long> mmapSequenceNumbers;
    //***********************************************************************//
  public:
    //***********************************************************************//
    //-----------------------------------------------------------------------//
    Scheduler();
    ~Scheduler();
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    /// This method will add a Packet to the Scheduler. This method 
    /// will characterize the Packet based on its priority.
    /// \param [in] rPacket
    /// This parameter is the message the callee wished to schedule.
    /// \return
    /// If this message was successfully scheduled then this method will
    /// return true. Otherwise, the method will return false.
    bool _cdecl SchedulePacket(const T& rPacket);
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    /// This method will return a reference to the Packet that is ready to
    /// processed next.  
    /// \return
    /// This method returns 
    const T* const _cdecl PopReadyPacket();
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    bool _cdecl WaitForReadyPacket(unsigned long ulTimeOut);
    bool _cdecl HasReadyPacket() const;
    //-----------------------------------------------------------------------//

    /// This method will set the desired transfer rate. This calls will use this 
    /// value to calculate a recommend wait time.
    void _cdecl SetTargetRate(unsigned long ulBitsPerSecond);

    //-----------------------------------------------------------------------//
    void _cdecl Update();
    //-----------------------------------------------------------------------//
    //***********************************************************************//
  }; // End of class Scheduler
  //*************************************************************************//
} // End of namespace MST
//***************************************************************************//

#endif MST_SCHEDULER_HPP
