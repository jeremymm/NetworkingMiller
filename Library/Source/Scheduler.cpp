//***************************************************************************//
// Scheduler Class Implementation
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

//***************************************************************************//
// System Includes
#include <assert.h>
#include <iostream>
//***************************************************************************//

//***************************************************************************//
// MST Technologies Includes
#include <MultithreadedMiller.hpp>
//***************************************************************************//

//***************************************************************************//
// Local Includes
#include "Scheduler.hpp"
#include "Throttler.hpp"
#include "Packet.hpp"
#include "ReliablePacket.hpp"
#include "RTNPacket.hpp"
#include "PacketBuffer.hpp"
//***************************************************************************//

//***************************************************************************//
using namespace std;
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //*************************************************************************//
  const unsigned long culMaxPacketCount = 8;
  //*************************************************************************//

  //*************************************************************************//
  // Start of class SchedulerItem implementation.
  //-------------------------------------------------------------------------//
  SchedulerItem::SchedulerItem(unsigned long ulStarvationThreshold)
  {
    this->mulStarvationThreshold = ulStarvationThreshold;
    this->mulPassOverCount = 0;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  SchedulerItem::~SchedulerItem()
  {
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void SchedulerItem::Feed()
  {
    this->mulPassOverCount = 0;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void SchedulerItem::PassOver()
  {
    if(this->mulPassOverCount < this->mulStarvationThreshold)
      this->mulPassOverCount++;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  float SchedulerItem::StarvationLevel() const
  {
    return (float)this->mulPassOverCount/(float)this->mulStarvationThreshold;
  }
  //-------------------------------------------------------------------------//
  // End of class Scheduler implementation.
  //*************************************************************************//

  //*************************************************************************//
  // Start of class Scheduler implementation.
  template class Scheduler<Packet>;
  template class Scheduler<ReliablePacket>;
  template class Scheduler<RTNPacket>;

  //-------------------------------------------------------------------------//
  template <class T>
  Scheduler<T>::Scheduler()
  {
    this->mpMutex = new Mutex();
    if(mpMutex)
    {
      this->mpMultiplexer = new Multiplexer();
      for(unsigned long i = 0; i < NUMBER_OF_PACKET_PRIORITY_LEVELS; i++)
      {
        this->mvPacketBuffers.push_back(new PacketBuffer<T>(culMaxPacketCount));
        this->mvSchedulerItems.push_back(new SchedulerItem(0x1 << (i*2)));
        this->mpMultiplexer->AddHandle(this->mvPacketBuffers.back()->InsertionEvent().Handle());
      }
      this->mpReadyPacket = new T();
      this->mpThrottler = new Throttler();
      this->mpTimer = new Timer();
    }
    else
    {
      this->mpMultiplexer = NULL;
      this->mpReadyPacket = NULL;
    }

    mbReadyPacketIsValid = false;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  Scheduler<T>::~Scheduler()
  {
    try
    {
      if(this->mpMutex)
      {
        this->mpMutex->Lock();

        for(unsigned long i = 0; i < NUMBER_OF_PACKET_PRIORITY_LEVELS; i++)
        {
          delete this->mvPacketBuffers.back();
          this->mvPacketBuffers.pop_back();
          delete this->mvSchedulerItems.back();
          this->mvSchedulerItems.pop_back();
        }

        if(this->mpMultiplexer)
          delete this->mpMultiplexer;

        if(this->mpReadyPacket)
          delete this->mpReadyPacket;

        if(this->mpThrottler)
          delete this->mpThrottler;

        if(this->mpTimer)
          delete this->mpTimer;

        this->mpMutex->Unlock();

        delete this->mpMutex;
      }
    }
    catch(...)
    {
    }
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool Scheduler<T>::SchedulePacket(const T& rPacket)
  {
    bool bReturnValue = false;
    if(this->mpMutex)
    {
      if(rPacket.Destination().IP() == 0)
        int i = 0;
      this->mpMutex->Lock();

      bReturnValue =  this->mvPacketBuffers[rPacket.Priority()]->Push(rPacket, false);

      if(bReturnValue)
        this->mvPacketBuffers[rPacket.Priority()]->InsertionEvent().Signal();

      this->mpMutex->Unlock();
    }
    return bReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  const T* const Scheduler<T>::PopReadyPacket()
  {
    T* pReturnValue = NULL;
    if(this->mpMutex)
    {
      this->mpMutex->Lock();
      if(this->mbReadyPacketIsValid)
      {
        pReturnValue = this->mpReadyPacket;
      }
      this->mpMutex->Unlock();
    }

    return pReturnValue;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool Scheduler<T>::HasReadyPacket() const
  {
    return this->mbReadyPacketIsValid;
  }
  //-------------------------------------------------------------------------//

  template <class T>
  void Scheduler<T>::SetTargetRate(unsigned long ulBitsPerSecond)
  {
    this->mpThrottler->SetTargetRate(ulBitsPerSecond);
  }

  //-------------------------------------------------------------------------//
  template <class T>
  void Scheduler<T>::Update()
  {
    assert(this->mpMutex);
    if(this->mpMutex)
    {
      this->mpMutex->Lock();
      if(mpTimer->ET(TIMER_1) > this->mpThrottler->RecommendedWait())
      {
        this->mpTimer->DT(TIMER_1);

        //-------------------------------------------------------------------//
        if(this->mvPacketBuffers.size())
        {
          //-----------------------------------------------------------------//
          unsigned long ulMostStarvedIndex = 0;
          float fHighestStarvationLevel = -1.0F;
          //-----------------------------------------------------------------//

          //-----------------------------------------------------------------//
          for(long i = NUMBER_OF_PACKET_PRIORITY_LEVELS - 1; i >= 0; i--)
          {
            if(this->mvPacketBuffers[i]->Length())
            {
              float fTempStatvationLevel = this->mvSchedulerItems[i]->StarvationLevel();
              if(fTempStatvationLevel > fHighestStarvationLevel)
              {
                ulMostStarvedIndex = i;
                fHighestStarvationLevel = fTempStatvationLevel;
              }
            }
          } // End of for(long i = NUMBER_OF_PACKET_PRIORITY_LEVELS - 1; i >= 0; i--)
          //-----------------------------------------------------------------//

          //-----------------------------------------------------------------//
          if(this->mvPacketBuffers[ulMostStarvedIndex]->Length() == 0)
          {
            this->mbReadyPacketIsValid = false;
          }
          else
          {
            this->mbReadyPacketIsValid = true;

            //---------------------------------------------------------------//
            for(unsigned long i = 0; i < NUMBER_OF_PACKET_PRIORITY_LEVELS; i++)
            {
              if(i != ulMostStarvedIndex)
              {
                if(this->mvPacketBuffers[i]->Length())
                  this->mvSchedulerItems[i]->PassOver();
              }
              else
              {
                this->mvSchedulerItems[i]->Feed();
                *(this->mpReadyPacket) = this->mvPacketBuffers[i]->Front();
                
                //-----------------------------------------------------------//
                // Get sequence number if needed
                if(this->mpReadyPacket->NeedsSequence())
                {
                  map<EndPoint, unsigned long>::iterator itrSequenceMap;
                  itrSequenceMap = this->mmapSequenceNumbers.find(this->mpReadyPacket->Destination());
                  unsigned long ulSequenceNumber = 1UL;
                  if(itrSequenceMap == this->mmapSequenceNumbers.end())
                  {
                    this->mmapSequenceNumbers.insert(make_pair(this->mpReadyPacket->Destination(), ulSequenceNumber));
                  }
                  else
                  {
                    itrSequenceMap->second++;
                    ulSequenceNumber = itrSequenceMap->second;
                  }
                  this->mpReadyPacket->SetSequence(ulSequenceNumber);
                } // End of if(this->mpReadyPacket->NeedsSequence())
                //-----------------------------------------------------------//

                this->mpReadyPacket->Serialize();
                this->mvPacketBuffers[i]->Pop();
              }
            } // End of for(unsigned long i = 0; i < NUMBER_OF_PACKET_PRIORITY_LEVELS; i++)
            //---------------------------------------------------------------//

          } // End of else for if(this->mvPacketBuffers[ulMostStarvedIndex]->Count() == 0)
          //-----------------------------------------------------------------//

        } // End of if(this->mvPacketBuffers.size())
        //-------------------------------------------------------------------//
      } // End of if(mpTimer->ET(TIMER_1) > this->mpThrottler->RecommendedWait())
      else
      {
        this->mbReadyPacketIsValid = false;
      }

      if(this->mbReadyPacketIsValid)
        this->mpThrottler->Update(mpReadyPacket->PayloadSize());
      
      this->mpMutex->Unlock();
    } // End of if(this->mpMutex)
    //-----------------------------------------------------------------------//
  } // End of void Scheduler::Run()
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  template <class T>
  bool Scheduler<T>::WaitForReadyPacket(unsigned long ulTimeOut)
  {
    return this->mpMultiplexer->WaitForTrigger(ulTimeOut);
  }
  //-------------------------------------------------------------------------//
  // End of class Scheduler implementation.
  //*************************************************************************//
} // End of namespace MST
//***************************************************************************//
