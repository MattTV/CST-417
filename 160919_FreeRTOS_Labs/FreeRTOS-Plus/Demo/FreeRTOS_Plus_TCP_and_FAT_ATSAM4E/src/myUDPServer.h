/*************************************************************
 * Author:          Matt Volpe
 * Filename:		myUDPServer.h
 * Date Created:	May 6, 2021
 * Modifications:   
 *
 * Lab/Assignment:	Lab 5 - UDP Client and Server
 *
 * Overview:
 *	This program implements a UDP Server that talks to a client.
 ************************************************************/

#ifndef MYUDPSERVER_H_
#define MYUDPSERVER_H_

#include "FreeRTOS_IP.h"

#define MY_UDP_PORT_NUMBER	9930
#define ipconfigUDP_MSS		(ipconfigNETWORK_MTU - ipSIZE_OF_IPv4_HEADER - ipSIZE_OF_UDP_HEADER)

void StartMyUdpServerTasks(uint16_t usStackSize, UBaseType_t uxPriority);
void MyUDPServerTask(void * pvParameters);

#endif /* MYUDPSERVER_H_ */