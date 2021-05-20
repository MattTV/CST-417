/*************************************************************
 * Author:          Matt Volpe
 * Filename:		myUDPServer.c
 * Date Created:	May 6, 2021
 * Modifications:   
 *
 * Lab/Assignment:	Lab 5 - UDP Client and Server
 *
 * Overview:
 *	This program implements a UDP Server that talks to a client.
 ************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <ctype.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_Sockets.h"

#include "myUDPServer.h"

static uint16_t uiMyUsedStackSize = 0;

/************************************************************************************************
* Purpose: StartMyUdpServerTasks creates a new task to run a MyUDPServerTask.
*
* Precondition:
*	usStackSize should be nonnegative?
*
* Postcondition:
*	uiMyUsedStackSize will equal usStackSize
*	A MyUDPServerTask task will be created.
************************************************************************************************/
void StartMyUdpServerTasks(uint16_t usStackSize, UBaseType_t uxPriority)
{
	xTaskCreate(MyUDPServerTask, "MyUDPServer", usStackSize, NULL, uxPriority + 1, NULL);

	uiMyUsedStackSize = usStackSize;
}
/************************************************************************************************
* Purpose: MyUDPServerTask will create a UDP server that receives data and prints it out to the UDP debug console.
*
* Precondition:
*	None
*
* Postcondition:
*	Received data will be printed to the UDP debug console.
************************************************************************************************/
void MyUDPServerTask(void * pvParameters)
{
	// Create one socket
	Socket_t sock;
	
	// Create Destination and Source Address Structs
	struct freertos_sockaddr xBindAddress;
	struct freertos_sockaddr srcaddress;
	
	// Open the socket with settings for UDP
	sock = FreeRTOS_socket(FREERTOS_AF_INET, FREERTOS_SOCK_DGRAM, FREERTOS_IPPROTO_UDP);
	
	// If the socket opened properly
	if (sock != FREERTOS_INVALID_SOCKET)
	{
		// Bind to the UDP port number
		xBindAddress.sin_port = FreeRTOS_htons(MY_UDP_PORT_NUMBER);
		if (FreeRTOS_bind(sock, &xBindAddress, sizeof(xBindAddress)) == 0)
		{
			// received tracks the number of bytes received
			int received = 0;
			// buffer stores the data, with the maximum message length
			char buffer[ipconfigUDP_MSS];
			
			// Infinitely...
			while(1)
			{
				// Clear the buffer
				memset(buffer, 0, ipconfigUDP_MSS);
				
				// Receive data and store the number of bytes received
				received = FreeRTOS_recvfrom(sock, buffer, ipconfigUDP_MSS, 0, &srcaddress, sizeof(srcaddress));
				
				// If data was received, print it to DTERM.
				if (received > 0)
				{
					FreeRTOS_printf( ( "Message: %s\n", buffer ) );
				}
			}			
		}
		
		// Close the socket once finished.
		FreeRTOS_closesocket(sock);
	}
}