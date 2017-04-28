//***************************************************************************//
// TCP Class Implementation
//
// Created: Feb 20, 2006
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
#include <assert.h> // For assertions
#include <iostream> // For access to cerr
#include <sstream>  // For stringstreams
#ifdef WIN32
#include <WinSock2.h>
#endif
//***************************************************************************//

//***************************************************************************//
// Local Includes
#include "TCP.hpp"
#include "Packet.hpp"
//***************************************************************************//

//***************************************************************************//
/// This is the size used to create buffers for sending a recving files
/// \todo Make this value availble to in the API.  This value will also be used
/// when data compression is added
#define SEND_RECV_FILE_BUFFER_SIZE (2 * 1048576) // 2MB
//***************************************************************************//

//***************************************************************************//
using namespace std;
//***************************************************************************//

//***************************************************************************//
namespace MST
{
  //*************************************************************************//
  // Start of TCP implementation
  //-------------------------------------------------------------------------//
  // Simply creates a socket to be used with the TCP transport protocol.
  TCP::TCP() : Socket(CONNECTION_ORIENTATED_SOCKET)
  {
    TCP::CreateDescriptor();
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  // Destructor
  TCP::~TCP()
  {
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  unsigned long TCP::CreateDescriptor()
  {
    //-----------------------------------------------------------------------//
    // Create a socket for use with the TCP transport protocol.
    if((this->mSocket = static_cast<unsigned long>(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))) == ULONG_MAX)
      cerr << "Failed to create TCP Socket\r\n";
    //-----------------------------------------------------------------------//

    int iOptionValue = 1;
    setsockopt(this->mSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&iOptionValue, sizeof(iOptionValue));

    //-----------------------------------------------------------------------//
    // Throw an assertion if we could not create the socket.
    assert(this->mSocket != SOCKET_ERROR);
    //-----------------------------------------------------------------------//

    return static_cast<unsigned long>(this->mSocket);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  unsigned long TCP::SendFile(const char* cpFileName, class Socket* const pSocket)
  {
    //-----------------------------------------------------------------------//
    // Create and input file stream and attempt to attach a binary file
    // to that stream.
    FILE* ifp;
    errno_t iError = fopen_s(&ifp, cpFileName, "rb");
    //-----------------------------------------------------------------------//

    /// \todo If iError == 2 then report there is no such file

    //-----------------------------------------------------------------------//
    // Throw and assertion is the file failed to open
    assert(iError == 0);
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    // This is the return value for this method
    // This unsinged long holds the amount of data that was sent over the 
    // network
    unsigned long ulTotalSent = 0;
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    // If the file was successfully opened..
    if(iError == 0)
    {
      // ..continue to read from this file and send its contents over 
      // a established TCP connection

      //---------------------------------------------------------------------//
      // If an external socket was passed in use that instead of the socket that
      // this class was inherited from
      class Socket* const pConnectedSocket = (pSocket) ? pSocket : this;
      //---------------------------------------------------------------------//

      //---------------------------------------------------------------------//
      if(pConnectedSocket->Valid())
      {
        //-------------------------------------------------------------------//
        // Create a buffer to read the data from the file into this
        // buffer then send the contents of this buffer over the network.
        char* cpBuffer = new char[SEND_RECV_FILE_BUFFER_SIZE];
        //-------------------------------------------------------------------//

        //-------------------------------------------------------------------//
        unsigned long ulCurrentPosition  = 0;
        //-------------------------------------------------------------------//

        //-------------------------------------------------------------------//
        // This variable is used to store how many bytes were sent 
        // over the network for a given loop iteration.
        long lNumberOfBytesSent = 0;
        //-------------------------------------------------------------------//

        //-------------------------------------------------------------------//
        // Read from the file while there is data left to read
        class Packet* pPacket = new Packet();
        while(!feof(ifp))
        {
          // Attempt to read a full buffer of data from the file
          unsigned long ulDataRead = static_cast<unsigned long>(fread(cpBuffer, sizeof(char), SEND_RECV_FILE_BUFFER_SIZE, ifp));
          
          stringstream sstrCommand;
          sstrCommand << '<' << ulDataRead << '>';

          pPacket->SetPayload(sstrCommand.str().c_str(), static_cast<unsigned long>(sstrCommand.str().size()));
          lNumberOfBytesSent = pConnectedSocket->Send(*pPacket);
          pPacket->Clear();

          assert(static_cast<unsigned long>(lNumberOfBytesSent) == sstrCommand.str().size());

          if(lNumberOfBytesSent > 0)
            ulTotalSent += static_cast<unsigned long>(lNumberOfBytesSent);
        } // End of while(feof(ifp))
        //-------------------------------------------------------------------//

        //-------------------------------------------------------------------//
        // We are all done with the input file stream and the buffer
        // so we can get rid of them
        fclose(ifp);
        delete [] cpBuffer;
        delete pPacket;
        //-------------------------------------------------------------------//
      } // End of if(oConnection.Valid())
      //---------------------------------------------------------------------//
    } // End of if(iError == 0)
    //-----------------------------------------------------------------------//
    // Return how many bytes were sent over the network
    return ulTotalSent;
  } // End of unsigned long  TCP::SendFile(const char* cpFileName)
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  unsigned long TCP::SendFile(const std::string& strFileName, class Socket* const pSocket)
  {
    return this->SendFile(strFileName.c_str(), pSocket);
  }
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  unsigned long TCP::RecvFile(const char* cpFileName, class Socket* const pSocket)
  {
    //-----------------------------------------------------------------------//
    // Create and file stream to write the data we recieve from the network.
    FILE* ofp;
    errno_t iError = fopen_s(&ofp, cpFileName, "wb");
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    // Throw an assertion if the file failed to open.
    assert(iError == 0);
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    // This unsigned long holds the return value which is how much we have read 
    // from the network
    unsigned long ulTotalRead = 0;
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    // If the output file is open
    if(iError == 0)
    {
      //---------------------------------------------------------------------//
      // If an external socket was passed in use that instead of the socket that
      // is this class was inherited from
      class Socket* const pConnectedSocket = (pSocket) ? pSocket : this;
      //---------------------------------------------------------------------//

      //---------------------------------------------------------------------//
      if(pConnectedSocket->Valid())
      {
        //-------------------------------------------------------------------//
        // Now we can get the data from the network and write it to a file
        long lBytesRead = 0;
        class Packet* pPacket = new Packet();
        while(lBytesRead >= 0)
        {
          //-------------------------------------------------------------------//
          // This bool is used to see if we found the delimiter in 
          // the command that holds the file size
          bool bDelimiterFound;
          // This string will store the information about the file size
          string strBlockSizeCommand;
          //-------------------------------------------------------------------//

          //-------------------------------------------------------------------//
          // read from the socket to get the size of the incoming block
          pConnectedSocket->Recv(pPacket);

          pPacket->FillString(strBlockSizeCommand);
          pPacket->Clear();

          if(strBlockSizeCommand.find(">") != string::npos)
            bDelimiterFound = true;
          else
            bDelimiterFound = false;

          if(bDelimiterFound && (strBlockSizeCommand.size() > 3))
          {
            //---------------------------------------------------------------//
            // Remove the '<' and the '>' from the command
            strBlockSizeCommand.erase(strBlockSizeCommand.begin());
            strBlockSizeCommand.erase(strBlockSizeCommand.end() - 1);
            //---------------------------------------------------------------//

            //---------------------------------------------------------------//
            // Convert the string into a long
            unsigned long ulIncomingBlockSize = static_cast<unsigned long>(atol(strBlockSizeCommand.c_str()));
            //---------------------------------------------------------------//

            //---------------------------------------------------------------//
            assert(ulIncomingBlockSize <= SEND_RECV_FILE_BUFFER_SIZE);
            //---------------------------------------------------------------//

            //---------------------------------------------------------------//
            if(ulIncomingBlockSize <= SEND_RECV_FILE_BUFFER_SIZE)
            {
              lBytesRead = pConnectedSocket->Recv(pPacket);

              if(lBytesRead > 0)
              {
                fwrite(pPacket->Payload(), sizeof(char), static_cast<unsigned long>(lBytesRead), ofp);
                ulTotalRead += static_cast<unsigned long>(lBytesRead);
              }

              pPacket->Clear();
            }
            //---------------------------------------------------------------//
          }
          else
          {
            break;
          }

        }// End of while(lBytesRead >= 0)
        //-------------------------------------------------------------------//

        delete pPacket;

      } // End of if(pConnectedSocket->Valid())
      //---------------------------------------------------------------------//

      //---------------------------------------------------------------------//
      // We are done with the output file stream so we can close it
      fclose(ofp);
      //---------------------------------------------------------------------//
    } // End of if(iError == 0)
    //-----------------------------------------------------------------------//

    //-----------------------------------------------------------------------//
    // Return the total amount of data in bytes that we received from the network
    return ulTotalRead;
    //-----------------------------------------------------------------------//
  } // End of unsigned long TCP::RecvFile(const char* cpFileName, class Socket* const pSocket)
  //-------------------------------------------------------------------------//

  //-------------------------------------------------------------------------//
  unsigned long TCP::RecvFile(const std::string& strFileName, class Socket* const pSocket)
  {
    return this->RecvFile(strFileName.c_str(), pSocket);
  }
  //-------------------------------------------------------------------------//
  // End of TCP implementation
  //*************************************************************************//
} // End of namespace MST
//***************************************************************************//
