#ifndef __LOBBY_CLIENT_INTERFACE_H
#define __LOBBY_CLIENT_INTERFACE_H

enum LobbyEvent
{
	LOBBY_EVENT_COUNT
};

enum LobbyClientEvents
{
	LCE_ADDED_FRIEND,
	LCE_SENT_MESSAGE_SUCCESS,
	LCE_BLOCK_USER_SUCCESS,
	LCE_ALL_FRIENDS_DOWNLOADED,
	LCE_FRIEND_REMOVED_FROM_LIST,
	LCE_FRIEND_ADDED_DIFFERENT_GAME,
	LCE_FRIEND_ADDED_SAME_GAME,
	LCE_FRIEND_OFFLINE,
	LCE_SAME_GAME_INSTANT_MESSAGE,
	LCE_GAME_INVITATION,
	LCE_GAME_GOT_MESSAGE_WITH_ATTACHMENT_GUI
};

class LobbyClientCBInterface
{
public:
	LobbyClientCBInterface() {}
	virtual ~LobbyClientCBInterface() {}
	virtual void AddedFriendSuccessfully(void) {}
	virtual void SentMessageSuccessfully(void) {}
	virtual void BlockedUserSuccessfully(void) {}
	virtual void AllFriendsDownloaded(void) {}
	virtual void FriendRemovedFromFriendsList(void *friendId, const char *friendName) {}
	virtual void FriendAddedToFriendsList(void *friendId, const char *friendName, bool inSameGame) {}
	virtual void FriendOffline(void *friendId, const char *friendName) {}
	virtual void GotInstantMessage(void *friendId, const char *friendName, char *data, int dataLength) {}
	virtual void GotGameInvitation(void *friendId, const char *friendName) {}
	virtual void GameMessageWithAttachmentArrived(void *friendId, const char *friendName) {}
	virtual void GotGameMessageWithAttachmentThroughGui(void *friendId, const char *friendName, char *data, int dataLength) {}

	virtual void LeaveRoomComplete(void) {}
	virtual void NewMemberJoinedRoom(void *memberId, const char *memberName) {}
	virtual void MemberLeftRoom(void *memberId, const char *memberName) {}
	virtual void RoomDisappeared(void) {}
	virtual void RoomOwnershipGranted(void *newOwnerId, const char *newOwnerName) {}
	virtual void RoomOwnershipChanged(void *newOwnerId, const char *newOwnerName) {}
	virtual void RoomMemberKicked(void *memberId, const char *memberName) {}
	virtual void KickedFromRoom(void) {}
	virtual void RoomJoined(void) {}
	virtual void RoomCreated(void) {}
	virtual void RoomSearchJoined(void) {}
	virtual void RoomQuickMatchJoined(void) {}
	virtual void RoomQuickMatchCancelled(void) {}
	virtual void RoomJoinInvitationSentConfirmation(void) {}
	virtual void RoomJoinInvitationAccepted(void *roomId) {}
	virtual void ReceivedRoomList(void) {}

};

// 3 versions
// PS3 version
// Scaleform version, which does what the PS3 internal client does for the GUI
// PC version, which is the same as the scaleform version, but does not have a built-in GUI and relies on the user to provide it
class LobbyClientInterface
{
public:	

};

#endif
