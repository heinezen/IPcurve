/// \file
/// \brief Client plugin for the autopatcher
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

#ifndef __AUTOPATCHER_CLIENT_H
#define __AUTOPATCHER_CLIENT_H

#include "RakNetTypes.h"
#include "Export.h"
#include "PluginInterface.h"
#include "PacketPriority.h"
#include "FileList.h"

class RakPeerInterface;
struct Packet;
class FileListTransfer;
class FileListTransferCBInterface;

/// \ingroup Autopatcher
class RAK_DLL_EXPORT AutopatcherClient : public PluginInterface
{
public:
	/// Constructor
	AutopatcherClient();

	/// Destructor
	~AutopatcherClient();

	/// Are we in the middle of patching?
	/// \return True if yes, false otherwise.
	bool IsPatching(void) const;

	/// What parameters to use for the RakPeerInterface::Send() call when uploading files.
	/// \param[in] _priority See RakPeerInterface::Send()
	/// \param[in] _orderingChannel See RakPeerInterface::Send()
	void SetUploadSendParameters(PacketPriority _priority, char _orderingChannel);

	/// This plugin has a dependency on the FileListTransfer plugin, which it uses to actually send the files.
	/// So you need an instance of that plugin registered with RakPeerInterface, and a pointer to that interface should be passed here.
	/// \param[in] flt A pointer to a registered instance of FileListTransfer
	void SetFileListTransferPlugin(FileListTransfer *flt);

	/// Patches a certain directory associated with a named application to match the same named application on the patch server
	/// \param[in] _applicationName The name of the application
	/// \param[in] _applicationDirectory The directory to write the output to.
	/// \param[in] lastUpdateDate A string representing the last time you updated from the patch server, so you only check newer files.  Should be 0 the first time, or if you want to do a full scan.  Returned in GetServerDate() after you call PatchApplication successfully.
	/// \param[in] host The address of the remote system to send the message to.
	/// \param[in] onFileCallback Callback to call per-file (optional).  When fileIndex+1==setCount in the callback then the download is done
	/// \param[in] _restartOutputFilename If it is necessary to restart this application, where to write the restart data to.  You can include a path in this filename.
	/// \param[in] pathToRestartExe What exe to launch from the AutopatcherClientRestarter .  argv[0] will work to relaunch this application.
	/// \return true on success, false on failure.  On failure, ID_AUTOPATCHER_REPOSITORY_FATAL_ERROR is returned, with the error message written using the stringCompressor class starting after the message id.
	bool PatchApplication(const char *_applicationName, const char *_applicationDirectory, const char *lastUpdateDate, SystemAddress host, FileListTransferCBInterface *onFileCallback, const char *restartOutputFilename, const char *pathToRestartExe);

	/// After getting ID_AUTOPATCHER_FINISHED or ID_AUTOPATCHER_RESTART_APPLICATION, the server date will be internally recorded.  You can send this to PatchApplication::lastUpdateDate to only check for files newer than that date.
	char *GetServerDate(void) const;

	/// Stop the current download
	void CancelDownload(void);

	/// Free internal memory.
	void Clear(void);

	/// \internal For plugin handling
	virtual void OnAttach(RakPeerInterface *peer);
	/// \internal For plugin handling
	virtual void Update(RakPeerInterface *peer);
	/// \internal For plugin handling
	virtual PluginReceiveResult OnReceive(RakPeerInterface *peer, Packet *packet);
	/// \internal For plugin handling
	virtual void OnShutdown(RakPeerInterface *peer);
	void OnThreadCompletion(void);
protected:
	PluginReceiveResult OnCreationList(RakPeerInterface *peer, Packet *packet);
	void OnDeletionList(RakPeerInterface *peer, Packet *packet);
	PluginReceiveResult OnDownloadFinishedInternal(RakPeerInterface *peer, Packet *packet);
	PluginReceiveResult OnDownloadFinished(RakPeerInterface *peer, Packet *packet);
	
	friend class AutopatcherClientCallback;
	void CopyAndRestart(const char *filePath);
	void Redownload(const char *filePath);

	char applicationDirectory[512];
	char applicationName[512];
	char copyOnRestartOut[512];
	char restartExe[512];
	char serverDate[128];
	FileListTransfer *fileListTransfer;
	RakPeerInterface *rakPeer;
	PacketPriority priority;
	SystemAddress serverId;
	SystemIndex serverIdIndex;
	char orderingChannel;
	unsigned short setId;
	FileListTransferCBInterface *userCB;
	bool patchComplete;
	FileList redownloadList, copyAndRestartList;
	bool processThreadCompletion;
};

#endif
