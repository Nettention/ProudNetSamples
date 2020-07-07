#include "stdafx.h"

#include "ProudNetServer.h"

using namespace std;
using namespace Proud;

#include "../Common/Vars.h"
#include "../Common/Simple_common.cpp"
#include "../Common/Simple_proxy.cpp"
#include "../Common/Simple_stub.cpp"

// Server-to-client RMI stub
// For details, check client source code first.
class SimpleStub: public Simple::Stub
{
public:
	DECRMI_Simple_Chat;
};

// and its instance
SimpleStub g_SimpleStub;

// Server-to-client RMI proxy
Simple::Proxy g_SimpleProxy;

// a P2P group where all clients are in.
HostID g_groupHostID = HostID_None;

DEFRMI_Simple_Chat(SimpleStub)
{
	cout << "[Server] chat message received :";
	cout << " a=" << string(a);
	cout << " b=" << b;
	cout << " c=" << c;
	cout << endl;

	// Echo chatting message which received from server to client.
	g_SimpleProxy.ShowChat(remote, RmiContext::ReliableSend, a, b + 1, c + 1);

	return true;
}


int main(int argc, char* argv[])
{
	// Network server instance.
	shared_ptr<CNetServer> srv(CNetServer::Create());

	// set a routine which is executed when a client is joining.
	// clientInfo has the client info including its HostID.
	srv->OnClientJoin = [](CNetClientInfo *clientInfo){
		printf("Client %d connected.\n", clientInfo->m_HostID);
	};

	// set a routine for client leave event.
	srv->OnClientLeave = [](CNetClientInfo *clientInfo, ErrorInfo *errorInfo, const ByteArray& comment){
		printf("Client %d disconnected.\n", clientInfo->m_HostID);
	};

	// Associate RMI proxy and stub instances to network object.
	srv->AttachStub(&g_SimpleStub);
	srv->AttachProxy(&g_SimpleProxy);

	CStartServerParameter p1;
	p1.m_protocolVersion = g_Version; // This must be the same to the client.
	p1.m_tcpPorts.Add(g_ServerPort); // TCP listening endpoint

	ErrorInfoPtr err;
	try
	{
		/* Starts the server.
		This function throws an exception on failure.
		Note: As we specify nothing for threading model,
		RMI function by message receive and event callbacks are
		called in a separate thread pool.
		You can change the thread model. Check out the help pages for details. */
		srv->Start(p1);
	}
	catch (Exception &e)
	{
		cout << "Server start failed: " << e.what() << endl;
		return 0;
	}

	puts("Server started. Enterable commands:\n");
	puts("1: Creates a P2P group where all clients join.\n");
	puts("2: Sends a message to P2P group members.\n");
	puts("q: Quit.\n");

	string userInput;

	while (true)
	{
		// get user input
		cin >> userInput;

		if ( userInput == "1" )
		{
			// get all client HostID array.
			vector<HostID> clients;
			int noofClients = srv->GetClientCount();
			clients.resize(noofClients);
			int listCount = srv->GetClientHostIDs(&clients[0], noofClients);

			// create a P2P group where all clients are in.
			g_groupHostID = srv->CreateP2PGroup(&clients[0], clients.size());
		}
		else if (userInput == "2")
		{
			// send an RMI message to every client.
			g_SimpleProxy.SystemChat(g_groupHostID, RmiContext::ReliableSend, _PNT("Hello~~~!"));
		}
		else if (userInput == "3")
		{
			// destroy the P2P group.
			srv->DestroyP2PGroup(g_groupHostID);
			g_groupHostID = HostID_None;
		}
		else if (userInput == "q")
		{
			// exit program.
			break;
		}
	}

	return 0;
}



