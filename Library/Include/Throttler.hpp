//***************************************************************************//
// Throttler Class Interface
//
// Created: Aug 20, 2008
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

#ifndef MST_THROTTLER_HPP
#define MST_THROTTLER_HPP

//***************************************************************************//
// System Includes
#include <deque>
//***************************************************************************//

//***************************************************************************//
// MST Technologies Includes
#include <MultithreadedMiller.hpp>
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //*************************************************************************//
  /// \brief  This class is designed to provide a mechanism to allow objects 
  /// that transfer data to control the rate at which they operate.
  class Throttler 
  {
  private:
    //***********************************************************************//
    std::deque<float> mdRates;
    float mfSumOfRates;

    std::deque<float> mdWaits;
    float mfSumOfWaits;

    /// This timer is used to keep track of how much data is being transferred.
    MST::Timer* mpTimer;

    /// This is the current rate the %Throttler is detecting in bits per second.
    /// This is a running average of the most recently detected rates.  The number
    /// of rates used is given by cmusRunningAverageLength.
    float mfCurrentRate;
    /// This is the target rate the %Throttler is attempting to achieve.
    float mfTargetRate;
    /// This it the recommend wait time in milliseconds that other objects
    /// should use to control the transmission of data.
    float mfRecommendedWait;

    float mfLastDT;
  public:
    //***********************************************************************//
    Throttler();
    ~Throttler();

    /// This method will set the desired transfer rate. This calls will use this 
    /// value to calculate a recommend wait time.
    void _cdecl SetTargetRate(unsigned long ulBitsPerSecond);
    /// This method must be called after each transfer so that this class can 
    /// keep track of the current transfer rate.  If this method is not called
    /// after each successful transfer this class will not function properly and 
    /// the recommend wait time will be inaccurate.
    /*void _cdecl Update(unsigned long ulBytes);*/
    void _cdecl Update(unsigned long ulBytes);
    /// This method returns the recommended wait time in milliseconds.  Objects 
    /// interested in using this class should wait the amount of time between transfers
    /// to accurately throttle the transfer rate.
    float _cdecl RecommendedWait() const;
    //***********************************************************************//
  }; // End of class Throttler 
  //*************************************************************************//
} // End of namespace MST
//***************************************************************************//


#endif MST_THROTTLER_HPP
