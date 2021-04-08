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

//Functions
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
}

BaseType_t prvOutputARPCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	( void ) xWriteBufferLen;
	
	BaseType_t lParameterStringLength;
	char* pcParameter = (char*) FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParameterStringLength);
	pcWriteBuffer[0] = '\0';
	
	if (pcParameter != NULL)
	{
		uint32_t converted = FreeRTOS_inet_addr(pcParameter);
		FreeRTOS_OutputARPRequest(converted);
		
		sprintf(pcWriteBuffer, "ARP Request sent out on %s", pcParameter);
	}
	else
		sprintf(pcWriteBuffer, "ARP Output failed: Could not retrieve parameter\r\n");

	return pdFALSE;
}
BaseType_t prvGetNetmaskCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	( void ) xWriteBufferLen;
	( void ) pcCommandString;

	pcWriteBuffer[0] = '\0';

	const uint32_t ret = FreeRTOS_GetNetmask();

	char converted[16];
	converted[15] = '\0';
	FreeRTOS_inet_ntoa(ret, converted);

	sprintf(pcWriteBuffer, "The Netmask is: %s", converted);

	return pdFALSE;
}
BaseType_t prvGetDNSCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	( void ) xWriteBufferLen;
	( void ) pcCommandString;

	pcWriteBuffer[0] = '\0';

	const uint32_t ret = FreeRTOS_GetDNSServerAddress();

	char converted[16];
	converted[15] = '\0';
	FreeRTOS_inet_ntoa(ret, converted);

	sprintf(pcWriteBuffer, "The DNS Server's IP Address is: %s", converted);

	return pdFALSE;
}
BaseType_t prvGetGatewayCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	( void ) xWriteBufferLen;
	( void ) pcCommandString;

	pcWriteBuffer[0] = '\0';

	const uint32_t ret = FreeRTOS_GetGatewayAddress();

	char converted[16];
	converted[15] = '\0';
	FreeRTOS_inet_ntoa(ret, converted);
	
	sprintf(pcWriteBuffer, "The Gateway's IP Address is: %s", converted);

	return pdFALSE;
}
BaseType_t prvGetIPCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	( void ) xWriteBufferLen;
	( void ) pcCommandString;

	pcWriteBuffer[0] = '\0';

	const uint32_t ret = FreeRTOS_GetIPAddress();

	char converted[16];
	converted[15] = '\0';
	FreeRTOS_inet_ntoa(ret, converted);

	sprintf(pcWriteBuffer, "The IP Address is: %s", converted);

	return pdFALSE;
}
BaseType_t prvGetMACCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	( void ) xWriteBufferLen;
	( void ) pcCommandString;

	pcWriteBuffer[0] = '\0';

	const uint8_t* ret = FreeRTOS_GetMACAddress();

	sprintf(pcWriteBuffer, "The MAC Address is: %02x:%02x:%02x:%02x:%02x:%02x", *(ret), *(ret + 1), *(ret + 2), *(ret + 3), *(ret + 4), *(ret + 5));	

	return pdFALSE;
}

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
        sprintf(pcWriteBuffer, "%s", "DNS Failed: Could not retreive parameter\r\n");
    }

    return pdFALSE;
}

