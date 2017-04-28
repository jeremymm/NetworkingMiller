//***************************************************************************//
// PacketBuffer Class Interface
//
// Created: Dec 13, 2006
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

#ifndef MST_PACKET_BUFFER_HPP
#define MST_PACKET_BUFFER_HPP

//***************************************************************************//
// System Includes
#include <queue>
//***************************************************************************//

#include <MultithreadedMiller.hpp>

//***************************************************************************//
// Local Includes
#include "EndPoint.hpp"
#include "Packet.hpp"
#include "ReliablePacket.hpp"
//***************************************************************************//



//***************************************************************************//
namespace MST
{
  //*************************************************************************//
  /// \brief This class is a buffer of messages that have been received or are ready 
  /// to be sent. 
  template <class T>
  class PacketBuffer
  {
  private:
    //***********************************************************************//
    //-----------------------------------------------------------------------//
    std::deque<T> mdPackets;
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    /// The below mutex is used to ensure this %PacketBuffer avoids race
    /// conditions in an multi threaded environment.
    class MST::Mutex* mpInternalMutex;
    /// The below event will be triggered when a new message is inserted in to the
    /// %PacketBuffer.
    class MST::Event* mpInsertEvent;
    //-----------------------------------------------------------------------//
    
    //-----------------------------------------------------------------------//
    unsigned long mulMaxPackets;
    //-----------------------------------------------------------------------//
    //***********************************************************************//
  public:
    //***********************************************************************//
    PacketBuffer();
    PacketBuffer(unsigned long ulMaxPackets);
    /// Currently the destructor cleans up the aging thread for this class
    /// and does nothing else
    virtual ~PacketBuffer();

    /// This method locks the message buffer. If this method is called by another
    /// thread this method will block until that thread releases the Lock by 
    /// calling unlock
    /// \return Returns true if the Lock was successful. Otherwise it returns false.
    bool _cdecl Lock();
    /// \overload bool Lock(unsigned long ulTimeout)
    /// This method locks the message buffer. If this method is called by another
    /// thread this method will block until that thread releases or until the number
    /// milliseconds passed into the method have passed.  If the method timeouts it 
    /// will return false.
    /// \param ulTimeout [in]
    /// This unsigned long contains the amount of time this method will wait for
    /// a Lock in milliseconds.
    /// \return Returns true if the Lock was successful. Otherwise it returns false.
    bool _cdecl Lock(unsigned long ulTimeout);
    /// This method unlocks and Locks that were attained.  This method must be called
    /// after Lock and called and after modification to the %PacketBuffer are complete.
    void _cdecl Unlock();
    /// This method will block until and insertion event occurs for this %PacketBuffer
    /// or until a time period has expired. This method is helpful for detecting 
    /// if there are Packets to processes in the %PacketBuffer.
    /// \param ulTimeout [in]
    /// This unsigned long contains the amount of time this method will wait, 
    /// in milliseconds for a insertion event to occur.
    bool _cdecl WaitForInsertion(unsigned long ulTimeout);
    class MST::Event& _cdecl InsertionEvent() const { return *mpInsertEvent; }


    /// This method gets the pointer the highest priority Packet in the 
    /// %PacketBuffer.
    /// \return A pointer to the highest priority Packet.
    const T& _cdecl Front() const;
    /// This method removes the highest priority Packet from the %PacketBuffer.
    void _cdecl Pop();

    /// This method will add an Packet to the %PacketBuffer
    /// \param rPacket [in]
    /// This parameter is a properly formated Packet that is ready to be sent
    /// or has been received.
    /// \todo update comments
    /// \return
    /// The method will return true if the message was added to the message buffer.
    /// Otherwise, it will return false.
    bool _cdecl Push(const T& rPacket, bool bSignalInsertion);
    
    /// This method returns the number of messages in the %PacketBuffer.
    /// \return Number of messages in this %PacketBuffer.
    unsigned long _cdecl Length() const;
    void _cdecl Length(unsigned long ulLength);
    //***********************************************************************//
  }; // End of class PacketBuffer
  //*************************************************************************//
} // End of namespace MST
//***************************************************************************//

#endif MST_PACKET_BUFFER_HPP
