
#include <WinSock2.h>

#include <iostream>
using namespace std;

#include <fcntl.h>
//---------------------------------------------------------------------------//
void LosePackets2()
{
  //unsigned long ulNumberOfTransmissions = 0UL;
  //class ReliablePacket* pRecvPacket = new ReliablePacket();
  //UDPServer<ReliablePacket> oUDPServer;

  //oUDPServer.Listen(UDP_SERVER_PORT);
  //
  //cout << "UDP Relay Server started on port: " << UDP_SERVER_PORT << ".\r\n";
  //cout << "UDP Relay Server connected to : " << gcpServerIP << " port: " << gusServerPort << ".\r\n";
  //oUDPClient.Connect(gusServerPort, gcpServerIP);

  //EndPoint oSourceEndPoint;

  //while(1)
  //{
  //  if(oUDPServer.Recv(pRecvPacket) > -1)
  //  {
  //    if(rand() % 3)
  //    {
  //      //pRecvPacket->Destination(pRecvPacket->Source().IP(), pRecvPacket->Source().Port());
  //      oSourceEndPoint = pRecvPacket->Source();
  //      pRecvPacket->Source(oUDPClient.LocalIP(), oUDPClient.LocalPort());
  //      pRecvPacket->Destination(oUDPClient.RemoteIP(), oUDPClient.RemotePort());
  //      oUDPClient.Send(pRecvPacket);
  //      cout << "To Server Relayed Seq: " << pRecvPacket->GetSequence() << "\r\n";
  //    }
  //    else
  //      cout << "To Server DROPPED Seq: " << pRecvPacket->GetSequence() << "\r\n";
  //  }

  //  if(oUDPClient.Recv(pRecvPacket) > -1)
  //  {
  //    if(rand() % 3)
  //    {
  //      pRecvPacket->Source(oUDPServer.LocalIP(), oUDPServer.LocalPort());
  //      pRecvPacket->Destination(oSourceEndPoint);
  //      oUDPServer.Send(pRecvPacket);
  //      cout << "To Client Relayed Seq: " << pRecvPacket->GetSequence() << "\r\n";
  //    }
  //    else
  //      cout << "To Client DROPPED Seq: " << pRecvPacket->GetSequence() << "\r\n";
  //  }

  //}

  WSAData* mpwsaData = new WSAData();
  long lResult;
 
    lResult = WSAStartup(MAKEWORD(2,2), mpwsaData);
    //assert(lResult == NO_ERROR);


  int csockfd;
  struct sockaddr_in cservaddr;
  char crecvline[8192];

  csockfd=socket(AF_INET,SOCK_DGRAM,0);

  ZeroMemory(&cservaddr,sizeof(cservaddr));
  cservaddr.sin_family = AF_INET;
  cservaddr.sin_addr.s_addr=inet_addr("192.168.0.2");
  cservaddr.sin_port=htons(12345);


  int ssockfd, n;
  struct sockaddr_in sservaddr,scliaddr;
  int len;
  char smesg[8192];

  ssockfd=socket(AF_INET,SOCK_DGRAM,0);

  unsigned long ulBlock = 1;
  ioctlsocket(ssockfd, FIONBIO, &ulBlock);
  ioctlsocket(csockfd, FIONBIO, &ulBlock);




  ZeroMemory(&sservaddr,sizeof(sservaddr));
  sservaddr.sin_family = AF_INET;
  sservaddr.sin_addr.s_addr=htonl(INADDR_ANY);
  sservaddr.sin_port=htons(12345);
  bind(ssockfd,(struct sockaddr *)&sservaddr,sizeof(sservaddr));

  ioctlsocket(ssockfd, FIONBIO, &ulBlock);
  ioctlsocket(csockfd, FIONBIO, &ulBlock);

  for (;;)
  {
    len = sizeof(scliaddr);
    n = recvfrom(ssockfd,smesg,8192,0,(struct sockaddr *)&scliaddr,&len);

    if(n > 0)
    {
      if(rand() % 4)
      {
        cout << "relaying to server" << endl;
        sendto(csockfd,smesg,n,0, (struct sockaddr *)&cservaddr,sizeof(cservaddr));
      }
      else
      {
        cout << "DROPPING to server" << endl;
      }

    }

    n=recvfrom(csockfd,crecvline,8192,0,NULL, NULL);
    if(n > 0)
    {
      cout << "relaying to client" << endl;
      sendto(ssockfd ,crecvline,n,0,(struct sockaddr *)&scliaddr,sizeof(scliaddr));
    }

  }


}