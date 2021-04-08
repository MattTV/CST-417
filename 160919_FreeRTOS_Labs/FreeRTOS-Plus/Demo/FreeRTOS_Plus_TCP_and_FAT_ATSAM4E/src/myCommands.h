/*************************************************************
 * Author:          Troy Scevers
 * Filename:		myCommands.c
 * Date Created:	4/16/2018
 * Modifications:   
 *
 * Lab/Assignment:	UDP CLI Commands
 *
 * Overview:
 *  
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

#endif /* MYCOMMANDS_H_ */