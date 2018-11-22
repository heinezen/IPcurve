#include "AutopatcherClient.h"
#include "DirectoryDeltaTransfer.h"
#include "FileList.h"
#include "StringCompressor.h"
#include "RakPeerInterface.h"
#include "FileListTransfer.h"
#include "FileListTransferCBInterface.h"
#include "BitStream.h"
#include "MessageIdentifiers.h"
#include "AutopatcherPatchContext.h"
#include "ApplyPatch.h"
#include "FileOperations.h"
#include "SHA1.h"
#include <stdio.h>
#include "FileOperations.h"
#include "RakAssert.h"
#include "ThreadPool.h"

#ifdef _MSC_VER
#pragma warning( push )
#endif

#define COPY_ON_RESTART_EXTENSION ".patched.tmp"

struct AutopatcherClientThreadInfo
{
	FileListTransferCBInterface::OnFileStruct onFileStruct;
	char applicationDirectory[512];
	PatchContext result;
	unsigned prePatchLength, postPatchLength;
	char *prePatchFile, *postPatchFile;
};
// -----------------------------------------------------------------
AutopatcherClientThreadInfo* AutopatcherClientWorkerThread(AutopatcherClientThreadInfo* input, bool *returnOutput, void* perThreadData)
{
	char fullPathToDir[1024];
	*returnOutput=true;

	strcpy(fullPathToDir, input->applicationDirectory);
	strcat(fullPathToDir, input->onFileStruct.fileName);
	if (input->onFileStruct.context==PC_WRITE_FILE)
	{
		if (WriteFileWithDirectories(fullPathToDir, (char*)input->onFileStruct.fileData, input->onFileStruct.finalDataLength)==false)
		{
			char newDir[1024];
			strcpy(newDir, fullPathToDir);
			strcat(newDir, COPY_ON_RESTART_EXTENSION);
			if (WriteFileWithDirectories(newDir, (char*)input->onFileStruct.fileData, input->onFileStruct.finalDataLength))
			{
				input->result=PC_NOTICE_WILL_COPY_ON_RESTART;
			}
			else
			{
				input->result=PC_ERROR_FILE_WRITE_FAILURE;
			}
		}
		else
		{
			input->result=(PatchContext) input->onFileStruct.context;
		}
	}
	else
	{
		assert(input->onFileStruct.context==PC_HASH_WITH_PATCH);

		CSHA1 sha1;
		FILE *fp;

		fp=fopen(fullPathToDir, "rb");
		if (fp==0)
		{
			input->result=PC_ERROR_PATCH_TARGET_MISSING;
			return input;
		}
		fseek(fp, 0, SEEK_END);
		input->prePatchLength = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		input->postPatchFile=0;
		input->prePatchFile= new char [input->prePatchLength];
		fread(input->prePatchFile, input->prePatchLength, 1, fp);
		fclose(fp);

		//				printf("apply patch %i bytes\n", finalDataLength-SHA1_LENGTH);
		//				for (int i=0; i < finalDataLength-SHA1_LENGTH; i++)
		//					printf("%i ", fileData[SHA1_LENGTH+i]);
		//				printf("\n");
		if (ApplyPatch((char*)input->prePatchFile, input->prePatchLength, &input->postPatchFile, &input->postPatchLength, (char*)input->onFileStruct.fileData+SHA1_LENGTH, input->onFileStruct.finalDataLength-SHA1_LENGTH)==false)
		{

			input->result=PC_ERROR_PATCH_APPLICATION_FAILURE;
			return input;
		}

		sha1.Reset();
		sha1.Update((unsigned char*) input->postPatchFile, input->postPatchLength);
		sha1.Final();

		if (memcmp(sha1.GetHash(), input->onFileStruct.fileData, SHA1_LENGTH)!=0)
		{
			input->result=PC_ERROR_PATCH_RESULT_CHECKSUM_FAILURE;
		}

		// Write postPatchFile over the existing file
		if (WriteFileWithDirectories(fullPathToDir, (char*)input->postPatchFile, input->postPatchLength)==false)
		{
			char newDir[1024];
			strcpy(newDir, fullPathToDir);
			strcat(newDir, COPY_ON_RESTART_EXTENSION);
			if (WriteFileWithDirectories(newDir, (char*)input->postPatchFile, input->postPatchLength))
			{
				input->result=PC_NOTICE_WILL_COPY_ON_RESTART;
			}
			else
			{
				input->result=PC_ERROR_FILE_WRITE_FAILURE;
			}
		}
		else
		{
			input->result=(PatchContext)input->onFileStruct.context;
		}
	}

	return input;
}
// -----------------------------------------------------------------
class AutopatcherClientCallback : public FileListTransferCBInterface
{
public:
	ThreadPool<AutopatcherClientThreadInfo*,AutopatcherClientThreadInfo*> threadPool;
	char applicationDirectory[512];
	FileListTransferCBInterface *onFileCallback;
	AutopatcherClient *client;
	bool downloadComplete;
	bool canDeleteUser;

	AutopatcherClientCallback(void)
	{
		threadPool.StartThreads(1,0);
		canDeleteUser=false;
		downloadComplete=false;
	}
	virtual ~AutopatcherClientCallback(void)
	{
		StopThreads();
	}
	void StopThreads(void)
	{
		threadPool.StopThreads();

		unsigned i;
		AutopatcherClientThreadInfo* info;
		for (i=0; i < threadPool.InputSize(); i++)
		{
			info = threadPool.GetInputAtIndex(i);
			if (info->prePatchFile)
				delete [] info->prePatchFile;
			if (info->postPatchFile)
				delete [] info->postPatchFile;
			if (info->onFileStruct.fileData)
				delete [] info->onFileStruct.fileData;
			delete info;
		}
		threadPool.ClearInput();
		for (i=0; i < threadPool.OutputSize(); i++)
		{
			info = threadPool.GetOutputAtIndex(i);
			if (info->prePatchFile)
				delete [] info->prePatchFile;
			if (info->postPatchFile)
				delete [] info->postPatchFile;
			if (info->onFileStruct.fileData)
				delete [] info->onFileStruct.fileData;
			delete info;
		}
		threadPool.ClearOutput();
	}
	virtual bool Update(void)
	{
		if (threadPool.HasOutputFast() && threadPool.HasOutput())
		{
			AutopatcherClientThreadInfo *threadInfo = threadPool.GetOutput();
			threadInfo->onFileStruct.context=threadInfo->result;
			switch (threadInfo->result)
			{
				case PC_NOTICE_WILL_COPY_ON_RESTART:
				{
					client->CopyAndRestart(threadInfo->onFileStruct.fileName);
					if (threadInfo->onFileStruct.context==PC_WRITE_FILE)
					{
						// Regular file in use but we can write the temporary file.  Restart and copy it over the existing
						onFileCallback->OnFile(&threadInfo->onFileStruct);
					}
					else
					{
						// Regular file in use but we can write the temporary file.  Restart and copy it over the existing
						delete [] threadInfo->onFileStruct.fileData;
						threadInfo->onFileStruct.fileData=threadInfo->postPatchFile;
						onFileCallback->OnFile(&threadInfo->onFileStruct);
						threadInfo->onFileStruct.fileData=0;
					}
				}
				break;
				case PC_ERROR_FILE_WRITE_FAILURE:
				{
					if (threadInfo->onFileStruct.context==PC_WRITE_FILE)
					{
						onFileCallback->OnFile(&threadInfo->onFileStruct);
					}
					else
					{
						delete [] threadInfo->onFileStruct.fileData;
						threadInfo->onFileStruct.fileData=threadInfo->postPatchFile;
						threadInfo->onFileStruct.finalDataLength=threadInfo->postPatchLength;
						onFileCallback->OnFile(&threadInfo->onFileStruct);
						threadInfo->onFileStruct.fileData=0;
					}					
				}
				break;
				case PC_ERROR_PATCH_TARGET_MISSING:
				{
					onFileCallback->OnFile(&threadInfo->onFileStruct);
					client->Redownload(threadInfo->onFileStruct.fileName);
				}
				break;
				case PC_ERROR_PATCH_APPLICATION_FAILURE:
				{
					// Failure - signal class and download this file.
					onFileCallback->OnFile(&threadInfo->onFileStruct);
					client->Redownload(threadInfo->onFileStruct.fileName);
				}
				break;
				case PC_ERROR_PATCH_RESULT_CHECKSUM_FAILURE:
				{
					// Failure - signal class and download this file.
					onFileCallback->OnFile(&threadInfo->onFileStruct);
					client->Redownload(threadInfo->onFileStruct.fileName);
				}
				break;
				default:
				{
					if (threadInfo->onFileStruct.context==PC_WRITE_FILE)
					{
						onFileCallback->OnFile(&threadInfo->onFileStruct);
					}
					else
					{
						delete [] threadInfo->onFileStruct.fileData;
						threadInfo->onFileStruct.fileData=threadInfo->postPatchFile;
						onFileCallback->OnFile(&threadInfo->onFileStruct);
						threadInfo->onFileStruct.fileData=0;
					}
				}
				break;
			}

			if (threadInfo->prePatchFile)
				delete [] threadInfo->prePatchFile;
			if (threadInfo->postPatchFile)
				delete [] threadInfo->postPatchFile;
			if (threadInfo->onFileStruct.fileData)
				delete [] threadInfo->onFileStruct.fileData;
			delete threadInfo;
		}

		// If both input and output are empty, we are done.
		if (onFileCallback->Update()==false)
			canDeleteUser=true;

		if ( downloadComplete &&
			canDeleteUser &&
			threadPool.IsWorking()==false)
		{
			// Stop threads before calling OnThreadCompletion, in case the other thread starts a new instance of this thread.
			StopThreads();
			client->OnThreadCompletion();
			return false;
		}

		return true;
	}
	virtual bool OnDownloadComplete(void)
	{
		downloadComplete=true;
		if (onFileCallback->OnDownloadComplete()==false)
		{
			canDeleteUser=true;
		}
		return true;
	};
	virtual void OnDereference(void)
	{
		onFileCallback->OnDereference();
	}
	virtual bool OnFile(OnFileStruct *onFileStruct)
	{
		AutopatcherClientThreadInfo *inStruct = new AutopatcherClientThreadInfo;
		inStruct->prePatchFile=0;
		inStruct->postPatchFile=0;
		memcpy(&(inStruct->onFileStruct), onFileStruct, sizeof(OnFileStruct));
		memcpy(inStruct->applicationDirectory,applicationDirectory,sizeof(applicationDirectory));
		if (onFileStruct->context==PC_HASH_WITH_PATCH)
			onFileStruct->context=PC_NOTICE_FILE_DOWNLOADED_PATCH;
		else
			onFileStruct->context=PC_NOTICE_FILE_DOWNLOADED;
		onFileCallback->OnFile(onFileStruct);
		threadPool.AddInput(AutopatcherClientWorkerThread, inStruct);

		// Return false means don't delete OnFileStruct::data
		return false;
	}
	virtual void OnFileProgress(OnFileStruct *onFileStruct,unsigned int partCount,unsigned int partTotal,unsigned int partLength)
	{
		char fullPathToDir[1024];

		if (onFileStruct->fileName)
		{
			strcpy(fullPathToDir, applicationDirectory);
			strcat(fullPathToDir, onFileStruct->fileName);
			onFileCallback->OnFileProgress(onFileStruct, partCount, partTotal, partLength);
		}
	}
};
AutopatcherClient::AutopatcherClient()
{
	rakPeer=0;
	serverId=UNASSIGNED_SYSTEM_ADDRESS;
	serverIdIndex=-1;
	applicationDirectory[0]=0;
	fileListTransfer=0;
    priority=HIGH_PRIORITY;
	orderingChannel=0;
	serverDate[0]=0;
	userCB=0;
	processThreadCompletion=false;
}
AutopatcherClient::~AutopatcherClient()
{
	Clear();
}
void AutopatcherClient::Clear(void)
{
	if (fileListTransfer)
		fileListTransfer->RemoveReceiver(serverId);
	serverId=UNASSIGNED_SYSTEM_ADDRESS;
	setId=(unsigned short)-1;
	redownloadList.Clear();
	copyAndRestartList.Clear();
}
void AutopatcherClient::SetUploadSendParameters(PacketPriority _priority, char _orderingChannel)
{
	priority=_priority;
	orderingChannel=_orderingChannel;
}
void AutopatcherClient::SetFileListTransferPlugin(FileListTransfer *flt)
{
	fileListTransfer=flt;
}
char* AutopatcherClient::GetServerDate(void) const
{
	return (char*)serverDate;
}
void AutopatcherClient::CancelDownload(void)
{
	fileListTransfer->CancelReceive(setId);
}
void AutopatcherClient::OnThreadCompletion(void)
{
	processThreadCompletion=true;
}
bool AutopatcherClient::IsPatching(void) const
{
	return fileListTransfer->IsHandlerActive(setId);
}
bool AutopatcherClient::PatchApplication(const char *_applicationName, const char *_applicationDirectory, const char *lastUpdateDate, SystemAddress host, FileListTransferCBInterface *onFileCallback, const char *restartOutputFilename, const char *pathToRestartExe)
{
    assert(applicationName);
	assert(applicationDirectory);
	assert(rakPeer);
	assert(pathToRestartExe);
	assert(restartOutputFilename);

	if (rakPeer->GetIndexFromSystemAddress(host)==-1)
		return false;
	if (IsPatching())
		return false; // Already in the middle of patching.

	strcpy(applicationDirectory, _applicationDirectory);
	if (IsSlash(applicationDirectory[strlen(applicationDirectory)-1])==false)
		strcat(applicationDirectory, "\\");
	strcpy(applicationName, _applicationName);
	serverId=host;
	patchComplete=false;
	userCB=onFileCallback;
	strcpy(copyOnRestartOut, restartOutputFilename);
	strcpy(restartExe, pathToRestartExe);
	processThreadCompletion=false;

	RakNet::BitStream outBitStream;
	outBitStream.Write((unsigned char)ID_AUTOPATCHER_GET_CHANGELIST_SINCE_DATE);
	stringCompressor->EncodeString(applicationName, 512, &outBitStream);
	stringCompressor->EncodeString(lastUpdateDate, 64, &outBitStream);
    rakPeer->Send(&outBitStream, priority, RELIABLE_ORDERED, orderingChannel, host, false);
	return true;
}
void AutopatcherClient::OnAttach(RakPeerInterface *peer)
{
	rakPeer=peer;
}
#ifdef _MSC_VER
#pragma warning( disable : 4100 ) // warning C4100: <variable name> : unreferenced formal parameter
#endif
void AutopatcherClient::Update(RakPeerInterface *peer)
{
	if (processThreadCompletion)
	{
		processThreadCompletion=false;
		fileListTransfer->RemoveReceiver(serverId);

		// If redownload list, process it
		if (redownloadList.fileList.Size())
		{
			RakNet::BitStream outBitStream;
			AutopatcherClientCallback *transferCallback;
			transferCallback = new AutopatcherClientCallback;
			strcpy(transferCallback->applicationDirectory, applicationDirectory);
			transferCallback->onFileCallback=userCB;
			transferCallback->client=this;
			setId = fileListTransfer->SetupReceive(transferCallback, true, serverId);

			// Ask for patches for the files in the list that are different from what we have.
			outBitStream.Write((unsigned char)ID_AUTOPATCHER_GET_PATCH);
			outBitStream.Write(setId);
			stringCompressor->EncodeString(applicationName, 512, &outBitStream);
			redownloadList.Serialize(&outBitStream);
			rakPeer->Send(&outBitStream, priority, RELIABLE_ORDERED, orderingChannel, serverId, false);
			redownloadList.Clear();
		}
		else if (copyAndRestartList.fileList.Size())
		{
			Packet *p = peer->AllocatePacket(1);
			p->bitSize=p->length*8;
			p->data[0]=ID_AUTOPATCHER_RESTART_APPLICATION;
			p->systemAddress=serverId;
			p->systemIndex=serverIdIndex;
			peer->PushBackPacket(p, false);

			FILE *fp;
			fp = fopen(copyOnRestartOut, "wt");
			assert(fp);
			if (fp)
			{
				fprintf(fp, "#Sleep 1000\n");
				unsigned i;
				for (i=0; i < copyAndRestartList.fileList.Size(); i++)
				{
					fprintf(fp, "del /q \"%s%s\"\n", applicationDirectory, copyAndRestartList.fileList[i].filename);
					fprintf(fp, "rename \"%s%s%s\" \"%s\"\n", applicationDirectory, copyAndRestartList.fileList[i].filename, COPY_ON_RESTART_EXTENSION, copyAndRestartList.fileList[i].filename);
				}
				fprintf(fp, "#CreateProcess \"%s\"\n", restartExe);
				fprintf(fp, "#DeleteThisFile\n");
				fclose(fp);
			}
		}
		else
		{
			Packet *p = peer->AllocatePacket(1);
			p->bitSize=p->length*8;
			p->data[0]=ID_AUTOPATCHER_FINISHED;
			p->systemAddress=serverId;
			p->systemIndex=serverIdIndex;
			peer->PushBackPacket(p, false);
		}
	}
}
PluginReceiveResult AutopatcherClient::OnReceive(RakPeerInterface *peer, Packet *packet)
{
	switch (packet->data[0]) 
	{
	case ID_CONNECTION_LOST:
	case ID_DISCONNECTION_NOTIFICATION:
		if (packet->systemAddress==serverId)
			Clear();
		return RR_CONTINUE_PROCESSING;
	case ID_AUTOPATCHER_CREATION_LIST:
		return OnCreationList(peer, packet);
	case ID_AUTOPATCHER_DELETION_LIST:
		OnDeletionList(peer, packet);
		return RR_STOP_PROCESSING_AND_DEALLOCATE;
	case ID_AUTOPATCHER_REPOSITORY_FATAL_ERROR:
		fileListTransfer->RemoveReceiver(serverId);
		Clear();
		return RR_CONTINUE_PROCESSING;
	case ID_AUTOPATCHER_FINISHED_INTERNAL:
		return OnDownloadFinishedInternal(peer, packet);
	case ID_AUTOPATCHER_FINISHED:
		return OnDownloadFinished(peer, packet);
	}
	return RR_CONTINUE_PROCESSING;
}
#ifdef _MSC_VER
#pragma warning( disable : 4100 ) // warning C4100: <variable name> : unreferenced formal parameter
#endif
void AutopatcherClient::OnShutdown(RakPeerInterface *peer)
{
	// TODO
}

PluginReceiveResult AutopatcherClient::OnCreationList(RakPeerInterface *peer, Packet *packet)
{
	assert(fileListTransfer);
	if (packet->systemAddress!=serverId)
		return RR_STOP_PROCESSING_AND_DEALLOCATE;

	RakNet::BitStream inBitStream(packet->data, packet->length, false);
	RakNet::BitStream outBitStream;
	FileList remoteFileList, missingOrChanged;
	inBitStream.IgnoreBits(8);
	if (remoteFileList.Deserialize(&inBitStream)==false)
		return RR_STOP_PROCESSING_AND_DEALLOCATE;

	stringCompressor->DecodeString(serverDate, 128, &inBitStream);
	RakAssert(serverDate[0]);

	// Go through the list of hashes.  For each file we already have, remove it from the list.
	remoteFileList.ListMissingOrChangedFiles(applicationDirectory, &missingOrChanged, true, false);

	if (missingOrChanged.fileList.Size()==0)
	{
		packet->data[0]=ID_AUTOPATCHER_FINISHED;
		return RR_CONTINUE_PROCESSING; // Pass to user
	}

	// Prepare the transfer plugin to get a file list.
	AutopatcherClientCallback *transferCallback;
	transferCallback = new AutopatcherClientCallback;
	strcpy(transferCallback->applicationDirectory, applicationDirectory);
	transferCallback->onFileCallback=userCB;
	transferCallback->client=this;
	setId = fileListTransfer->SetupReceive(transferCallback, true, packet->systemAddress);

	// Ask for patches for the files in the list that are different from what we have.
	outBitStream.Write((unsigned char)ID_AUTOPATCHER_GET_PATCH);
	outBitStream.Write(setId);
	stringCompressor->EncodeString(applicationName, 512, &outBitStream);
	missingOrChanged.Serialize(&outBitStream);
	rakPeer->Send(&outBitStream, priority, RELIABLE_ORDERED, orderingChannel, packet->systemAddress, false);

	return RR_STOP_PROCESSING_AND_DEALLOCATE; // Absorb this message
}
void AutopatcherClient::OnDeletionList(RakPeerInterface *peer, Packet *packet)
{
	if (packet->systemAddress!=serverId)
		return;

	RakNet::BitStream inBitStream(packet->data, packet->length, false);
	RakNet::BitStream outBitStream;
	inBitStream.IgnoreBits(8);
	FileList fileList;
	if (fileList.Deserialize(&inBitStream)==false)
		return;
	fileList.DeleteFiles(applicationDirectory);
}
PluginReceiveResult AutopatcherClient::OnDownloadFinished(RakPeerInterface *peer, Packet *packet)
{
	RakNet::BitStream inBitStream(packet->data, packet->length, false);
	inBitStream.IgnoreBits(8);
	// This may have been created internally, with no serverDate written (line 469 or so)
	if (inBitStream.GetNumberOfUnreadBits()>7)
		stringCompressor->DecodeString(serverDate, 128, &inBitStream);
	RakAssert(serverDate[0]);
	serverId=packet->systemAddress;
	serverIdIndex=packet->systemIndex;

	return RR_CONTINUE_PROCESSING;
}
PluginReceiveResult AutopatcherClient::OnDownloadFinishedInternal(RakPeerInterface *peer, Packet *packet)
{
	RakNet::BitStream inBitStream(packet->data, packet->length, false);
	inBitStream.IgnoreBits(8);
	serverId=packet->systemAddress;
	serverIdIndex=packet->systemIndex;
	stringCompressor->DecodeString(serverDate, 128, &inBitStream);
	RakAssert(serverDate[0]);

	return RR_STOP_PROCESSING_AND_DEALLOCATE;
}
void AutopatcherClient::CopyAndRestart(const char *filePath)
{
	// We weren't able to write applicationDirectory + filePath so we wrote applicationDirectory + filePath + COPY_ON_RESTART_EXTENSION instead
	copyAndRestartList.AddFile(filePath, 0, 0, 0, 0);
}
void AutopatcherClient::Redownload(const char *filePath)
{
	redownloadList.AddFile(filePath, 0, 0, 0, 0);
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif
