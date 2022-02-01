#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <sstream>

#define MRM_SERVER_CONNECT (WM_USER + 1)
#define MRM_NEW_CLIENT     (WM_USER + 2)
#define MRM_CLIENT         (WM_USER + 10)		  // 64 next reserved

#define MRNM_GET_GAME_NAME		40
#define MRNM_REMOVE_ENTRY		41
#define MRNM_GAME_NAME			42
#define MRNM_CONN_NAME_GET_SET	43
#define MRNM_CONN_NAME_SET		44
#define MRNM_CLIENT_ADDR_REQ	45
#define MRNM_CLIENT_ADDR		46
#define MRNM_LAG_TEST			47
#define MRNM_LAG_ANSWER			48
#define MRNM_LAG_INFO			49
#define MRNM_CONNECTION_DONE	50
#define MRNM_READY				51
#define MRNM_CANCEL_GAME		52
#define MRNM_SET_PLAYER_ID		53

#define MR_CONNECTION_TIMEOUT   21000			  // 21 sec

#define MR_PING_RETRY_TIME		750
#define MR_CLIENT_RETRY_TIME	5000

#define MR_ID_NOT_SET				255

#define MR_OUT_QUEUE_LEN			2048
#define MR_MAX_NET_MESSAGE_LEN		255

#define MR_NET_REQUIRED				1
#define MR_NET_TRY					2
#define MR_NOT_REQUIRED				0
#define MR_NET_DATAGRAM				-1

#define LV_ITEM LVITEM


typedef signed char MR_Int8;
typedef unsigned char MR_UInt8;
typedef signed short MR_Int16;
typedef unsigned short MR_UInt16;
typedef signed int MR_Int32;
typedef unsigned int MR_UInt32;
typedef LONGLONG MR_Int64;
typedef DWORDLONG MR_UInt64;

class CentralisedNetMessageBuffer
{
public:
	// MR_UInt16  mSendingTime:10; // 4ms of precision on a +-2sec range (if a datagram take more than 2 sec to travel..it will be droped (UDP only)
	// Not used yet


	MR_UInt16 mDatagramNumber : 8;			  /// used only for datagrams (UDP)
	MR_UInt16 mDatagramQueue : 2;				  /// used only for datagrams (UDP)
	MR_UInt16 mMessageType : 6;
	MR_UInt8 mClient;
	MR_UInt8 mDataLen;
	MR_UInt8 mData[255];

	// int MessageLen()const { return mDataLen+5; }
};




#pragma comment (lib, "ws2_32.lib")

void BroadCastData( const CentralisedNetMessageBuffer* pMessage, int pReqLevel)
void SendData(const CentralisedNetMessageBuffer* pMessage, int pReqLevel);


using namespace std;
#define MR_MAX_NET_MESSAGE_LEN		255
int mWatchdog;
std::string mGameName;			/// just the track name



int mOutQueueLen;
int mOutQueueHead;
MR_UInt8 mOutQueue[MR_OUT_QUEUE_LEN];

SOCKET sock;

void main()
{
	// Initialze winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		cerr << "Can't Initialize winsock! Quitting" << endl;
		return;
	}

	// Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cerr << "Can't create a socket! Quitting" << endl;
		return;
	}

	// Bind the ip address and port to a socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54200);
	hint.sin_addr.S_un.S_addr = INADDR_ANY; // Could also use inet_pton .... 

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Tell Winsock the socket is for listening 
	listen(listening, SOMAXCONN);


	cout << "listening" << endl;
	// Create the master file descriptor set and zero it
	fd_set master;
	FD_ZERO(&master);

	// Add our first socket that we're interested in interacting with; the listening socket!
	// It's important that this socket is added for our server or else we won't 'hear' incoming
	// connections 
	FD_SET(listening, &master);

	// this will be changed by the \quit command (see below, bonus not in video!)
	bool running = true;

	while (running)
	{
		// Make a copy of the master file descriptor set, this is SUPER important because
		// the call to select() is _DESTRUCTIVE_. The copy only contains the sockets that
		// are accepting inbound connection requests OR messages. 

		// E.g. You have a server and it's master file descriptor set contains 5 items;
		// the listening socket and four clients. When you pass this set into select(), 
		// only the sockets that are interacting with the server are returned. Let's say
		// only one client is sending a message at that time. The contents of 'copy' will
		// be one socket. You will have LOST all the other sockets.

		// SO MAKE A COPY OF THE MASTER LIST TO PASS INTO select() !!!

		fd_set copy = master;

		// See who's talking to us
		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		// Loop through all the current connections / potential connect
		for (int i = 0; i < socketCount; i++)
		{
			// Makes things easy for us doing this assignment
			sock = copy.fd_array[i];

			// Is it an inbound communication?
			if (sock == listening)
			{
				// Accept a new connection
				SOCKET client = accept(listening, nullptr, nullptr);

				// Add the new connection to the list of connected clients
				FD_SET(client, &master);


				char host[NI_MAXHOST];		// Client's remote name
				char service[NI_MAXSERV];	// Service (i.e. port) the client is connect on

				ZeroMemory(host, NI_MAXHOST); // same as memset(host, 0, NI_MAXHOST);
				ZeroMemory(service, NI_MAXSERV);

				cout << host << " connected on port " << service << endl;



				// Send a welcome message to the connected client
			//	string welcomeMsg = "Welcome to the Awesome Chat Server!\r\n";
			//	send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
			}
			else // It's an inbound message
			{
				char buf[4096];
				ZeroMemory(buf, 4096);
		
				std::string s(buf, sizeof(buf));
			//	cout << s <<"shame";
				int mInputMessageBufferIndex;
				mInputMessageBufferIndex = 0;
				CentralisedNetMessageBuffer mInputMessageBuffer;
//#define MR_NET_HEADER_LEN  (sizeof(CentralisedNetMessageBuffer) - MR_MAX_NET_MESSAGE_LEN)
#define MR_NET_HEADER_LEN  (sizeof(CentralisedNetMessageBuffer) - MR_MAX_NET_MESSAGE_LEN)
				
				
				if (mInputMessageBufferIndex < MR_NET_HEADER_LEN) {
					// Try to read message header
					int lLen = recv(sock, ((char*)&mInputMessageBuffer) + mInputMessageBufferIndex, MR_NET_HEADER_LEN - mInputMessageBufferIndex, 0);

					if (lLen > 0) {
						mInputMessageBufferIndex += lLen;
				//		TRACE("Begin recv, client %d, message %d, number %d\n", mInputMessageBuffer.mClient, mInputMessageBuffer.mMessageType, mInputMessageBuffer.mDatagramNumber);
					}
				}

				if (mInputMessageBufferIndex >= MR_NET_HEADER_LEN && (mInputMessageBuffer.mDataLen > 0)) {
					int lLen = recv(sock, ((char*)&mInputMessageBuffer) + mInputMessageBufferIndex, mInputMessageBuffer.mDataLen - (mInputMessageBufferIndex - MR_NET_HEADER_LEN), 0);

					if (lLen > 0) {
						mInputMessageBufferIndex += lLen;
					//	TRACE("Continue recv, client %d, message %d, number %d\n", mInputMessageBuffer.mClient, mInputMessageBuffer.mMessageType, mInputMessageBuffer.mDatagramNumber);
					}
				}

				if ((mInputMessageBufferIndex >= MR_NET_HEADER_LEN) && (mInputMessageBufferIndex == (MR_NET_HEADER_LEN + mInputMessageBuffer.mDataLen))) {
					mInputMessageBufferIndex = 0;
				//	mWatchdog = timeGetTime();
				//	TRACE("Done receiving packet from client %d, message %d, number %d\n", mInputMessageBuffer.mClient, mInputMessageBuffer.mMessageType, mInputMessageBuffer.mDatagramNumber);
				//	return &mInputMessageBuffer;
				}
				
				
				cout << mInputMessageBuffer.mMessageType << endl;
				CentralisedNetMessageBuffer lAnswer;
std::string mPlayer ="Central Server";

std::string lConnectionName ((const char*)(mInputMessageBuffer.mData + 4), mInputMessageBuffer.mDataLen - 4);
				switch (mInputMessageBuffer.mMessageType)
				{

				case 47:

					//it was requesting a lag test


					//let's force start the race lol

					lAnswer.mMessageType = MRNM_READY;
					lAnswer.mDataLen = 1;

					//Need to have a player counter and the new player should have the id based on that
					lAnswer.mData[0] = 0 + 1;

					SendData(sock,&lAnswer, MR_NET_REQUIRED);


					break;


				case 43:

					
					//Client has sent their name

					//In HoverRace when this message is recieved it add the new user to the list of players in the race
					//Instead we're doing to keep them in either an array or a list


					//This part also exchanges the UDP ports for players. Since we're not going to have players opening ports we'll just ignore this



				//	LV_ITEM lItem;

				CentralisedNetMessageBuffer lAnswer;
	//	 lConnectionName=((const char*)(mInputMessageBuffer.mData + 4), mInputMessageBuffer.mDataLen - 4);
				 cout << "Player name is" << lConnectionName.c_str() << endl;
					
			
				 
				 //I think this is where we should add the player to the list of players
					

					//We're not using udp so ignore this
					//mActiveInterface->mClient[lClient].SetRemoteUDPPort(*(unsigned int*)(mInputMessageBuffer.mData));
					//mActiveInterface->mClientName[lClient] = lConnectionName;
                    
					//more listview stuff that we don't need in a server
				//	lItem.mask = LVIF_TEXT;
				//	lItem.iItem = lClient + 1;
				//	lItem.iSubItem = 0;
				//	lItem.pszText = const_cast<char*>(lConnectionName.c_str());

				//	if (ListView_GetItemCount(lListHandle) > lItem.iItem) {
				//		ListView_SetItem(lListHandle, &lItem);
				//	}
				//	else {
				///		ListView_InsertItem(lListHandle, &lItem);
				//	}
				//	SetListViewText(lListHandle, lClient + 1, 1, _("Computing lag"));
				//	SetListViewText(lListHandle, lClient + 1, 2, _("Connecting"));

					// return local name as an answer
					// also include UDP port number in the request


					//let's just send out dUMMu data... WE're not going to get everyone to open a UDP port
						
					lAnswer.mMessageType = MRNM_CONN_NAME_SET;
					lAnswer.mDataLen = mPlayer.length() + 4;
					*(unsigned int*)(lAnswer.mData) = htons(1690);
					memcpy(lAnswer.mData + 4, mPlayer.c_str(), lAnswer.mDataLen - 4);

				SendData(sock,&lAnswer, MR_NET_REQUIRED);

					// Tell client their ID and send client list if server
				
						// send player ID
						lAnswer.mMessageType = MRNM_SET_PLAYER_ID;
						lAnswer.mDataLen = 1;

						//Need to have a player counter and the new player should have the id based on that
						lAnswer.mData[0] = 0 + 1;

						SendData(sock,&lAnswer, MR_NET_REQUIRED);

						// now send the client list
						lAnswer.mMessageType = MRNM_CLIENT_ADDR;
						lAnswer.mClient = 0;
						lAnswer.mDataLen = 12;

						// Send the actual client list

						/*
						for (int lCounter = 0; lCounter < 9; lCounter++) {
							if ((lCounter != 100)
								//&& (mActiveInterface->mClient[lCounter].IsConnected())
								//&& (mActiveInterface->mCanBePreLogued[lCounter])) {
						//		*(int*)&(lAnswer.mData[0]) = mActiveInterface->mClientAddr[lCounter];
							//	*(int*)&(lAnswer.mData[4]) = mActiveInterface->mClientBkAddr[lCounter];
						//		*(int*)&(lAnswer.mData[8]) = mActiveInterface->mClientPort[lCounter];

							//	SendData(&lAnswer, MR_NET_REQUIRED);

							}*/
					
						lAnswer.mDataLen = 0;
						SendData(sock,&lAnswer, MR_NET_REQUIRED);

						//mActiveInterface->mCanBePreLogued[lClient] = TRUE;
				

			
		


					break;


				case 40:
					cout <<  "User sent MRNM_GET_GAME_NAME" << endl;

					//so now we need to send them the game name
				
				mGameName ="SteepleChase";			/// just the track name


					lAnswer.mMessageType = MRNM_GAME_NAME;
					lAnswer.mDataLen = mGameName.length();
					memcpy(lAnswer.mData, mGameName.c_str(), lAnswer.mDataLen);

				//	mActiveInterface->mClient[lClient].Send(&lAnswer, MR_NET_REQUIRED);


					int sendResult;



#define MR_NET_HEADER_LEN  (sizeof(CentralisedNetMessageBuffer) - MR_MAX_NET_MESSAGE_LEN)
					// First try to send buffered data


					SendData(sock,&lAnswer, MR_NET_REQUIRED);

					
				
				 cout << "responded " << endl;

				// send(outSock, strOut.c_str(), strOut.size() + 1, 0);
				 
				 break;

				}//end code to handle which kind of tcp message was recieved

				
		//		int lLen = recv(sock, ((char*)&mInputMessageBuffer) + mInputMessageBufferIndex, ((sizeof(CentralisedNetMessageBuffer) - MR_MAX_NET_MESSAGE_LEN)) - mInputMessageBufferIndex, 0);

				// Receive message
			/*
				int bytesIn = recv(sock, buf, 4096, 0);
				cout << buf[0] << "bame";
				cout << buf[1] << "bame";
				cout << buf[2] << "bame";
				cout << buf[3] << "bame";
				cout << buf[4] << "bame";

			
				if (bytesIn <= 0)
				{
					// Drop the client
					closesocket(sock);
					FD_CLR(sock, &master);
				}
				else
				{
					// Check to see if it's a command. \quit kills the server
					if (buf[0] == '\\')
					{
						// Is the command quit? 
						string cmd = string(buf, bytesIn);
						if (cmd == "\\quit")
						{
							running = false;
							break;
						}

						// Unknown command
						continue;
					}

					// Send message to other clients, and definiately NOT the listening socket

					for (int i = 0; i < master.fd_count; i++)
					{
						SOCKET outSock = master.fd_array[i];
						if (outSock != listening && outSock != sock)
						{
							ostringstream ss;
							ss << "SOCKET #" << sock << ": " << buf << "\r\n";
							string strOut = ss.str();

							send(outSock, strOut.c_str(), strOut.size() + 1, 0);
						}
					}
				}*/
			}
		}
	}

	// Remove the listening socket from the master file descriptor set and close it
	// to prevent anyone else trying to connect.
	FD_CLR(listening, &master);
	closesocket(listening);

	// Message to let users know what's happening.
	string msg = "Server is shutting down. Goodbye\r\n";

	while (master.fd_count > 0)
	{
		// Get the socket number
		SOCKET sock = master.fd_array[0];

		// Send the goodbye message
		send(sock, msg.c_str(), msg.size() + 1, 0);

		// Remove it from the master file list and close the socket
		FD_CLR(sock, &master);
		closesocket(sock);
	}

	// Cleanup winsock
	WSACleanup();

	//system("pause");
}



void BroadCastData( const CentralisedNetMessageBuffer* pMessage, int pReqLevel)
{
	
	// Send message to other clients, and definiately NOT the listening socket

					for (int i = 0; i < master.fd_count; i++)
					{
						SOCKET outSock = master.fd_array[i];
						if (outSock != listening && outSock != sock)
						{
					SendData(outSock, &pMessage, MR_NET_REQUIRED);
					
						}
					}	
	
	
}



void SendData(SOCKET socktoSendTo, const CentralisedNetMessageBuffer* pMessage, int pReqLevel)
{
	// First try to send buffered data
	if (socktoSendTo != INVALID_SOCKET) {
		BOOL lEndQueueLoop = (mOutQueueLen == 0);

		while (!lEndQueueLoop) {
			// Determine first batch to send
			int lToSend;

			if ((mOutQueueHead + mOutQueueLen) > MR_OUT_QUEUE_LEN) {
				lToSend = MR_OUT_QUEUE_LEN - mOutQueueHead;
			}
			else {
				lToSend = mOutQueueLen;
				lEndQueueLoop = TRUE;
			}

			int lReturnValue = send(socktoSendTo, (const char*)(mOutQueue + mOutQueueHead), lToSend, 0);

			if (lReturnValue >= 0) {
				mOutQueueLen -= lReturnValue;

				mOutQueueHead = (mOutQueueHead + lReturnValue) % MR_OUT_QUEUE_LEN;

				if (lReturnValue != lToSend) {
					lEndQueueLoop = TRUE;
				}
			}
			else {
				lEndQueueLoop = TRUE;

				if (WSAGetLastError() == WSAEWOULDBLOCK) {
					// nothing to do
				}
				else {
					//	TRACE("Communication error A %d\n", WSAGetLastError());
						//Disconnect();
				}
			}
		}
	}

	if (sock != INVALID_SOCKET) {
		int lToSend = MR_NET_HEADER_LEN + pMessage->mDataLen;

		int lReturnValue;						  // send return value

		if (mOutQueueLen > 0) {
			lReturnValue = 0;
		}
		else {
			lReturnValue = send(socktoSendTo, ((const char*)pMessage), lToSend, 0);

			// TRACE( "Send %d %d %d\n", lToSend, lToSend-lSent, lReturnValue );

			if ((lReturnValue == -1) && (WSAGetLastError() == WSAEWOULDBLOCK)) {
				//	ASSERT(FALSE);
				lReturnValue = 0;
			}
		}

		if (lReturnValue < 0) {
			//	TRACE("Communication error B %d\n", WSAGetLastError());
			//	Disconnect();
		}
		else if (lReturnValue != lToSend) {
			if ((lReturnValue > 0) || (pReqLevel == MR_NET_REQUIRED) || ((pReqLevel == MR_NET_TRY) && (mOutQueueLen < (MR_OUT_QUEUE_LEN / 4)))) {
				lToSend -= lReturnValue;

				if (lToSend > (MR_OUT_QUEUE_LEN - mOutQueueLen)) {
					// no space left
			//		TRACE("Output queue full\n");
			//		Disconnect();
			//		ASSERT(FALSE);
				}
				else {
					// put the remaining part of the message in the queue
					int lTail = (mOutQueueHead + mOutQueueLen) % MR_OUT_QUEUE_LEN;

					int lFirstBlocSize = min(lToSend, MR_OUT_QUEUE_LEN - lTail);
					int lSecondBlocSize = max(0, lToSend - lFirstBlocSize);

					//		ASSERT(lFirstBlocSize > 0);

					memcpy(mOutQueue + lTail, pMessage + lReturnValue, lFirstBlocSize);

					if (lSecondBlocSize > 0) {
						memcpy(mOutQueue, pMessage + lFirstBlocSize + lReturnValue, lSecondBlocSize);
					}

					mOutQueueLen += lToSend;
				}
			}
		}
	}
}
