//***************************************************************************//
// PacketArchive Class Interface
//
// Created: July 1, 2008
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

#ifndef MST_PACKET_ARCHIVE_HPP
#define MST_PACKET_ARCHIVE_HPP

//***************************************************************************//
// System Includes
#include <deque>
#include <set>
//***************************************************************************//

//***************************************************************************//

#include <MultithreadedMiller.hpp>

// Local Includes
#include "UDP.hpp"
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //*************************************************************************//
  template <class T>
  class ArchivedPacket
  {
  private:
    //***********************************************************************//
    // This is message this class will hold.
    T* mpPacket;

    void Create();
    void Clone(const ArchivedPacket& rSource);
    void Destroy();
    class MST::Mutex* mpMutex;
    //***********************************************************************//
  public:
    //***********************************************************************//
    ArchivedPacket();
    ~ArchivedPacket();
    // Copy Constructor
    ArchivedPacket(const ArchivedPacket& rSource);

    /// This method will compare ArchivedPacket and sort them based on 
    bool operator < (const ArchivedPacket& rRHS) const;
    ArchivedPacket<T>& operator=(const ArchivedPacket& rRHS);

    bool _cdecl SetPacket(const T& rMessaage);
    const T* const _cdecl GetPacket() const;
    //***********************************************************************//
  };
  //*************************************************************************//

  //*************************************************************************//
  template <class T>
  class PacketArchiverThread : public MST::Thread
  {
  private:
    //***********************************************************************//
    class MST::Mutex* mpMutex;

    unsigned long mulMaxArchivedPackets;

    /// \todo could use a const_iterator here with VC9 but not with VC8.
    std::deque<typename std::set<ArchivedPacket<typename T> >::iterator> mdArchivedPackets;
    std::set<ArchivedPacket<typename T> > mmapArchivedPacketsIndex;
    
    static void  __cdecl ArchiverThread(void* vpParameters);

    class UDP<T>* mpUDP;
    
    bool _cdecl AddPacket(const T& rPacket);
    //***********************************************************************//
  public:
    //***********************************************************************//
    PacketArchiverThread(class UDP<T>* pUDP);
    ~PacketArchiverThread();

    
    const T* const _cdecl GetArchivedPacket(const T& rNAKPacket);
    //***********************************************************************//
  }; // End of class PacketArchiverThread : public MST::Thread
  //*************************************************************************//
} // End namespace MST
//***************************************************************************//

#endif MST_PACKET_ARCHIVE_HPP
