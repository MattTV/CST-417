/*************************************************************
 * Author:          Matt Volpe
 * Filename:		myEchoServer.c
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

#include "myEchoServer.h"

static uint16_t usUsedStackSize = 0;

/************************************************************************************************
* Purpose: StartMyEchoTCPServerTasks creates a listening task in the operating system and saves the stack size for future use.
*
* Precondition:
*	usStackSize should be nonnegative?
*
* Postcondition:
*	usUsedStackSize will equal usStackSize
*	A new task will be created that calls MyEchoServerListeningTask?
************************************************************************************************/
void StartMyEchoTCPServerTasks(uint16_t usStackSize, UBaseType_t uxPriority)
{
	// Create the TCP echo server.
	xTaskCreate(MyEchoServerListeningTask, "ServerListener", usStackSize, NULL, uxPriority + 1, NULL);
	
	// Save the stack size value to use when other tasks are created in the program.
	usUsedStackSize = usStackSize;
}
/************************************************************************************************
* Purpose: MyEchoServerListeningTask listens for an incoming telnet connection, and then starts an task to create a MyEchoServerInstance.
*
* Precondition:
*	None
*
* Postcondition:
*	A MyEchoServerListeningTask will be created when a connection is accepted.
************************************************************************************************/
void MyEchoServerListeningTask(void *pvParameters)
{
	struct freertos_sockaddr xClient, xBindAddress;
	Socket_t xListeningSocket, xConnectedSocket;
	socklen_t xSize = sizeof(xClient);
	static const TickType_t xReceiveTimeOut = portMAX_DELAY;
	const BaseType_t xBacklog = 20;
	
	#if(ipconfigUseTCP_WIN == 1)
		WinProperties_t xWinProps;
		
		// Fill in the buffer and window sizes that will be used by the socket.
		xWinProps.lTxBufSize = ipconfigTCP_TX_BUFFER_LENGTH;
		xWinProps.lTxWinSize = configECHO_SERVER_TX_WINDOW_SIZE;
		xWinProps.lRxBufSize = ipconfigTCP_RX_BUFFER_LENGTH;
		xWinProps.lRxWinSize = configECHO_SERVER_RX_WINDOW_SIZE;
	#endif
	
	// Prevent compiler warnings.
	(void)pvParameters;
	
	// Attempt to open the socket.
	xListeningSocket = FreeRTOS_socket(FREERTOS_AF_INET, FREERTOS_SOCK_STREAM, FREERTOS_IPPROTO_TCP);
	configASSERT(xListeningSocket != FREERTOS_INVALID_SOCKET);
	
	// Set a timeout so accept() will just wait for a connection.
	FreeRTOS_setsockopt(xListeningSocket, 0, FREERTOS_SO_RCVTIMEO, &xReceiveTimeOut, sizeof(xReceiveTimeOut));
	
	// Set the window and buffer sizes.
	#if(ipconfigUseTCP_WIN == 1)
	{
		FreeRTOS_setsockopt(xListeningSocket, 0, FREERTOS_SO_WIN_PROPERTIES, (void*)&xWinProps, sizeof(xWinProps));
	}
	#endif
	
	// Bind the socket to the port that the client task will send to.
	xBindAddress.sin_port = myTCPechoPORT_NUMBER;
	xBindAddress.sin_port = FreeRTOS_htons(xBindAddress.sin_port);
	FreeRTOS_bind(xListeningSocket, &xBindAddress, sizeof(xBindAddress));
	
	// Listen for incoming connections.
	FreeRTOS_listen(xListeningSocket, xBacklog);
	
	for (;;)
	{
		// Wait for a client to connect.
		xConnectedSocket = FreeRTOS_accept(xListeningSocket, &xClient, &xSize);
		configASSERT(xConnectedSocket != FREERTOS_INVALID_SOCKET);
		
		// Spawn a task to handle the connection.
		xTaskCreate(MyEchoServerInstance, "EchoServer", usUsedStackSize, (void*)xConnectedSocket, tskIDLE_PRIORITY, NULL);
	}
}
/************************************************************************************************
* Purpose: MyEchoServerInstance takes a string from a telnet connection, converts it to all uppercase, and echos it back. If "matt" is received, a welcome message will be sent. A '.' ends the session.
*
* Precondition:
*	None
*
* Postcondition:
*	Strings will be echoed back in uppercase
*	If "matt" is read, a greeting will be sent back.
*	If a '.' is read, the instance ends.
************************************************************************************************/
void MyEchoServerInstance(void *pvParameters)
{
	int32_t lBytes, lSent, lTotalSent;
	Socket_t xConnectedSocket;
	static const TickType_t xReceiveTimeOut = pdMS_TO_TICKS(5000);
	static const TickType_t xSendTimeOut = pdMS_TO_TICKS(5000);
	TickType_t xTimeOnShutdown;
	uint8_t *pucRxBuffer;
	
	xConnectedSocket = (Socket_t)pvParameters;
	
	// Try to create the buffer to receive the string that is echoed back.
	pucRxBuffer = (uint8_t*)pvPortMalloc(ipconfigTCP_MSS);
	
	// If the buffer was created
	if (pucRxBuffer != NULL)
	{
		// Set socket options for sending and receiving.
		FreeRTOS_setsockopt(xConnectedSocket, 0, FREERTOS_SO_RCVTIMEO, &xReceiveTimeOut, sizeof(xReceiveTimeOut));
		FreeRTOS_setsockopt(xConnectedSocket, 0, FREERTOS_SO_SNDTIMEO, &xSendTimeOut, sizeof(xReceiveTimeOut));
		
		int repeat = 1;
		
		// Until repeat is changed to 0.
		while (repeat)
		{
			// Zero out the receive array so there is null at the end of the string.
			memset(pucRxBuffer, 0x00, ipconfigTCP_MSS);
			
			// Receive data on the socket.
			lBytes = FreeRTOS_recv(xConnectedSocket, pucRxBuffer, ipconfigTCP_MSS, 0);
			
			// If data was received, change it and echo it back.
			if (lBytes >= 0)
			{
				lSent = 0;
				lTotalSent = 0;
				
				// If a period is NOT all that was sent.
				if (strcmp(".\r\n", pucRxBuffer))
				{
					// If the name Matt was sent.
					if (strcmp("matt\r\n", pucRxBuffer) == 0)
					{
						// Put the new string in the same buffer.
						strcpy(pucRxBuffer, "Sup kid\n");
						// Change the length.
						lBytes = strlen(pucRxBuffer);
						
						// Call send() until all the data has been sent.
						while ((lSent >= 0) && (lTotalSent < lBytes))
						{
							// Send "Sup kid".
							lSent = FreeRTOS_send(xConnectedSocket, pucRxBuffer, lBytes - lTotalSent, 0);
							lTotalSent += lSent;
						}
					}
					// If anything else was sent.
					else
					{
						// Call send() until all the data has been sent.
						while ((lSent >= 0) && (lTotalSent < lBytes))
						{
							// Send the string in uppercase.
							lSent = FreeRTOS_send(xConnectedSocket, strupr(pucRxBuffer), lBytes - lTotalSent, 0);
							lTotalSent += lSent;
						}
					}
				}
				else
				{
					// Change this to 0 if a period was encountered as a lone message, so the loop stops and a shutdown can begin.
					repeat = 0;
				}
			}
		}
	}
	
	// Initiate a shutdown in case it has not already been initiated.
	FreeRTOS_shutdown(xConnectedSocket, FREERTOS_SHUT_RDWR);
	
	// Wait for the shutdown to take effect, indiciated by FreeRTOS_recv() returning an error.
	xTimeOnShutdown = xTaskGetTickCount();
	do 
	{
		if (FreeRTOS_recv(xConnectedSocket, pucRxBuffer, ipconfigTCP_MSS, 0) < 0)
		{
			break;
		}
	} while ((xTaskGetTickCount() - xTimeOnShutdown) < myTCPechoSHUTDOWN_DELAY);
	
	// Finished with socket, buffer, and the task.
	vPortFree(pucRxBuffer);
	FreeRTOS_closesocket(xConnectedSocket);
	
	vTaskDelete(NULL);
}