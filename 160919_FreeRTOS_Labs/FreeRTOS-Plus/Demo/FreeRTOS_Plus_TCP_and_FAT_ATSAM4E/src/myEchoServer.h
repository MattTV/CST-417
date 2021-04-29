/*************************************************************
 * Author:          Matt Volpe
 * Filename:		myEchoServer.h
 * Date Created:	April 29, 2021
 * Modifications:   
 *
 * Lab/Assignment:	Lab 4 - TCP Echo Server
 *
 * Overview:
 *  This function implements an echo server that returns in
 *	uppercase what was sent to it. If the name "Matt" is sent,
 *	a greeting will be sent back. A single '.' ends the session.
 ************************************************************/

#ifndef MYECHOSERVER_H_
#define MYECHOSERVER_H_

#include <stdint.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

#define myTCPechoSHUTDOWN_DELAY	(pdMS_TO_TICKS(5000))
#define myTCPechoPORT_NUMBER	8080

void StartMyEchoTCPServerTasks(uint16_t usStackSize, UBaseType_t uxPriority);
void MyEchoServerListeningTask(void *pvParameters);
void MyEchoServerInstance(void *pvParameters);

#endif /* MYECHOSERVER_H_ */