#******************************************************************************
# NetworkingMillerSimulator
#
# Created: Feb 11, 2012
# By: Jeremy Michael Miller
#
# Copyright (c) 2006-2016 Jeremy Michael Miller. 
# Author: = "Jeremy Michael Miller"
# Copyright: = "Copyright 2005-2016, Networking Miller,  All rights reserved."
# Credits = ["Jeremy Michael Miller"]
# License: "Fair use v0.9"
# Version: "0.0.1"
# Maintainer: "Jeremy Michael Miller"
# Email: "maybe_later@mst.dnsalias.net"
# Status: "Alpha"
#*****************************************************************************

DEFAULT_HOST_IP   = "127.0.0.1"
DEFAULT_HOST_UDP_PORT = 5000
DEFAULT_HOST_TCP_PORT = 5001

DEFAULT_FORWARD_IP = "127.0.0.1"
DEFAULT_FORWARD_UDP_PORT = 6000
DEFAULT_FORWARD_TCP_PORT = 6001

#******************************************************************************
# Python Includes
import queue
import socket
import threading
import signal
import sys
#******************************************************************************


#******************************************************************************
class RouteUDP:
	def __init__(self, forward_to_address):
		self.mDropper    = PacketDropper()
		self.mDelayer    = PacketDelayer()
		self.mModifier   = PacketModifier()
		self.mOutgoing   = socket(socket.AF_INET, socket.SOCK_DGRAM)
		self.mForwardAddress = forward_to_address
	
	def SubmitMessage(self, message):
		self.mOutgoing.sendto(message, self.mForwardAddress)

#******************************************************************************

#******************************************************************************
class RouteTCP:
	def __init__(self, forward_to_address):
		self.mDropper        = PacketDropper()
		self.mDelayer        = PacketDelayer()
		self.mModifier       = PacketModifier()
		self.mOutgoing       = socket(socket.AF_INET, socket.SOCK_DGRAM)
		self.mForwardAddress = (DEFAULT_FORWARD_IP, DEFAULT_FORWARD_UDP_PORT)
		self.mQueueEvent     = threading.Event()
		self.mQueueEvent.clear()
		self.mQueue = queue()
	
	def SubmitMessage(self, message):
		self.mQueue.put(message, block=True)
		
		
	def run(self):
		while True:
			if self.mQueueEvent.wait(1000): # did not time out
				# do modifers
				while self.mQueue.empty() is False:
					self.mOutgoing.sendto(self.mQueue.get(block=True), self.mForwardAddress)

#******************************************************************************

#******************************************************************************
class PacketDropper:
	def __init__(self):
		self.fNormalizedDropRate = 0.0
		self.fCurrentDropRate		= 0.0

	def SetDropRate(self, fNormalizedDropRate):
		self.fNormalizedDropRate = fNormalizedDropRate
		
	def ForwardPacket(self):
		returnValue = 0
		if self.fCurrentDropRate > self.fNormalizedDropRate:
			returnValue = 0
		else:
			returnValue = 1
		return returnValue
#******************************************************************************

#******************************************************************************
class PacketModifier:
	def __init__(self):
		self.mfNormalizedModifyRate = 0.0
#******************************************************************************

#******************************************************************************
class PacketDelayer:
	def __init__(self):
		self.mfNormalizedDelayRate = 0.0
#******************************************************************************

#******************************************************************************
class TCP_Simulator(threading.Thread):

	def __init__(self):
		threading.Thread.__init__(self)
		self.mRouteDict = {}
		
	def run(self):
		self.mIncoming = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.mIncoming.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR,1)
		self.mIncoming.bind((DEFAULT_HOST_IP, DEFAULT_HOST_TCP_PORT))
		self.mIncoming.listen(10)
		
		while True:
			connection, address = self.mIncoming.accept()
			print(connection)
			print(address)

		return 0
#******************************************************************************

#******************************************************************************
class UDP_Simulator(threading.Thread):

	def __init__(self):
		threading.Thread.__init__(self)
		self.mRouteDict = {}
		
	def run(self):
		self.mIncoming = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		self.mIncoming.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR,1)
		self.mIncoming.bind((DEFAULT_HOST_IP, DEFAULT_HOST_UDP_PORT))
		
		while True:
			message, address = self.mIncoming.recvfrom(8192)
			print(message)
			print(address)
			if address not in self.mRouteDict:
				self.mRouteDict[address] = RouteUDP()
			self.mRouteDict[address].PassMessage(message)
				

		return 0
#******************************************************************************


def SignalInterruptHandler(signal, frame):
	print("You pressed Ctrl+C!")
	sys.exit(0)

#******************************************************************************
if __name__ == '__main__':
	signal.signal(signal.SIGINT, SignalInterruptHandler)
	
	tcp_simulator = TCP_Simulator()
	udp_simulator = UDP_Simulator()
	tcp_simulator.start()
	udp_simulator.start()
#******************************************************************************


