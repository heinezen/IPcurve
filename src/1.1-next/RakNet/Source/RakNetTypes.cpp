/// \file
///
/// This file is part of RakNet Copyright 2003 Kevin Jenkins.
///
/// Usage of RakNet is subject to the appropriate license agreement.
/// Creative Commons Licensees are subject to the
/// license found at
/// http://creativecommons.org/licenses/by-nc/2.5/
/// Single application licensees are subject to the license found at
/// http://www.rakkarsoft.com/SingleApplicationLicense.html
/// Custom license users are subject to the terms therein.
/// GPL license users are subject to the GNU General Public
/// License as published by the Free
/// Software Foundation; either version 2 of the License, or (at your
/// option) any later version.

#include "RakNetTypes.h"
#include <string.h>
#include <stdio.h>

#ifdef _CONSOLE_1
#include "Console1Includes.h"
#elif defined(_WIN32)
// IP_DONTFRAGMENT is different between winsock 1 and winsock 2.  Therefore, Winsock2.h must be linked againt Ws2_32.lib
// winsock.h must be linked against WSock32.lib.  If these two are mixed up the flag won't work correctly
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <string.h> // strncasecmp
#include <stdlib.h> // itoa

// Fast itoa from http://www.jb.man.ac.uk/~slowe/cpp/itoa.html for Linux since it seems like Linux doesn't support this function.
// I modified it to remove the std dependencies.
char* my_itoa( int value, char* result, int base ) {
	// check that the base if valid
	if (base < 2 || base > 16) { *result = 0; return result; }
	char* out = result;
	int quotient = value;

	int absQModB;

	do {
		// KevinJ - get rid of this dependency
		//*out = "0123456789abcdef"[ std::abs( quotient % base ) ];
		absQModB=quotient % base;
		if (absQModB < 0)
			absQModB=-absQModB;
		*out = "0123456789abcdef"[ absQModB ];
		++out;
		quotient /= base;
	} while ( quotient );

	// Only apply negative sign for base 10
	if ( value < 0 && base == 10) *out++ = '-';

	// KevinJ - get rid of this dependency
	// std::reverse( result, out );
	*out = 0;

	// KevinJ - My own reverse code
    char *start = result;
	char temp;
	out--;
	while (start < out)
	{
		temp=*start;
		*start=*out;
		*out=temp;
		start++;
		out--;
	}

	return result;
}

SocketDescriptor::SocketDescriptor() {port=0; hostAddress[0]=0;}
SocketDescriptor::SocketDescriptor(unsigned short _port, const char *_hostAddress)
{
	port=_port;
	if (_hostAddress)
		strcpy(hostAddress, _hostAddress);
	else
		hostAddress[0]=0;
}

// Defaults to not in peer to peer mode for NetworkIDs.  This only sends the localSystemAddress portion in the BitStream class
// This is what you want for client/server, where the server assigns all NetworkIDs and it is unnecessary to transmit the full structure.
// For peer to peer, this will transmit the systemAddress of the system that created the object in addition to localSystemAddress.  This allows
// Any system to create unique ids locally.
// All systems must use the same value for this variable.
bool RAK_DLL_EXPORT NetworkID::peerToPeerMode=false;

bool SystemAddress::operator==( const SystemAddress& right ) const
{
	return binaryAddress == right.binaryAddress && port == right.port;
}

bool SystemAddress::operator!=( const SystemAddress& right ) const
{
	return binaryAddress != right.binaryAddress || port != right.port;
}

bool SystemAddress::operator>( const SystemAddress& right ) const
{
	return ( ( binaryAddress > right.binaryAddress ) || ( ( binaryAddress == right.binaryAddress ) && ( port > right.port ) ) );
}

bool SystemAddress::operator<( const SystemAddress& right ) const
{
	return ( ( binaryAddress < right.binaryAddress ) || ( ( binaryAddress == right.binaryAddress ) && ( port < right.port ) ) );
}
char *SystemAddress::ToString(bool writePort) const
{
#ifdef _CONSOLE_1
	return "";
#else
	static char str[22];
	in_addr in;
	in.s_addr = binaryAddress;
	strcpy(str, inet_ntoa( in ));
	if (writePort)
	{
		strcat(str, ":");
#if (defined(__GNUC__)  || defined(__GCCXML__))
		my_itoa(port, str+strlen(str), 10);
#else
		_itoa(port, str+strlen(str), 10);
#endif
	}
	
	return (char*) str;
#endif
}
void SystemAddress::SetBinaryAddress(const char *str)
{
//#ifdef _CONSOLE_1
//	binaryAddress=UNASSIGNED_SYSTEM_ADDRESS.binaryAddress;
//#else
	// Split the string into the first part, and the : part
	int index, portIndex;
	char IPPart[22];
	char portPart[10];
	// Only write the valid parts, don't change existing if invalid
//	binaryAddress=UNASSIGNED_SYSTEM_ADDRESS.binaryAddress;
//	port=UNASSIGNED_SYSTEM_ADDRESS.port;
	for (index=0; str[index] && str[index]!=':' && index<22; index++)
	{
		IPPart[index]=str[index];
	}
	IPPart[index]=0;
	portPart[0]=0;
	if (str[index] && str[index+1])
	{
		index++;
		for (portIndex=0; portIndex<10 && str[index] && index < 22+10; index++, portIndex++)
			portPart[portIndex]=str[index];
		portPart[portIndex]=0;
	}

#ifdef _WIN32
	if (strnicmp(str,"localhost", 9)==0)
#else
	if (strncasecmp(str,"localhost", 9)==0)
#endif
		binaryAddress=inet_addr("127.0.0.1");
	else if (IPPart[0])
		binaryAddress=inet_addr(IPPart);
	if (portPart[0])
		port=(unsigned short) atoi(portPart);
//#endif
}

NetworkID& NetworkID::operator = ( const NetworkID& input )
{
	systemAddress = input.systemAddress;
	localSystemAddress = input.localSystemAddress;
	return *this;
}

bool NetworkID::operator==( const NetworkID& right ) const
{
	if (NetworkID::peerToPeerMode)
		return systemAddress == right.systemAddress && localSystemAddress == right.localSystemAddress;
	else
		return localSystemAddress==right.localSystemAddress;
}

bool NetworkID::operator!=( const NetworkID& right ) const
{
	if (NetworkID::peerToPeerMode)
		return systemAddress != right.systemAddress || localSystemAddress != right.localSystemAddress;
	else
		return localSystemAddress!=right.localSystemAddress;
}

bool NetworkID::operator>( const NetworkID& right ) const
{
	if (NetworkID::peerToPeerMode)
		return ( ( systemAddress > right.systemAddress ) || ( ( systemAddress == right.systemAddress ) && ( localSystemAddress > right.localSystemAddress ) ) );
	else
		return localSystemAddress>right.localSystemAddress;
}

bool NetworkID::operator<( const NetworkID& right ) const
{
	if (NetworkID::peerToPeerMode)
		return ( ( systemAddress < right.systemAddress ) || ( ( systemAddress == right.systemAddress ) && ( localSystemAddress < right.localSystemAddress ) ) );
	else
		return localSystemAddress<right.localSystemAddress;
}

bool NetworkID::IsPeerToPeerMode(void)
{
	return peerToPeerMode;
}
void NetworkID::SetPeerToPeerMode(bool isPeerToPeer)
{
	peerToPeerMode=isPeerToPeer;
}
