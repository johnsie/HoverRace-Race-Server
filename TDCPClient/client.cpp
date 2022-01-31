
//This is a testing tool for that emulates HoverRace connecting to a race

#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include "client.h"
#pragma comment(lib, "ws2_32.lib")
#	define ASSERT(e) assert(e)
typedef signed char MR_Int8;
typedef unsigned char MR_UInt8;
typedef signed short MR_Int16;
typedef unsigned short MR_UInt16;
typedef signed int MR_Int32;
typedef unsigned int MR_UInt32;
typedef LONGLONG MR_Int64;
typedef DWORDLONG MR_UInt64;

#define s_addr  S_un.S_addr /* can be used for most tcp & ip code */
#define s_host  S_un.S_un_b.s_b2    // host on imp
#define s_net   S_un.S_un_b.s_b1    // network
#define s_imp   S_un.S_un_w.s_w2    // imp
#define s_impno S_un.S_un_b.s_b4    // imp #
#define s_lh    S_un.S_un_b.s_b3    // logical host

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



#define MRNM_SET_TIME                 1
#define MRNM_CREATE_MAIN_ELEM         2
#define MRNM_SET_MAIN_ELEM_STATE      3
#define MRNM_CREATE_AUTO_ELEM         4
// #define MRNM_FINISH_STATS          5
#define MRNM_CHAT_MESSAGE             6
#define MRNM_PLAYER_STATS             7
#define MRNM_SET_PERM_ELEMENT_STATE   8
#define MRNM_SEND_KEYID               9
#define MRNM_HIT_MESSAGE             10

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

using namespace std;
#define MR_MAX_NET_MESSAGE_LEN		255
int mWatchdog;
std::string mGameName;			/// just the track name
	// Create socket

int mOutQueueLen;
int mOutQueueHead;
MR_UInt8 mOutQueue[MR_OUT_QUEUE_LEN];
class CentralisedNetClientMessageBuffer
{
public:
	// MR_UInt16  mSendingTime:10; // 4ms of precision on a +-2sec range (if a datagram take more than 2 sec to travel..it will be droped (UDP only)
	// Not used yet

	typedef signed char MR_Int8;
	typedef unsigned char MR_UInt8;
	typedef signed short MR_Int16;
	typedef unsigned short MR_UInt16;
	typedef signed int MR_Int32;
	typedef unsigned int MR_UInt32;
	typedef LONGLONG MR_Int64;
	typedef DWORDLONG MR_UInt64;
	MR_UInt16 mDatagramNumber : 8;			  /// used only for datagrams (UDP)
	MR_UInt16 mDatagramQueue : 2;				  /// used only for datagrams (UDP)
	MR_UInt16 mMessageType : 6;
	MR_UInt8 mClient;
	MR_UInt8 mDataLen;
	MR_UInt8 mData[255];

	// int MessageLen()const { return mDataLen+5; }
};
SOCKET sock;

using namespace std;



	void main()
	{
		string ipAddress = "192.168.10.9";			// IP Address of the server
		int port = 9530;						// Listening port # on the server

		// Initialize WinSock

		WSAData data;
		WORD ver = MAKEWORD(2, 2);
		int wsResult = WSAStartup(ver, &data);
		if (wsResult != 0)
		{
			cerr << "Can't start Winsock, Err #" << wsResult << endl;
			return;
		}
		sock = socket(AF_INET, SOCK_STREAM, 0);

		if (sock == INVALID_SOCKET)
		{
			cerr << "Can't create socket, Err #" << WSAGetLastError() << endl;
			WSACleanup();
			return;
		}

		// Fill in a hint structure
		sockaddr_in hint;
		hint.sin_family = AF_INET;
		hint.sin_port = htons(port);
		inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

		// Connect to server
		int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
		if (connResult == SOCKET_ERROR)
		{
			cerr << "Can't connect to server, Err #" << WSAGetLastError() << endl;
			closesocket(sock);
			WSACleanup();
			return;
		}


		cout << "Connected" << endl;


















		// Do-while loop to send and receive data
		char buf[4096];
		string userInput;


		// Prompt the user for some text

			// Send the text
		//	int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);
		CentralisedNetClientMessageBuffer lOutputBuffer;

		SOCKADDR_IN lAddr;
		lAddr.sin_family = AF_INET;

		//	getsockname(mActiveInterface->mClient[0].GetSocket(), (struct sockaddr*) & lAddr, &lSize);

		lAddr.sin_family = AF_INET;
		// lAddr.sin_addr.s_addr = INADDR_ANY;
	//	lAddr.sin_port = htons(mActiveInterface->mTCPRecvPort);


		lOutputBuffer.mMessageType = MRNM_GET_GAME_NAME;
		lOutputBuffer.mClient = 4;
		lOutputBuffer.mDataLen = 8;
		//	*(int*)&(lOutputBuffer.mData[0]) = lAddr.sin_addr.s_addr;
		//	*(int*)&(lOutputBuffer.mData[4]) = lAddr.sin_port;
		int sendResult;


#define MR_NET_HEADER_LEN  (sizeof(CentralisedNetClientMessageBuffer) - MR_MAX_NET_MESSAGE_LEN)
		// First try to send buffered data
		if (sock != INVALID_SOCKET) {
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

				int lReturnValue = send(sock, (const char*)(mOutQueue + mOutQueueHead), lToSend, 0);

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
						//	Disconnect();
					}
				}
			}
		}

		if (sock != INVALID_SOCKET) {
			int lToSend = MR_NET_HEADER_LEN + lOutputBuffer.mDataLen;

			int lReturnValue;						  // send return value

			if (mOutQueueLen > 0) {
				lReturnValue = 0;
			}
			else {
				lReturnValue = send(sock, ((const char*)&lOutputBuffer), lToSend, 0);

				//	cout <<lReturnValue << endl;

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
				if ((lReturnValue > 0) || (MR_NET_REQUIRED == MR_NET_REQUIRED) || ((MR_NET_REQUIRED == MR_NET_TRY) && (mOutQueueLen < (MR_OUT_QUEUE_LEN / 4)))) {
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

						//	ASSERT(lFirstBlocSize > 0);

						memcpy(mOutQueue + lTail, &lOutputBuffer + lReturnValue, lFirstBlocSize);

						if (lSecondBlocSize > 0) {
							memcpy(mOutQueue, &lOutputBuffer + lFirstBlocSize + lReturnValue, lSecondBlocSize);
						}

						mOutQueueLen += lToSend;
					}
				}
			}
		}









		sendResult = 1;

		//	bool running = true;
		bool running = true;


		while (running)

		{
			// Wait for response



			int mInputMessageBufferIndex;
			mInputMessageBufferIndex = 0;
			CentralisedNetClientMessageBuffer mInputMessageBuffer;
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


			cout << "Recieved Message type" << mInputMessageBuffer.mMessageType << endl;


			switch (mInputMessageBuffer.mMessageType)
			{

				//handle recieved messages

			case 7:

				string s = "geeksforgeeks";

				int n = s.length();

				// declaring character array


				// copying the contents of the
				// string to char array

				char pMessage[120];
				//	strcpy(pMessage, s.c_str());

				CentralisedNetClientMessageBuffer lMessage;

				// lMessage.mSendingTime    = mSession.GetSimulationTime()>>2;
				lMessage.mMessageType = MRNM_CHAT_MESSAGE;
				lMessage.mDataLen = strlen(pMessage);

				if (lMessage.mDataLen > 0) {
					memcpy(lMessage.mData, pMessage, lMessage.mDataLen);

			//		Todo. Get this function working

			//		SendData(&lMessage, MR_NET_REQUIRED);


					break;


				}//switch to handle



			}//End while running



	// Gracefully close down everything
			closesocket(sock);
			WSACleanup();
		}

	}



	void SendData(const CentralisedNetClientMessageBuffer* pMessage, int pReqLevel)
	{
		// First try to send buffered data
		if (sock != INVALID_SOCKET) {
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

				int lReturnValue = send(sock, (const char*)(mOutQueue + mOutQueueHead), lToSend, 0);

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
				lReturnValue = send(sock, ((const char*)pMessage), lToSend, 0);

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

