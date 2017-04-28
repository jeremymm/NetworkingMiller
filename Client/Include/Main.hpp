
//***************************************************************************//
// Main header
// 
// This file holds global defines to be used throughout the program
//
// Created: June 17, 2008
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

#ifndef CLIENT_MAIN_HPP
#define CLIENT_MAIN_HPP

#define APP_VERSION 0.01F
#define INVALID_INPUT_STRING  "Invalid Input. Please enter the letter or number contained\r\nwithing the backets [ ].\r\n"
#define HEADER_STRING "NetworkingMiller Client Tester"
#define CONTACT_STRING "Contact: Jeremy Miller\r\nEmail maybe_later@mst.dnsalias.net\r\n";

extern bool gbTesting;
extern char* gcpServerIP;
extern char* gcpServerPort;
extern unsigned short gusServerPort;

#endif CLIENT_MAIN_HPP