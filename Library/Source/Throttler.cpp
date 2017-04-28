//***************************************************************************//
// Throttler Class Implementation
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

//***************************************************************************//
// Local Includes
#include "Throttler.hpp"
//***************************************************************************//

//***************************************************************************//
#define RUNNING_AVERAGE_LENGTH 16
#define RECOMMENDED_WAIT_STEP 0.01F
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //*************************************************************************//
  // Start of Throttler implementation
  //-------------------------------------------------------------------------//
  Throttler::Throttler()
  {   
    mpTimer                = new Timer();
    mfCurrentRate          = 0.0F;
    mfTargetRate           = 0.0F;
    mfRecommendedWait      = 0.0F;
    mfSumOfRates           = 0.0F;
    mfLastDT               = 0.0F;

    mfSumOfWaits = 0.0F;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  Throttler::~Throttler()
  {
    try
    {
      if(this->mpTimer)
        delete this->mpTimer;
    }
    catch(...)
    {
    }
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void Throttler::SetTargetRate(unsigned long ulBitsPerSecond)
  {
    this->mfTargetRate = static_cast<float>(ulBitsPerSecond);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  void Throttler::Update(unsigned long ulBytes)
  {
    if(mfTargetRate > 0.0F)
    {
      float fDT = (float)this->mpTimer->DT(TIMER_1);

      if(mdRates.size() == RUNNING_AVERAGE_LENGTH)
      {
        this->mfSumOfRates -= mdRates.front();
        mdRates.pop_front();
      }
      float fCurrentRate = (ulBytes * 8.0f) / fDT;
      this->mfSumOfRates += fCurrentRate;
      mdRates.push_back(fCurrentRate);

      this->mfCurrentRate = this->mfSumOfRates / mdRates.size();

      float fTempWait = 0.0F;

      bool bModifyRecommendedWait = false;
      if(this->mfCurrentRate > this->mfTargetRate)
      {
        fTempWait = mfRecommendedWait + (RECOMMENDED_WAIT_STEP * fDT);
        bModifyRecommendedWait = true;
      }
      else if(this->mfCurrentRate < this->mfTargetRate)
      {
        fTempWait = mfRecommendedWait - (RECOMMENDED_WAIT_STEP * fDT);
        bModifyRecommendedWait = true;
      }    

      if(fTempWait < 0.0F)
        fTempWait = 0.0F;

      if(bModifyRecommendedWait)
      {
        if(mdWaits.size() == RUNNING_AVERAGE_LENGTH)
        {
          this->mfSumOfWaits -= mdWaits.front();
          mdWaits.pop_front();
        }

        this->mfSumOfWaits += fTempWait;
        mdWaits.push_back(fTempWait);

        if(fTempWait == 0.0F)
          this->mfRecommendedWait = 0.0F;
        else
          this->mfRecommendedWait = this->mfSumOfWaits / mdWaits.size();
      }
    }
    else
      this->mfRecommendedWait = 0.0F;
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  float Throttler::RecommendedWait() const
  {
    return this->mfRecommendedWait;
  }
  //-------------------------------------------------------------------------//
  // End of Throttler implementation
  //*************************************************************************//
} // End of namespace MST
//***************************************************************************//
