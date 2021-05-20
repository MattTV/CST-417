/*************************************************************
 * Author:          Troy Scevers and Matt Volpe
 * Filename:		myCommands.c
 * Date Created:	4/16/2018
 * Modifications:   
 *	April 6, 2021 - Added definitions for commands get-mac, get-ip, get-gateway, get-dns, get-netmask, and send-arp.
 *  April 9, 2021 - Updated this header block.
 *	May 13, 2021  - Added prvSendUDPMessageCommand declaration for the udp-send command.
 *
 * Lab/Assignment:	UDP CLI Commands
 *
 * Overview:
 *  This document defines commands that can be executed on the
 *	device by a user using the program YAT or one with similar
 *	functionality.
 ************************************************************/
#ifndef MYCOMMANDS_H_
#define MYCOMMANDS_H_


void vRegisterMyCommands(void);

// Define Command Functions
BaseType_t prvNetStatusCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
BaseType_t prvDnsLookupCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
BaseType_t prvGetMACCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
BaseType_t prvGetIPCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
BaseType_t prvGetGatewayCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
BaseType_t prvGetDNSCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
BaseType_t prvGetNetmaskCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
BaseType_t prvOutputARPCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
BaseType_t prvArpReqCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
BaseType_t prvPingRequestCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
BaseType_t prvSendUDPMessageCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

#endif /* MYCOMMANDS_H_ */