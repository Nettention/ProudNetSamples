#include "ProudNetActor.h"

#include "EngineUtils.h"
#include "SimpleUE4GameModeBase.h"
#include "../../../Simple/Common/Simple_common.cpp"
#include "../../../Simple/Common/Simple_proxy.cpp"
#include "../../../Simple/Common/Simple_stub.cpp"

// RMI 송신을 위한 프라우드넷 프록시 객체입니다.
Simple::Proxy g_simpleProxy;

// RMI 수신을 위한 프라우드넷 스텁 객체입니다.
SimpleStub g_simpleStub;

// P2P 그룹에 join하게 되면, join된 그룹의 HostID가 이 전역 변수에 저장되어집니다.
Proud::HostID g_recentP2PGroupHostID = Proud::HostID_None;

// 화면에 보여질 메세지입니다.
Proud::String g_message = _PNT("");

// 하나의 P2P 그룹 내의 피어들 간의 채팅을 위한 RMI 스텁함수입니다.
DEFRMI_Simple_P2PChat(SimpleStub)
{
	std::wstringstream tstrstream;

	tstrstream << "[Client] P2PChat: relayed="
		<< (rmiContext.m_relayed ? "true" : "false")
		<< " a=" << a.GetString()
		<< " b=" << b
		<< " c=" << c;

	g_message += Proud::String(tstrstream.str()) + LINE_TERMINATOR;

	// 현재는 RMI 스텁함수에서는 관행적으로 true를 반환해야 됩니다.
	// 반환값에는 의미가 없습니다.
	return true;
}

// 서버가 보내주는 채팅 echo 메세지를 위한 RMI 스텁함수입니다.
DEFRMI_Simple_ShowChat(SimpleStub)
{
	std::wstringstream tstrstream;

	tstrstream << "[Client] ShowChat: a=" << a.GetString() << ",b=" << b << ",c=" << c;

	g_message += Proud::String(tstrstream.str()) + LINE_TERMINATOR;

	// 현재는 RMI 스텁함수에서는 관행적으로 true를 반환해야 됩니다.
	// 반환값에는 의미가 없습니다.
	return true;
}

// 서버의 공지 메세지를 위한 RMI 스텁함수입니다.
DEFRMI_Simple_SystemChat(SimpleStub)
{
	std::wstringstream tstrstream;

	tstrstream << "[Client] SystemChat: txt=" << txt.GetString();

	g_message += Proud::String(tstrstream.str()) + LINE_TERMINATOR;

	// 현재는 RMI 스텁함수에서는 관행적으로 true를 반환해야 됩니다.
	// 반환값에는 의미가 없습니다.
	return true;
}

AProudNetActor::AProudNetActor()
	: m_pNetClient(Proud::CNetClient::Create())
{
	// Tick 함수가 매 프레임마다 호출될 수 있도록 해줍니다.
	PrimaryActorTick.bCanEverTick = true;
}

// 게임이 시작된 직후 혹은 이 Actor가 Spawn된 직후에 호출되어집니다.
void AProudNetActor::BeginPlay()
{
	Super::BeginPlay();

	// 서버로 커넥션이 맺어졌는지 여부를 저장하고 있습니다.
	bool isConnected = false;

	// 서버로의 커넥션 시도가 완료되면 호출되어집니다.
	m_pNetClient->OnJoinServerComplete =
		[&](Proud::ErrorInfo* info, const Proud::ByteArray& replyFromServer)
	{
		if (info->m_errorType == Proud::ErrorType::Ok)
		{
			// 서버로의 커넥션에 성공하였습니다.
			printf("Succeed to connect server. Allocated hostID=%d\n", m_pNetClient->GetLocalHostID());
			isConnected = true;

			// 서버로 메세지를 보냅니다.
			g_simpleProxy.Chat(Proud::HostID_Server, // 수신자의 HostID입니다.
				Proud::RmiContext::ReliableSend, // Reliable 혹은 UnReliable로 지정하실 수 수 있습니다.
				_PNT("Hello ProudNet~!!!."), 333, 22.33f); // PIDL 파일의 함수 시그니쳐로 지정하실 수 있는 유저가 정의한 파라미터들입니다.

		}
		else
		{
			// 서버로의 커넥션에 실패하였습니다.
			std::cout << "Failed to connect to server.\n";
		}
	};

	// 서버와의 커넥션이 끊겼으면 호출되어집니다.
	m_pNetClient->OnLeaveServer = [&](Proud::ErrorInfo* errorInfo) {
		// 왜 커넥션이 끊겼는지 출력해봅니다.
		std::cout << "OnLeaveServer: " << Proud::StringT2A(errorInfo->m_comment).GetString() << std::endl;

		// 메인 루프가 break되도록 합니다.
		isConnected = false;
	};

	// 속해있는 P2P 그룹에 다른 피어가 Join하였을시에 호출되어집니다.
	m_pNetClient->OnP2PMemberJoin = [&](Proud::HostID memberHostID, Proud::HostID groupHostID, int memberCount, const Proud::ByteArray& customField) {
		// memberHostID = 이 콜백이 호출되어지게 만든 새로 join된 피어의 HostID입니다.
		// groupHostID = 속해있는 P2P 그룹의 HostID입니다.
		printf("[Client] P2P member %d joined group %d.\n", memberHostID, groupHostID);

		g_simpleProxy.P2PChat(memberHostID, Proud::RmiContext::ReliableSend,
			_PNT("Welcome!"), 5, 7.f);

		g_recentP2PGroupHostID = groupHostID;
	};

	// 속해있는 P2P 그룹에서 피어가 나가면 호출되어집니다.
	m_pNetClient->OnP2PMemberLeave = [](Proud::HostID memberHostID, Proud::HostID groupHostID, int memberCount) {
		printf("[Client] P2P member %d left group %d.\n", memberHostID, groupHostID);
	};

	// 클라이언트에 생성한 프록시 객체와 스텁 객체를 부착시킵니다.
	m_pNetClient->AttachProxy(&g_simpleProxy);
	m_pNetClient->AttachStub(&g_simpleStub);

	Proud::CNetConnectionParam cp;
	cp.m_protocolVersion = g_Version;
	cp.m_serverIP = _PNT("127.0.0.1");
	cp.m_serverPort = g_ServerPort;

	m_pNetClient->Connect(cp);
}

// 매 프레임마다 호출되어집니다.
void AProudNetActor::Tick(float deltaTimeSecond)
{
	Super::Tick(deltaTimeSecond);

	m_pNetClient->FrameMove();
}