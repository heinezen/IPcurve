#include "ThreadsafePacketLogger.h"
#include <string.h>

ThreadsafePacketLogger::ThreadsafePacketLogger()
{

}
ThreadsafePacketLogger::~ThreadsafePacketLogger()
{
	char **msg;
	while ((msg = logMessages.ReadLock()) != 0)
	{
		delete [] (*msg);
	}
}
void ThreadsafePacketLogger::Update(RakPeerInterface *peer)
{
	char **msg;
	while ((msg = logMessages.ReadLock()) != 0)
	{
		WriteLog(*msg);
		delete [] (*msg);
	}
}
void ThreadsafePacketLogger::AddToLog(const char *str)
{
	char **msg = logMessages.WriteLock();
	*msg = new char [strlen(str)+1];
	strcpy(*msg, str);
	logMessages.WriteUnlock();
}
