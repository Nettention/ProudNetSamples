#include "stdafx.h"

#include "ProudNetClient.h"

using namespace std;
using namespace Proud;

#include "../Common/Vars.h"
#include "../Common/Simple_common.cpp"
#include "../Common/Simple_proxy.cpp"
#include "../Common/Simple_stub.cpp"

/* Protects all variable here. 
If you are developing a game program or an app with fast-running loop, 
this is not needed, as you are making a single threaded app.
This is required as this sample program uses two threads. 
One is for waiting user input, the other is for processing 
received messages and events. */
CriticalSection g_critSec;

/* Client-to-server RMI proxy.
RMI proxy is used for sending messages aka. 
calling a function which resides other process.
*/
Simple::Proxy g_SimpleProxy;

/* RMI stub for receiving messages.
Unlike RMI proxy, it is derived and RMI function is 
implemented by function override. */
class SimpleStub : public Simple::Stub
{
public:
	DECRMI_Simple_ShowChat;
	DECRMI_Simple_SystemChat;

	DECRMI_Simple_P2PChat;
};

// RMI stub instance.
SimpleStub g_SimpleStub;

// RMI function implementation that you define.
// We use DEFRMI_GlobalName_FunctionName for easier coding.
DEFRMI_Simple_P2PChat(SimpleStub)
{
	CriticalSectionLock lock(g_critSec, true);
	cout << "[Client] P2PChat: relayed="
		<< (rmiContext.m_relayed ? "true" : "false")
		<< " a=" << string(a)
		<< " b=" << b
		<< " c=" << c
		<< endl;

	// no meaning, but must return true always.
	return true;
}

DEFRMI_Simple_ShowChat(SimpleStub)
{
	CriticalSectionLock lock(g_critSec, true);
	cout << "[Client] ShowChat: a=" << (string)a << ",b=" << b << ",c=" << c << endl;
	return true;
}

DEFRMI_Simple_SystemChat(SimpleStub)
{
	CriticalSectionLock lock(g_critSec, true);
	cout << "[Client] SystemChat: txt=" << (string)txt << endl;
	return true;
}

int main(int argc, char* argv[])
{
	// Network client object.
	shared_ptr<CNetClient> netClient(CNetClient::Create());

	// set to false to exit the main loop.
	volatile bool keepWorkerThread = true;
	// set to true if server connection is established.
	bool isConnected = false;
	// changed if another P2P peer joined.
	HostID recentP2PGroupHostID = HostID_None;

	// set a routine which is run when the server connection attempt
	// is success or failed.
	netClient->OnJoinServerComplete = 
		[&](ErrorInfo *info, const ByteArray &replyFromServer)
	{
		// as here is running in 2nd thread, lock is needed for console print.
		CriticalSectionLock lock(g_critSec, true);

		if (info->m_errorType == ErrorType_Ok)
		{
			// connection successful.
			printf("Succeed to connect server. Allocated hostID=%d\n", netClient->GetLocalHostID());
			isConnected = true;

			// send a message.
			g_SimpleProxy.Chat(HostID_Server, // send destination
				RmiContext::ReliableSend, // how to send
				_PNT("Hello ProudNet~!!!."), 333, 22.33f); // user defined parameters

		}
		else
		{
			// connection failure.
			cout << "Failed to connect to server.\n";
		}
	};

	// set a routine for network disconnection.
	netClient->OnLeaveServer = [&](ErrorInfo *errorInfo) {
		// lock is needed as above.
		CriticalSectionLock lock(g_critSec, true);

		// show why disconnected.
		cout << "OnLeaveServer: " << StringT2A(errorInfo->m_comment).GetString() << endl;

		// let main loop exit
		isConnected = false;
		keepWorkerThread = false;
	};

	// set a routine for P2P member join (P2P available)
	netClient->OnP2PMemberJoin = [&](HostID memberHostID, HostID groupHostID,int memberCount, const ByteArray &customField) {
		// lock as the above
		CriticalSectionLock lock(g_critSec, true);

		// memberHostID = P2P connected client ID
		// groupHostID = P2P group ID where the P2P peer is in.
		printf("[Client] P2P member %d joined group %d.\n", memberHostID, groupHostID);

		g_SimpleProxy.P2PChat(memberHostID, RmiContext::ReliableSend,
			_PNT("Welcome!"), 5, 7.f);
		recentP2PGroupHostID = groupHostID;
	};

	// called when a P2P member left.
	netClient->OnP2PMemberLeave = [](HostID memberHostID, HostID groupHostID,int memberCount) {
		printf("[Client] P2P member %d left group %d.\n", memberHostID, groupHostID);
	};

	// attach RMI proxy and stub to client object.
	netClient->AttachProxy(&g_SimpleProxy);	
	netClient->AttachStub(&g_SimpleStub);	

	CNetConnectionParam cp;

	// Protocol version which have to be same to the value at the server.
	cp.m_protocolVersion = g_Version;
    cp.m_closeNoPingPongTcpConnections=false;

	// server address
	if (argc > 1)
	{
		cp.m_serverIP = StringA2T(argv[1]);
	}
	else
	{
		cp.m_serverIP = _PNT("localhost");
	}

	// server port
	cp.m_serverPort = g_ServerPort;		

	// Starts connection.
	// This function returns immediately.
	// Meanwhile, connection attempt is process in background
	// and the result is notified by OnJoinServerComplete event.
	netClient->Connect(cp);

	// As we have to be notified events and message receivings,
	// We call FrameMove function for every short interval.
	// If you are developing a game, call FrameMove
	// without doing any sleep. 
	// If you are developing an app, call FrameMove
	// in another thread or your timer callback routine.
	Proud::Thread workerThread([&](){
		while (keepWorkerThread)
		{
			// Prevent CPU full usage.
			Proud::Sleep(10);		

			// process received RMI and events.
			netClient->FrameMove();
		}
	});
	workerThread.Start();

	cout << "a: Send a P2P message to current P2P group members except for self.\n";
	cout << "q: Exit.\n";

	while (keepWorkerThread)
	{
		// get user input
		string userInput;
		cin >> userInput;

		// received user command. process it.
		if (userInput == "q")
		{
			// let worker thread exit.
			keepWorkerThread = false;
		}
		else if (userInput == "a")
		{
			if (isConnected)
			{
				// As we access recentP2PGroupHostID which is also accessed by
				// another thread, lock it.
				CriticalSectionLock lock(g_critSec, true);

				// Sends a P2P message to everyone in a group 
				// specified at recentP2PGroupHostID.
				RmiContext sendHow = RmiContext::ReliableSend;
				sendHow.m_enableLoopback = false; // don't sent to myself.
				g_SimpleProxy.P2PChat(recentP2PGroupHostID, sendHow, 
					_PNT("Welcome ProudNet!!"), 1, 1);
			}
			else
			{
				// We have no server connection. Show error.
				cout << "Not yet connected.\n";
			}
		}
	}

	// Waits for 2nd thread exits.
	workerThread.Join();

	// Disconnects.
	// Note: deleting NetClient object automatically does it.
	netClient->Disconnect();

	return 0;
}
