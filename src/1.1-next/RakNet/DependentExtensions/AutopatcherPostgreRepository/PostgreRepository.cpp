/// \file
/// \brief An implementation of the AutopatcherRepositoryInterface to use PostgreSQL to store the relevant data
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

#include "PostgreRepository.h"
#ifdef _USE_POSTGRE_REPOSITORY
#include "AutopatcherPatchContext.h"
#include "FileList.h"
// libpq-fe.h is part of PostgreSQL which must be installed on this computer to use the PostgreRepository
#include "libpq-fe.h"
#include "CreatePatch.h"
#include "AutopatcherPatchContext.h"
#include "SHA1.h"
#include <stdlib.h>

// ntohl
#ifdef _WIN32
#include <Winsock2.h>
#else
#include <netinet/in.h>
#endif

// alloca
#ifdef _COMPATIBILITY_1
#elif defined(_WIN32)
#include <malloc.h>
#else
//#include <stdlib.h>
#endif

#ifndef _MSC_VER
#define stricmp strcasecmp
#endif

PostgreRepository::PostgreRepository()
{
	pgConn=0;
	isConnected=false;
	lastError[0]=0;
}
PostgreRepository::~PostgreRepository()
{
	if (isConnected)
		PQfinish(pgConn);
}
bool PostgreRepository::Connect(const char *conninfo)
{
	if (isConnected==false)
	{
		pgConn=PQconnectdb(conninfo);
		ConnStatusType status = PQstatus(pgConn);
		isConnected=status==CONNECTION_OK;
		if (isConnected==false)
			PQfinish(pgConn);
	}
	
	return isConnected;
}
void PostgreRepository::Disconnect(void)
{
	if (isConnected)
	{
		PQfinish(pgConn);
		isConnected=false;
	}
}
bool PostgreRepository::CreateAutopatcherTables(void)
{
	if (isConnected==false)
		return false;

	const char *command =
		"BEGIN;"
	"CREATE TABLE Applications ("
		"applicationID serial PRIMARY KEY UNIQUE,"
		"applicationName text NOT NULL UNIQUE,"
		"changeSetID integer NOT NULL DEFAULT 0,"
		"userName text NOT NULL"
		");"
	"CREATE TABLE FileVersionHistory ("
		"fileID serial PRIMARY KEY UNIQUE,"
		"applicationID integer REFERENCES Applications ON DELETE CASCADE,"
		"filename text NOT NULL,"
		"fileLength integer,"
		"content bytea,"
		"contentHash bytea,"
		"patch bytea,"
		"createFile boolean NOT NULL,"
		"modificationDate timestamp NOT NULL DEFAULT LOCALTIMESTAMP,"
		"lastSentDate timestamp,"
		"timesSent integer NOT NULL DEFAULT 0,"
		"changeSetID integer NOT NULL,"
		"userName text NOT NULL,"
		"CONSTRAINT file_has_data CHECK ( createFile=FALSE OR ((content IS NOT NULL) AND (contentHash IS NOT NULL) AND (fileLength IS NOT NULL) ) )"
		");"
		"COMMIT;";

	PGresult *result;
	bool res = ExecuteBlockingCommand(command, &result);
	PQclear(result);
	return res;
}
void PostgreRepository::Rollback(void)
{
	PGresult *result = PQexec(pgConn, "ROLLBACK;");
	PQclear(result);
}
bool PostgreRepository::IsResultSuccessful(PGresult *result)
{
	if (result==0)
		return false;

	bool success=false;
	ExecStatusType execStatus = PQresultStatus(result);
	strcpy(lastError,PQresultErrorMessage(result));
	switch (execStatus)
	{
	case PGRES_COMMAND_OK:
		success=true;
		break;
	case PGRES_EMPTY_QUERY:
		break;
	case PGRES_TUPLES_OK:
		success=true;
		break;
	case PGRES_COPY_OUT:
		break;
	case PGRES_COPY_IN:
		break;
	case PGRES_BAD_RESPONSE:
		break;
	case PGRES_NONFATAL_ERROR:
		break;
	case PGRES_FATAL_ERROR:
		Rollback();
		break;
	}
	return success;
}
bool PostgreRepository::ExecuteBlockingCommand(const char *command, PGresult **result)
{
	*result = PQexec(pgConn, command);
	return IsResultSuccessful(*result);
	
}
bool PostgreRepository::DestroyAutopatcherTables(void)
{
	if (isConnected==false)
		return false;

	const char *command = 
		"BEGIN;"
		"DROP TABLE Applications CASCADE;"
		"DROP TABLE FileVersionHistory;"
		"COMMIT;";

	PGresult *result;
	bool b = ExecuteBlockingCommand(command, &result);
	PQclear(result);
	return b;
}
bool PostgreRepository::AddApplication(const char *applicationName, char *userName)
{
	char query[512];
	if (strlen(applicationName)>100)
		return false;
	if (strlen(userName)>100)
		return false;

	sprintf(query, "INSERT INTO Applications (applicationName, userName) VALUES ('%s', '%s');", applicationName, userName);
	PGresult *result;
	bool b = ExecuteBlockingCommand(query, &result);
	PQclear(result);
	return b;
}
bool PostgreRepository::RemoveApplication(const char *applicationName)
{
	char query[512];
	if (strlen(applicationName)>100)
		return false;
	
	sprintf(query, "DELETE FROM Applications WHERE applicationName=%s;", applicationName);
	PGresult *result;
	bool b = ExecuteBlockingCommand(query, &result);
	PQclear(result);
	return b;
}

// TODO - Make this asynch
bool PostgreRepository::GetChangelistSinceDate(const char *applicationName, FileList *addedFiles, FileList *deletedFiles, const char *sinceDate, char currentDate[64])
{
	PGresult *result;
	char query[512];
	if (strlen(applicationName)>100)
		return false;
	if (strlen(sinceDate)>63)
		return false;
	sprintf(query, "SELECT applicationID FROM applications WHERE applicationName='%s';", applicationName);
	if (ExecuteBlockingCommand(query, &result)==false)
	{
		PQclear(result);
		return false;
	}
	int numRows;
	numRows = PQntuples(result);
	if (numRows==0)
	{
		sprintf(lastError,"ERROR: %s not found in UpdateApplicationFiles\n",applicationName);
		return false;
	}
	char *res;
	res = PQgetvalue(result,0,0);
	int applicationID;
	applicationID=atoi(res);
	PQclear(result);
	if (sinceDate && sinceDate[0])
		sprintf(query, "SELECT DISTINCT ON (filename) filename, fileLength, contentHash, createFile FROM FileVersionHistory WHERE applicationId=%i AND modificationDate > '%s' ORDER BY filename, fileId DESC;", applicationID, sinceDate);
	else
		sprintf(query, "SELECT DISTINCT ON (filename) filename, fileLength, contentHash, createFile FROM FileVersionHistory WHERE applicationId=%i ORDER BY filename, fileId DESC;", applicationID);

	result = PQexecParams(pgConn, query,0,0,0,0,0,1);
	if (IsResultSuccessful(result)==false)
	{
		PQclear(result);
		return false;
	}

	int filenameColumnIndex = PQfnumber(result, "filename");
	int contentHashColumnIndex = PQfnumber(result, "contentHash");
	int createFileColumnIndex = PQfnumber(result, "createFile");
	int fileLengthColumnIndex = PQfnumber(result, "fileLength");
	char *hardDriveFilename;
	char *hardDriveHash;
	char *createFileResult;
	char *fileLengthPtr;
	int rowIndex;
	int fileLength;
	assert(PQfsize(result, fileLengthColumnIndex)==sizeof(fileLength));
	
	numRows = PQntuples(result);

	for (rowIndex=0; rowIndex < numRows; rowIndex++)
	{
		createFileResult = PQgetvalue(result, rowIndex, createFileColumnIndex);
		hardDriveFilename = PQgetvalue(result, rowIndex, filenameColumnIndex);
		if (createFileResult[0]==1)
		{
			hardDriveHash = PQgetvalue(result, rowIndex, contentHashColumnIndex);
			fileLengthPtr = PQgetvalue(result, rowIndex, fileLengthColumnIndex);
			memcpy(&fileLength, fileLengthPtr, sizeof(fileLength));
			fileLength=ntohl(fileLength); // This is asinine...
			addedFiles->AddFile(hardDriveFilename, hardDriveHash, SHA1_LENGTH, fileLength, 0);
		}
		else
		{
			deletedFiles->AddFile(hardDriveFilename,0,0,0,0);
		}
	}

	if (ExecuteBlockingCommand("SELECT LOCALTIMESTAMP", &result)==false)
	{
		PQclear(result);
		return false;
	}

	char *ts=PQgetvalue(result, 0, 0);
	if (ts)
	{
		strncpy(currentDate, ts,63);
		currentDate[63]=0;
	}
	else
	{
		strcpy(lastError, "Can't read current time\n");
		return false;
	}
	
	return true;
}

// TODO - Make this asynch
bool PostgreRepository::GetPatches(const char *applicationName, FileList *input, FileList *patchList, char currentDate[64])
{
	PGresult *result;
	char query[512];
	if (strlen(applicationName)>100)
		return false;
	sprintf(query, "SELECT applicationID FROM applications WHERE applicationName='%s';", applicationName);
	if (ExecuteBlockingCommand(query, &result)==false)
	{
		PQclear(result);
		return false;
	}
	int numRows;
	numRows = PQntuples(result);
	if (numRows==0)
	{
		sprintf(lastError,"ERROR: %s not found in UpdateApplicationFiles\n",applicationName);
		return false;
	}
	char *res;
	res = PQgetvalue(result,0,0);
	int applicationID;
	applicationID=atoi(res);
	PQclear(result);

	// Go through the input list.
	unsigned inputIndex;
	char *userHash, *userFilename, *content, *contentHash;
	char *fileId, *fileLength;
	char *patch;
	int contentLength, patchLength;
//	int contentColumnIndex;
	int contentHashIndex, fileIdIndex, fileLengthIndex;
	char *outTemp[2];
	int outLengths[2];
	int formats[2];
	PGresult *patchResult;
	for (inputIndex=0; inputIndex < input->fileList.Size(); inputIndex++)
	{
		userHash=input->fileList[inputIndex].data;
		userFilename=input->fileList[inputIndex].filename;

		if (userHash==0)
		{
			// If the user does not have a hash in the input list, get the contents of latest version of this named file and write it to the patch list
			sprintf(query, "SELECT DISTINCT ON (filename) content FROM FileVersionHistory WHERE applicationId=%i AND filename=$1::text ORDER BY filename, fileId DESC;", applicationID);
			outTemp[0]=userFilename;
			outLengths[0]=(int)strlen(userFilename);
			formats[0]=1;
			result = PQexecParams(pgConn, query,1,0,outTemp,outLengths,formats,1);
			if (IsResultSuccessful(result)==false)
			{
				PQclear(result);
				return false;
			}
			numRows = PQntuples(result);
			if (numRows>0)
			{
				// Shouldn't be necessary
				//contentColumnIndex = PQfnumber(result, "content");
				//content = PQgetvalue(result, 0, contentColumnIndex);
				content = PQgetvalue(result, 0, 0);
				contentLength=PQgetlength(result, 0, 0);
				patchList->AddFile(userFilename, content, contentLength, contentLength, PC_WRITE_FILE);
			}
			PQclear(result);
		}
		else // Assuming the user does have a hash.
		{
			if (input->fileList[inputIndex].dataLength!=SHA1_LENGTH)
				return false;

			// Get the hash and ID of the latest version of this file, by filename.
			sprintf(query, "SELECT DISTINCT ON (filename) contentHash, fileId, fileLength FROM FileVersionHistory WHERE applicationId=%i AND filename=$1::text ORDER BY filename, fileId DESC;", applicationID);
			outTemp[0]=userFilename;
			outLengths[0]=(int)strlen(userFilename);
			formats[0]=1;
			result = PQexecParams(pgConn, query,1,0,outTemp,outLengths,formats,1);
			if (IsResultSuccessful(result)==false)
			{
				PQclear(result);
				return false;
			}
			numRows = PQntuples(result);
			if (numRows>0)
			{
				contentHashIndex = PQfnumber(result, "contentHash");
				fileIdIndex = PQfnumber(result, "fileId");
				fileLengthIndex = PQfnumber(result, "fileLength");
				contentHash = PQgetvalue(result, 0, contentHashIndex);
				fileId = PQgetvalue(result, 0, fileIdIndex);
				fileLength = PQgetvalue(result, 0, fileLengthIndex);

				if (memcmp(contentHash, userHash, SHA1_LENGTH)!=0)
				{
					// Look up by user hash/filename/applicationID, returning the patch
					sprintf(query, "SELECT patch FROM FileVersionHistory WHERE applicationId=%i AND filename=$1::text AND contentHash=$2::bytea;", applicationID);
					outTemp[0]=userFilename;
					outLengths[0]=(int)strlen(userFilename);
					formats[0]=1;
					outTemp[1]=userHash;
					outLengths[1]=SHA1_LENGTH;
					formats[1]=1;
					patchResult = PQexecParams(pgConn, query,2,0,outTemp,outLengths,formats,1);

					if (IsResultSuccessful(patchResult)==false)
					{
						PQclear(patchResult);
						return false;
					}
					numRows = PQntuples(patchResult);
					if (numRows==0)
					{
						PQclear(patchResult);

						// If no patch found, then this is a non-release version, or a very old version we are no longer tracking.
						// Get the contents of latest version of this named file by fileId and return it.
						sprintf(query, "SELECT content FROM FileVersionHistory WHERE fileId=$1::integer;");
						outTemp[0]=fileId;
						outLengths[0]=PQfsize(result, fileIdIndex);
						formats[0]=1;
						patchResult = PQexecParams(pgConn, query,1,0,outTemp,outLengths,formats,1);
						if (IsResultSuccessful(patchResult)==false)
						{
							PQclear(patchResult);
							return false;
						}
						numRows = PQntuples(patchResult);
						if (numRows>0)
						{
							assert(numRows==1);
							// Write the whole file
							content = PQgetvalue(patchResult, 0, 0);
							contentLength=PQgetlength(patchResult, 0, 0);
							patchList->AddFile(userFilename, content, contentLength, contentLength, PC_WRITE_FILE);
						}
						PQclear(patchResult);
					}
					else
					{
						// Otherwise, write the hash of the new version and then write the patch to get to that version.
						// 
						patch = PQgetvalue(patchResult, 0, 0);
						patchLength=PQgetlength(patchResult, 0, 0);
						// Bleh, get a stack overflow here
						// char *temp = (char*) _alloca(patchLength+SHA1_LENGTH);
						char *temp = new char [patchLength + SHA1_LENGTH];
						memcpy(temp, contentHash, SHA1_LENGTH);
						memcpy(temp+SHA1_LENGTH, patch, patchLength);
						int len;
						assert(PQfsize(result, fileLengthIndex)==sizeof(fileLength));
						memcpy(&len, fileLength, sizeof(len));
						len=ntohl(len);
					//	printf("send patch %i bytes\n", patchLength);
					//	for (int i=0; i < patchLength; i++)
					//		printf("%i ", patch[i]);
					//	printf("\n");
						patchList->AddFile(userFilename, temp, SHA1_LENGTH+patchLength, len, PC_HASH_WITH_PATCH);
						PQclear(patchResult);
						delete [] temp;
					}
				}
				else
				{
					// else if the hash of this file matches what the user has, the user has the latest version.  Done.
				}
			}
			else
			{
				// else if there is no such file, skip this file.
			}

			PQclear(result);
		}
	}

	if (ExecuteBlockingCommand("SELECT LOCALTIMESTAMP", &result)==false)
	{
		PQclear(result);
		return false;
	}

	char *ts=PQgetvalue(result, 0, 0);
	if (ts)
	{
		strncpy(currentDate, ts,63);
		currentDate[63]=0;
	}
	else
	{
		strcpy(lastError, "Can't read current time\n");
		return false;
	}

	return true;
}
char* PostgreRepository::GetLastError(void) const
{
	return (char*)lastError;
}

bool PostgreRepository::UpdateApplicationFiles(const char *applicationName, const char *applicationDirectory, char *userName)
{
	FileList filesOnHarddrive;
	filesOnHarddrive.AddFilesFromDirectory(applicationDirectory,"", true, true, true, 0);
	if (filesOnHarddrive.fileList.Size()==0)
	{
		sprintf(lastError,"ERROR: Can't find files at %s in UpdateApplicationFiles\n",applicationDirectory);
		return false;
	}

	int numRows;
	PGresult *result;
	char query[512];
	if (strlen(applicationName)>100)
		return false;
	if (strlen(userName)>100)
		return false;

	sprintf(query, "SELECT applicationID FROM applications WHERE applicationName='%s';", applicationName);
	if (ExecuteBlockingCommand(query, &result)==false)
	{
		PQclear(result);
		return false;
	}
	numRows = PQntuples(result);
	if (numRows==0)
	{
		sprintf(lastError,"ERROR: %s not found in UpdateApplicationFiles\n",applicationName);
		return false;
	}
	char *res;
	res = PQgetvalue(result,0,0);
	int applicationID;
	applicationID=atoi(res);
	PQclear(result);

	// If ExecuteBlockingCommand fails then it does a rollback
	if (ExecuteBlockingCommand("BEGIN;", &result)==false)
	{
		PQclear(result);
		return false;
	}
	PQclear(result);

	sprintf(query, "UPDATE applications SET changeSetId = changeSetId + 1 where applicationID=%i; SELECT changeSetId FROM applications WHERE applicationID=%i;", applicationID, applicationID);
	if (ExecuteBlockingCommand(query, &result)==false)
	{
		PQclear(result);
		return false;
	}
	numRows = PQntuples(result);
	if (numRows==0)
	{
		sprintf(lastError,"ERROR: applicationID %i not found in UpdateApplicationFiles\n", applicationID);
		Rollback();
		return false;
	}
	res = PQgetvalue(result,0,0);
	int changeSetId;
	changeSetId=atoi(res);
	PQclear(result);

	// +1 was added in the update
	changeSetId--;

	// Gets all newest files
	// TODO - This can be non-blocking
	sprintf(query, "SELECT DISTINCT ON (filename) filename, contentHash, createFile FROM FileVersionHistory WHERE applicationID=%i ORDER BY filename, fileId DESC;", applicationID);
	result = PQexecParams(pgConn, query,0,0,0,0,0,1);
	if (IsResultSuccessful(result)==false)
	{
		PQclear(result);
		return false;
	}

	int filenameColumnIndex = PQfnumber(result, "filename");
	int contentHashColumnIndex = PQfnumber(result, "contentHash");
	int createFileColumnIndex = PQfnumber(result, "createFile");

	//char *PQgetvalue(result,int row_number,int column_number);
	//int PQgetlength(result, int row_number, int column_number);

	unsigned fileListIndex;
	int rowIndex;
	char *hardDriveFilename;
	char *hardDriveHash;
	char *queryFilename;
	char *createFileResult;
	char *hash;
	bool addFile;
	FileList newFiles;
	numRows = PQntuples(result);
    
	// Loop through files om filesOnHarddrive
	// If the file in filesOnHarddrive does not exist in the query result, or if it does but the hash is different or non-existent, add this file to the create list
	for (fileListIndex=0; fileListIndex < filesOnHarddrive.fileList.Size(); fileListIndex++)
	{
		if (fileListIndex%10==0)
			printf("Hashing files %i/%i\n", fileListIndex, filesOnHarddrive.fileList.Size());
		addFile=true;
		hardDriveFilename=filesOnHarddrive.fileList[fileListIndex].filename;
		hardDriveHash=filesOnHarddrive.fileList[fileListIndex].data;
		for (rowIndex=0; rowIndex < numRows; rowIndex++ )
		{
			queryFilename = PQgetvalue(result, rowIndex, filenameColumnIndex);

			if (stricmp(hardDriveFilename, queryFilename)==0)
			{
				createFileResult = PQgetvalue(result, rowIndex, createFileColumnIndex);
				hash = PQgetvalue(result, rowIndex, contentHashColumnIndex);
				if (createFileResult[0]==1 && memcmp(hash, hardDriveHash, SHA1_LENGTH)==0)
				{
					// File exists in database and is the same
					addFile=false;
				}

				break;
			}
		}

		// Unless set to false, file does not exist in query result or is different.
		if (addFile==true)
		{
			newFiles.AddFile(hardDriveFilename, filesOnHarddrive.fileList[fileListIndex].data, filesOnHarddrive.fileList[fileListIndex].dataLength, filesOnHarddrive.fileList[fileListIndex].fileLength, 0);
		}
	}
	
	// Go through query results that are marked as create
	// If a file that is currently on the database is not on the harddrive, add it to the delete list
	FileList deletedFiles;
	bool fileOnHarddrive;
	for (rowIndex=0; rowIndex < numRows; rowIndex++ )
	{
		queryFilename = PQgetvalue(result, rowIndex, filenameColumnIndex);
		createFileResult = PQgetvalue(result, rowIndex, createFileColumnIndex);
		if (createFileResult[0]!=1)
			continue; // If already false don't mark false again.

		fileOnHarddrive=false;
		for (fileListIndex=0; fileListIndex < filesOnHarddrive.fileList.Size(); fileListIndex++)
		{
			hardDriveFilename=filesOnHarddrive.fileList[fileListIndex].filename;
			hardDriveHash=filesOnHarddrive.fileList[fileListIndex].data;

			if (stricmp(hardDriveFilename, queryFilename)==0)
			{
				fileOnHarddrive=true;
				break;
			}
		}

		if (fileOnHarddrive==false)
			deletedFiles.AddFile(queryFilename,0,0,0,0);
	}

	// Query memory and files on harddrive no longer needed.  Free this memory since generating all the patches is memory intensive.
	PQclear(result);
	filesOnHarddrive.Clear();

	char *outTemp[3];
	int outLengths[3];
	int formats[3];
	formats[1]=1; // Always happens to be binary
	formats[2]=1; // Always happens to be binary

	// For each file in the delete list add a row indicating file deletion
	for (fileListIndex=0; fileListIndex < deletedFiles.fileList.Size(); fileListIndex++)
	{
		if (fileListIndex%10==0)
			printf("Tagging deleted files %i/%i\n", fileListIndex, newFiles.fileList.Size());

		// BUGGED
		sprintf(query, "INSERT INTO FileVersionHistory(applicationID, filename, createFile, changeSetID, userName) VALUES (%i, $1::text,FALSE,%i,'%s');", applicationID, changeSetId, userName);
		outTemp[0]=deletedFiles.fileList[fileListIndex].filename;
		outLengths[0]=(int)strlen(deletedFiles.fileList[fileListIndex].filename);
		formats[0]=0;
		result = PQexecParams(pgConn, query,1,0,outTemp,outLengths,formats,1);
		if (IsResultSuccessful(result)==false)
		{
			deletedFiles.Clear();
			newFiles.Clear();
			PQclear(result);
			return false;
		}
		PQclear(result);
	}
	
	// Clear the delete list as it is no longer needed.
	deletedFiles.Clear();

	int contentColumnIndex;		

	PGresult *uploadResult;
	char *content;
	char *fileID;
	int contentLength;
	int fileIDLength;
	int fileIDColumnIndex;
	char *hardDriveData;
	unsigned hardDriveDataLength;
	char *patch;
	unsigned patchLength;	

	// For each file in the create list
	for (fileListIndex=0; fileListIndex < newFiles.fileList.Size(); fileListIndex++)
	{
		if (fileListIndex%10==0)
			printf("Adding file %i/%i\n", fileListIndex, newFiles.fileList.Size());
		hardDriveFilename=newFiles.fileList[fileListIndex].filename;
		hardDriveData=newFiles.fileList[fileListIndex].data+SHA1_LENGTH;
		hardDriveHash=newFiles.fileList[fileListIndex].data;
		hardDriveDataLength=newFiles.fileList[fileListIndex].fileLength;

		// Get the file history of all files marked for create
		outTemp[0]=hardDriveFilename;
		outLengths[0]=(int)strlen(hardDriveFilename);
		formats[0]=0;
		sprintf(query, "SELECT content, fileID from FileVersionHistory WHERE applicationID=%i AND filename=$1::text AND createFile=TRUE;",applicationID);
		// Last param of 1 means binary format output
		result = PQexecParams(pgConn, query,1,0,outTemp,outLengths,formats,1);
		if (IsResultSuccessful(result)==false)
		{
			newFiles.Clear();
			PQclear(result);
			return false;
		}


		contentColumnIndex= PQfnumber(result, "content");
		fileIDColumnIndex= PQfnumber(result, "fileID");
		numRows = PQntuples(result);

		// Create new patches for every create version
		for (rowIndex=0; rowIndex < numRows; rowIndex++ )
		{
			contentLength=PQgetlength(result, rowIndex, contentColumnIndex);
			content=PQgetvalue(result, rowIndex, contentColumnIndex);
			fileIDLength=PQgetlength(result, rowIndex, fileIDColumnIndex);			
			fileID=PQgetvalue(result, rowIndex, fileIDColumnIndex);
			if (CreatePatch(content, contentLength, hardDriveData, hardDriveDataLength, &patch, &patchLength)==false)
			{
				strcpy(lastError,"CreatePatch failed.");
				Rollback();
				newFiles.Clear();
				PQclear(result);
				return false;
			}

			outTemp[0]=patch;
			outTemp[1]=fileID;
			outLengths[0]=patchLength;
			outLengths[1]=fileIDLength;
			formats[0]=1; // Binary

	//		printf("create patch %i bytes\n", patchLength);
	//		for (int i=0; i < patchLength; i++)
	//			printf("%i ", patch[i]);
	//		printf("\n");

			// Upload these patches using PQexecParams so I can pass binary data in memory.
			// PQexecParams only takes one command at a time.
			uploadResult = PQexecParams(pgConn, "UPDATE FileVersionHistory SET patch=$1::bytea where fileID=$2::int;",2,0,outTemp,outLengths,formats,0);
			if (IsResultSuccessful(uploadResult)==false)
			{
				Rollback();
				newFiles.Clear();
				PQclear(result);
				return false;
			}


			/*
			char *newf;
			unsigned newSize;
			if (ApplyPatch(content, contentLength, &newf, &newSize, patch, patchLength)==false)
			{
				printf("Failed\n");

			}
			*/

			// Done with this patch data
			delete [] patch;

			PQclear(uploadResult);
		}

		PQclear(result);

		sprintf(query, "INSERT INTO FileVersionHistory (applicationID, filename, fileLength, content, contentHash, createFile, changeSetID, userName) "
			"VALUES ("
			"%i,"
			"$1::text,"
			"%i,"
			"$2::bytea,"
			"$3::bytea,"
			"TRUE,"
			"%i,"
			"'%s'"
			");", applicationID, hardDriveDataLength, changeSetId, userName);

		outTemp[0]=hardDriveFilename;
		outTemp[1]=hardDriveData;
		outTemp[2]=hardDriveHash;
		outLengths[0]=(int)strlen(hardDriveFilename);
		outLengths[1]=hardDriveDataLength;
		outLengths[2]=SHA1_LENGTH;
		formats[0]=0; // Text
		assert(formats[1]=1);
		assert(formats[2]=1);
		
		// Upload the new file
		uploadResult = PQexecParams(pgConn, query,3,0,outTemp,outLengths,formats,1);
		if (IsResultSuccessful(uploadResult)==false)
		{
			newFiles.Clear();
			PQclear(uploadResult);
			return false;
		}

		// Clear the upload result
		PQclear(uploadResult);
	}
	
	printf("DB COMMIT\n");

	// If ExecuteBlockingCommand fails then it does a rollback
	if (ExecuteBlockingCommand("COMMIT;", &result)==false)
	{
		printf("COMMIT Failed!\n");
		PQclear(result);
		return false;
	}
	PQclear(result);

	printf("COMMIT Success\n");

	return true;
}
char *PostgreRepository::GetLocalTimestamp(void)
{
	static char strRes[256];

	PGresult *result;
	if (ExecuteBlockingCommand("SELECT LOCALTIMESTAMP", &result))
	{
		char *ts=PQgetvalue(result, 0, 0);
		if (ts)
		{
			sprintf(strRes,"Local timestamp is: %s\n", ts);
		}
		else
		{
			sprintf(strRes,"Can't read current time\n");
		}
		PQclear(result);
	}
	else
		sprintf(strRes,"Failed to read LOCALTIMESTAMP\n");
	
	return (char*)strRes;

}
#endif
