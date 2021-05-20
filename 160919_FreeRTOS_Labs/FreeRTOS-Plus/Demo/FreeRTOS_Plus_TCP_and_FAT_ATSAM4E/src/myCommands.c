/*************************************************************
 * Author:          Troy Scevers and Matt Volpe
 * Filename:		myCommands.c
 * Date Created:	4/16/2018
 * Modifications:   
 *	April 6, 2021 - Added commands get-mac, get-ip, get-gateway, get-dns, get-netmask, and send-arp.
 *	April 9, 2021 - Added documentation for new commands and updated this header block.
 *	May 13, 2021  - Added and documented the udp-send command and the functions to make it work.
 *	May 20, 2021  - Completed the udp-send command implementation.
 *
 * Lab/Assignment:	UDP CLI Commands
 *
 * Overview:
 *  This document defines commands that can be executed on the
 *	device by a user using the program YAT or one with similar
 *	functionality.
 ************************************************************/
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* FreeRTOS+CLI includes. */
#include "FreeRTOS_CLI.h"

/* FreeRTOS+TCP includes, just to make the stats available to the CLI
   commands. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

/* My Includes */
#include "myCommands.h"

#define MY_UDP_PORT_NUMBER	9930

// Command Definitions
static const CLI_Command_Definition_t xNetUp =
{
    "net-status", /* The command string to type. */
    "\r\nnet-status:\r\n Calls FreeRTOS_isNetworkUp()\r\n",
    prvNetStatusCommand, /* The function to run. */
    0 /* No parameters are expected. */
};
static const CLI_Command_Definition_t xDNSLookup =
{
    "dns-lookup",	// The command name.
    "\r\ndns-lookup <host-name>:\r\n For Example dns-lookup www.example.com\r\n",	// The help string.
    prvDnsLookupCommand,	// The function to run.
    1	// The number of parameters expected.
};
static const CLI_Command_Definition_t xGetMACAddress =
{
	"get-mac",	// The command name.
	"\r\nget-mac:\r\n Displays the MAC Address of the NIC.",	// The help string.
	prvGetMACCommand,	// The function to run.
	0	// The number of parameters expected.
};
static const CLI_Command_Definition_t xGetIP =
{
	"get-ip",	// The command name.
	"\r\nget-ip:\r\n Displays the IP address of the NIC.",	// The help string.
	prvGetIPCommand,	// The function to run.
	0	// The number of parameters expected.
};
static const CLI_Command_Definition_t xGetGateway =
{
	"get-gateway",	// The command name.
	"\r\nget-gateway:\r\n Displays the IP Address of the Gateway.",	// The help string.
	prvGetGatewayCommand,	// The function to run.
	0	// The number of parameters expected.
};
static const CLI_Command_Definition_t xGetDNSServer =
{
	"get-dns",	// The command name.
	"\r\nget-dns:\r\n Displays the IP Address of the DNS Server.",	// The help string.
	prvGetDNSCommand,	// The function to run.
	0	// The number of parameters expected.
};
static const CLI_Command_Definition_t xGetNetmask =
{
	"get-netmask",	// The command name.
	"\r\nget-netmask:\r\n Displays the Net Mask.",	// The help string.
	prvGetNetmaskCommand,	// The function to run.
	0	// The number of parameters expected.
};
static const CLI_Command_Definition_t xOutputARPRequest =
{
	"send-arp",	// The command name.
	"\r\nsend-arp <ip-address>:\r\n Sends an ARP request to the provided IP address.",	// The help string.
	prvOutputARPCommand,	// The function to run.
	1	// The number of parameters expected.
};
static const CLI_Command_Definition_t xSendUDPMessage =
{
	"udp-send",	// The command name
	"\r\nudp-send <host> <message>\r\nExample: udp-send 10.101.131.58 \"Hello World\"\r\n",	// The help string
	prvSendUDPMessageCommand,	// The function to run
	2	// The number of parameters expected
};

/************************************************************************************************
* Purpose: vRegisterMyCommands uses an operating system function to register user-defined commands with the operating system.
*
* Precondition:
*	None
*
* Postcondition:
*	net-status, dns-lookup, get-mac, get-ip, get-gateway, get-dns, get-netmask, and send-arp will be registered with the OS.
************************************************************************************************/
void vRegisterMyCommands(void)
{
	// Add the commands into the operating system.
    FreeRTOS_CLIRegisterCommand( &xNetUp );
    FreeRTOS_CLIRegisterCommand( &xDNSLookup );
	FreeRTOS_CLIRegisterCommand( &xGetMACAddress );
	FreeRTOS_CLIRegisterCommand( &xGetIP );
	FreeRTOS_CLIRegisterCommand( &xGetGateway );
	FreeRTOS_CLIRegisterCommand( &xGetDNSServer );
	FreeRTOS_CLIRegisterCommand( &xGetNetmask );
	FreeRTOS_CLIRegisterCommand( &xOutputARPRequest );
	FreeRTOS_CLIRegisterCommand( &xSendUDPMessage );
}

// The commands themselves.
/************************************************************************************************
* Purpose: prvOutputARPCommand takes in an IP address to send an ARP Request to, sends it, and prints a message of confirmation or failure.
*
* Precondition:
*	An IP address in 111.111.111.111 format must be provided as an argument.
*
* Postcondition:
*	An ARP Request will have gone out to the address, if provided.
*	A message of confirmation will have been written to pcWriteBuffer.
*	Upon failure, no ARP Request will have been sent, and pcWriteBuffer will have a message of failure.
************************************************************************************************/
BaseType_t prvOutputARPCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	( void ) xWriteBufferLen;	// Remove warnings for unused variables.
	
	// Get the length of the parameter string.
	BaseType_t lParameterStringLength;
	char* pcParameter = (char*) FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParameterStringLength);
	
	pcWriteBuffer[0] = '\0';	// Start with an empty write buffer.
	
	// If the pcParameter exists...
	if (pcParameter != NULL)
	{
		// ...convert the common 111.111... format address from the user into an integer format...
		uint32_t converted = FreeRTOS_inet_addr(pcParameter);
		// ...call on the operating system to send an ARP request to the address, in the converted integer format...
		FreeRTOS_OutputARPRequest(converted);
		
		// ...write a message that the ARP request was sent out, and give the IP address entered.
		sprintf(pcWriteBuffer, "ARP Request sent out on %s", pcParameter);
	}
	// If the pcParameter was not given...
	else
		// ...tell the user that this function failed.
		sprintf(pcWriteBuffer, "ARP Output failed: Could not retrieve parameter\r\n");

	return pdFALSE;	// Return the same as other commands.
}
/************************************************************************************************
* Purpose: prvGetNetmaskCommand prints the Net Mask to the pcWriteBuffer in a formatted string.
*
* Precondition:
*	None
*
* Postcondition:
*	The Net Mask will have been written to pcWriteBuffer in a formatted string.
************************************************************************************************/
BaseType_t prvGetNetmaskCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	( void ) xWriteBufferLen;	// Remove warnings for unused variables.
	( void ) pcCommandString;	// Remove warnings for unused variables.

	pcWriteBuffer[0] = '\0';	// Start with an empty write buffer.

	const uint32_t ret = FreeRTOS_GetNetmask();	// Execute the operating system's function and save its result.

	char converted[16];					// A string convert the address into the common 111.111... format from the returned integer.
	converted[15] = '\0';				// Place a null character at the end of the string.
	FreeRTOS_inet_ntoa(ret, converted);	// Convert the integer format into the readable 111.111... format using the operating system's built-in function.

	// Write the Netmask address to the buffer.
	sprintf(pcWriteBuffer, "The Netmask is: %s", converted);

	return pdFALSE;	// Return the same as other commands.
}
/************************************************************************************************
* Purpose: prvGetDNSCommand prints the DNS Server Address to the pcWriteBuffer in a formatted string.
*
* Precondition:
*	None
*
* Postcondition:
*	The DNS Server Address will have been written to pcWriteBuffer in a formatted string.
************************************************************************************************/
BaseType_t prvGetDNSCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	( void ) xWriteBufferLen;	// Remove warnings for unused variables.
	( void ) pcCommandString;	// Remove warnings for unused variables.

	pcWriteBuffer[0] = '\0';	// Start with an empty write buffer.

	const uint32_t ret = FreeRTOS_GetDNSServerAddress();	// Execute the operating system's function and save its result.

	char converted[16];					// A string convert the address into the common 111.111... format from the returned integer.
	converted[15] = '\0';				// Place a null character at the end of the string.
	FreeRTOS_inet_ntoa(ret, converted);	// Convert the integer format into the readable 111.111... format using the operating system's built-in function.

	// Write the IP address to the buffer.
	sprintf(pcWriteBuffer, "The DNS Server's IP Address is: %s", converted);

	return pdFALSE;	// Return the same as other commands.
}
/************************************************************************************************
* Purpose: prvGetGatewayCommand prints the IP Address of the Gateway to the pcWriteBuffer in a formatted string.
*
* Precondition:
*	None
*
* Postcondition:
*	The IP Address of the Gateway will have been written to pcWriteBuffer in a formatted string.
************************************************************************************************/
BaseType_t prvGetGatewayCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	( void ) xWriteBufferLen;	// Remove warnings for unused variables.
	( void ) pcCommandString;	// Remove warnings for unused variables.

	pcWriteBuffer[0] = '\0';	// Start with an empty write buffer.

	const uint32_t ret = FreeRTOS_GetGatewayAddress();	// Execute the operating system's function and save its result.

	char converted[16];					// A string convert the address into the common 111.111... format from the returned integer.
	converted[15] = '\0';				// Place a null character at the end of the string.
	FreeRTOS_inet_ntoa(ret, converted);	// Convert the integer format into the readable 111.111... format using the operating system's built-in function.
	
	// Write the IP address to the buffer.
	sprintf(pcWriteBuffer, "The Gateway's IP Address is: %s", converted);

	return pdFALSE;	// Return the same as other commands.
}
/************************************************************************************************
* Purpose: prvGetIPCommand prints the IP Address of the board's NIC to the pcWriteBuffer in a formatted string.
*
* Precondition:
*	None
*
* Postcondition:
*	The IP Address of the board's NIC will have been written to pcWriteBuffer in a formatted string.
************************************************************************************************/
BaseType_t prvGetIPCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	( void ) xWriteBufferLen;	// Remove warnings for unused variables.
	( void ) pcCommandString;	// Remove warnings for unused variables.

	pcWriteBuffer[0] = '\0';	// Start with an empty write buffer.

	const uint32_t ret = FreeRTOS_GetIPAddress();	// Execute the operating system's function and save its result.

	char converted[16];					// A string convert the address into the common 111.111... format from the returned integer.
	converted[15] = '\0';				// Place a null character at the end of the string.
	FreeRTOS_inet_ntoa(ret, converted);	// Convert the integer format into the readable 111.111... format using the operating system's built-in function.

	// Write the IP address to the buffer.
	sprintf(pcWriteBuffer, "The IP Address is: %s", converted);

	return pdFALSE;	// Return the same as other commands.
}
/************************************************************************************************
* Purpose: prvGetMACCommand prints the MAC Address of the board's NIC to the pcWriteBuffer in a formatted string.
*
* Precondition:
*	None
*
* Postcondition:
*	The board's MAC Address will have been written to pcWriteBuffer in a formatted string.
************************************************************************************************/
BaseType_t prvGetMACCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	( void ) xWriteBufferLen;	// Remove warnings for unused variables.
	( void ) pcCommandString;	// Remove warnings for unused variables.

	pcWriteBuffer[0] = '\0';	// Start with an empty write buffer.

	const uint8_t* ret = FreeRTOS_GetMACAddress();	// Execute the operating system's function and save its result.

	// Print the MAC address out in standard XX:XX... format, prefilled with 0s.
	sprintf(pcWriteBuffer, "The MAC Address is: %02x:%02x:%02x:%02x:%02x:%02x", *(ret), *(ret + 1), *(ret + 2), *(ret + 3), *(ret + 4), *(ret + 5));

	return pdFALSE;	// Return the same as other commands.
}
/************************************************************************************************
* Purpose: prvNetStatusCommand prints the status (up or down) of the network to the pcWriteBuffer in a formatted string.
*
* Precondition:
*	None
*
* Postcondition:
*	The network status will have been written to pcWriteBuffer in a formatted string.
************************************************************************************************/
BaseType_t prvNetStatusCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    BaseType_t xReturnValue = pdFALSE;

    // Get rid of compiler warnings about unused variables
    ( void ) pcCommandString;
    ( void ) xWriteBufferLen;
    configASSERT( pcWriteBuffer );

    // Start with an empty string
    pcWriteBuffer[0] = '\0';

    xReturnValue = FreeRTOS_IsNetworkUp();

    if(xReturnValue == pdFALSE)
    {
        sprintf(pcWriteBuffer, "%s", "The Network is Currently down\r\n");
    }
    else
    {
        sprintf(pcWriteBuffer, "%s", "The Network is Currently Up\r\n");
    }

    return pdFALSE;
}
/************************************************************************************************
* Purpose: prvDnsLookupCommand returns the address of a URL to the pcWriteBuffer.
*
* Precondition:
*	A valid URL should have been provided as an argument.
*
* Postcondition:
*	The address of a URL will have been written to pcWriteBuffer in a formatted string, if a valid URL was provided.
*	If the URL could not resolve to an address, a message of failure will be written to the pcWriteBuffer.
*	If no argument was provided, an error will be printed to pcWriteBuffer.
************************************************************************************************/
BaseType_t prvDnsLookupCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    uint32_t ulIPAddress;
    int8_t cBuffer[ 16 ];
    char * pcParameter;
    BaseType_t lParameterStringLength;
    
    // Get rid of compiler warnings about unused variables
    ( void ) pcCommandString;
    ( void ) xWriteBufferLen;
    configASSERT( pcWriteBuffer );

    // Start with an empty string.
    pcWriteBuffer[0] = '\0';

    // Get the parameter
    pcParameter = (char *) FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParameterStringLength);

    if( pcParameter != NULL )
    {
        // Add Null Terminator to the string
        pcParameter[lParameterStringLength] = '\0';

        // Call get host by name to do dns lookup
        ulIPAddress = FreeRTOS_gethostbyname(pcParameter);

        if(ulIPAddress != 0)
        {
            FreeRTOS_inet_ntoa(ulIPAddress, (char *) cBuffer);

            sprintf(pcWriteBuffer, "%s is at IP Address %s\r\n", pcParameter, cBuffer);
        }
        else
        {
            sprintf(pcWriteBuffer, "%s", "DNS Lookup Failed\r\n");
        }
    }
    else
    {
        sprintf(pcWriteBuffer, "%s", "DNS Failed: Could not retrieve parameter\r\n");
    }

    return pdFALSE;
}
/************************************************************************************************
* Purpose: prvSendUDPMessageCommand will try to connect to a UDP server at the provided address to send the provided message.
*
* Precondition:
*	A valid IP Address in the 10.101.132.58 form should be provided
*	A message should be provided
*
* Postcondition:
*	The message will be sent to the UDP server at the given address if connection succeeds.
************************************************************************************************/
BaseType_t prvSendUDPMessageCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	// 1 Socket for communication
	Socket_t sock;
	
	// A struct for a destination address
	struct freertos_sockaddr xSendUDPMessageAddress;
	xSendUDPMessageAddress.sin_port = FreeRTOS_htons(MY_UDP_PORT_NUMBER);
	// A struct for the local address
	struct freertos_sockaddr xLocalAddress;
	xLocalAddress.sin_port = MY_UDP_PORT_NUMBER;
	
	// Get the IP address argument
	BaseType_t addrlen;
	char* addrparam = (char*)FreeRTOS_CLIGetParameter(pcCommandString, 1, &addrlen);
	
	// Get the message
	BaseType_t msglen;
	char* msg = (char*)FreeRTOS_CLIGetParameter(pcCommandString, 2, &msglen);
	
	// Null-terminate both strings.
	addrparam[addrlen] = '\0';
	msg[msglen] = '\0';
	
	// Put the address into the socket address struct. Done here because it needs the null terminator, but the second parameter must be grabbed before the null-terminator is added.
	xSendUDPMessageAddress.sin_addr = FreeRTOS_inet_addr(addrparam);
	
	// Open the socket with settings for UDP
	sock = FreeRTOS_socket(FREERTOS_AF_INET, FREERTOS_SOCK_DGRAM, FREERTOS_IPPROTO_UDP);
	
	// If the socket opened properly
	if (sock != FREERTOS_INVALID_SOCKET)
	{
		// Bind the socket
		BaseType_t ret = FreeRTOS_bind(sock, &xLocalAddress, sizeof(xLocalAddress));
		// If the socket binded properly
		if (ret == 0)
		{
			// Send the message
			ret = FreeRTOS_sendto(sock, msg, msglen, 0, &xSendUDPMessageAddress, sizeof(xSendUDPMessageAddress));
		}
		
		// Close the socket once finished
		FreeRTOS_closesocket(sock);
	}
	
	return pdFALSE;
}
