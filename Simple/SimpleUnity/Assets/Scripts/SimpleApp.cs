using UnityEngine;
using System.Collections;
using Nettention.Proud;
using System.Collections.Generic;

namespace SimpeProudUnityClient
{
	internal class ClientEventSink
	{
		NetClient m_client;
		Simple.Proxy m_simpleProxy;
		public bool IsConnectWaiting;
		public bool IsConnected;
		public HostID GroupID;

		public ClientEventSink(NetClient client, Simple.Proxy proxy)
		{
			m_client = client;
			m_simpleProxy = proxy;

			IsConnectWaiting = true;
			IsConnected = false;
			GroupID = HostID.HostID_None;

			m_client.JoinServerCompleteHandler = OnJoinServerComplete;
			m_client.LeaveServerHandler = OnLeaveServer;
			m_client.P2PMemberJoinHandler = OnP2PMemberJoin;
			m_client.P2PMemberLeaveHandler = OnP2PMemberLeave;

			m_client.ErrorHandler = OnError;
			m_client.WarningHandler = OnWarning;
			m_client.ExceptionHandler = OnException;
			m_client.InformationHandler = OnInformation;

			m_client.NoRmiProcessedHandler = OnNoRmiProcessed;
			m_client.ChangeServerUdpStateHandler = OnChangeServerUdp;
			m_client.ReceivedUserMessageHandler = OnReceiveUserMessage;
		}

		public void OnJoinServerComplete(ErrorInfo info, ByteArray replyFromServer)//
		{
			IsConnectWaiting = false;
			if (info.errorType == ErrorType.Ok)
			{
                SimpleApp.print("Server connection ok. Client HostID = " + m_client.LocalHostID);
				IsConnected = true;
			}
			else
			{
                SimpleApp.print("Server connection failed.");
			}
		}

		public void OnLeaveServer(ErrorInfo errorInfo)//
		{
			IsConnected = false;
		}

		public void OnP2PMemberJoin(HostID memberHostID, HostID groupHostID, int memberCount, ByteArray customField)
		{
            SimpleApp.print("[Client] P2P member " + memberHostID + " joined  group " + groupHostID + ".");

			this.GroupID = groupHostID;

			if (memberHostID != m_client.LocalHostID)
			{
				m_simpleProxy.P2PChat(memberHostID, RmiContext.ReliableSend, "Hello~~", 1, 1);
			}
		}

		public void OnP2PMemberLeave(HostID memberHostID, HostID groupHostID, int memberCount)
		{
            SimpleApp.print("[Client] P2P member " + memberHostID + " left group " + groupHostID + ".");
		}

		public void OnError(ErrorInfo errorInfo)
		{
            SimpleApp.print("Error : " + errorInfo.ToString());
		}

		public void OnException(HostID remoteID, System.Exception e)
		{
            SimpleApp.print("exception : " + e.ToString());
		}

		public void OnInformation(ErrorInfo errorInfo)
		{
            SimpleApp.print("Information " + errorInfo.ToString());
		}

		public void OnNoRmiProcessed(RmiID rmiID)
		{
            SimpleApp.print("NoRmiProcessed : " + rmiID);
		}

		public void OnWarning(ErrorInfo errorInfo)
		{
            SimpleApp.print("Warinig :  " + errorInfo.ToString());
		}

		public void OnChangeServerUdp(ErrorType reason)
		{
            SimpleApp.print("ChangeServerUdp " + reason);
		}

		public void OnReceiveUserMessage(HostID sender, RmiContext rmiContext, ByteArray payload)
		{
            SimpleApp.print("ReceiveUserMessage HostID : " + sender);
		}
	}



	public class SimpleApp : MonoBehaviour
	{

		NetClient m_client;
		NetConnectionParam m_connectionParam;

		ClientEventSink m_eventSink;

        Simple.Proxy m_simpleProxy;
        Simple.Stub m_simpleStub;

		SimpleUnity.MyClass m_myClass;

		int m_screenWidth; // 현재 화면의 가로 값 셋팅.
		int m_screenHeight; // 현재 화면의 세로 값 셋팅.
		int m_screenOneW; // 현재 화면의 가로를 5로 나눈값으로 셋팅.
		int m_screenOneH; // 현재 화면의 세로를 10으로 나눈 값으로 셋팅.

		int m_height;
		Vector2 m_scrollPosition;
		float m_scrollHeight;

		List<int> m_listData;
		Dictionary<int, float> m_dictionaryData;
		ByteArray m_blockData;

		// 창에 뿌려질 내용에 대한 list.
		public List<string> LogList;

		// 코루틴에 사용할 대기시간. WaitForSeconds() 는 초단위.
		float m_WaitTime = 0.01f;

		// Use this for initialization
		IEnumerator Start()
		{
			m_client = new NetClient();
			m_connectionParam = new NetConnectionParam();

			m_simpleProxy = new Simple.Proxy();
			m_simpleStub = new Simple.Stub();

			m_eventSink = new ClientEventSink(m_client, m_simpleProxy);

			m_myClass = new SimpleUnity.MyClass();

			m_simpleStub.ShowChat = ShowChat;
			m_simpleStub.SystemChat = SystemChat;
			m_simpleStub.P2PChat = P2PChat;

			m_client.AttachProxy(m_simpleProxy);
			m_client.AttachStub(m_simpleStub);

            m_connectionParam.protocolVersion = new Nettention.Proud.Guid();
            m_connectionParam.protocolVersion.Set(SimpleUnity.Vars.Version);

			// 웹플레이어로 접속시에는 localhost 가 아닌 IP를 써주어야 함.
			m_connectionParam.serverIP = "localhost";
			m_connectionParam.serverPort = (ushort)SimpleUnity.Vars.ServerPort;

			m_screenWidth = Screen.width; // 현재 화면의 가로 값 셋팅.
			m_screenHeight = Screen.height; // 현재 화면의 세로 값 셋팅.
			m_screenOneW = m_screenWidth / 5; // 현재 화면의 가로를 5로 나눈값으로 셋팅.
			m_screenOneH = m_screenHeight / 10; // 현재 화면의 세로를 10으로 나눈 값으로 셋팅.

			m_height = m_screenOneH * 7; // 채팅창 높이 초기화.

			m_scrollHeight = 0;

			LogList = new List<string>();

#if UNITY_WEBPLAYER
			Security.PrefetchSocketPolicy(m_connectionParam.serverIP, (int)m_connectionParam.serverPort);
#endif

			if (m_client.Connect(m_connectionParam) == false)
			{
				print("m_client failed to connect to server!!");
			}
			else
			{

				long time = System.DateTime.Now.Ticks;
				long currentTime;

				while (m_eventSink.IsConnectWaiting)
				{
					// 코루틴을 이용해 m_WaitTime 동안 대기.
					yield return new WaitForSeconds(m_WaitTime);
					m_client.FrameMove();

					currentTime = System.DateTime.Now.Ticks;

					// ticks 의 1초는 10000000
					if (currentTime - time >  100000000)
						break;
				}

                m_myClass.a = 1;
				m_myClass.b = 1.5f;
				m_myClass.c = 3.141592;

				// List혹은 Dictionary를 Rmi인자로 사용이 가능합니다.
				m_listData = new List<int>();
				m_listData.Add(4);
				m_listData.Add(5);

				m_dictionaryData = new Dictionary<int, float>();
				m_dictionaryData.Add(1, 1.5f);
				m_dictionaryData.Add(2, 3.1f);

				m_blockData = new ByteArray();
				for (int i = 0; i < 100; ++i)
					m_blockData.Add((byte)i);

				m_simpleProxy.Chat(HostID.HostID_Server, RmiContext.ReliableSend, "우갸갸갸갸갹.", 22, 22.33f);
			}

		}

		// Update is called once per frame
		void Update()
		{
			if (m_eventSink.IsConnected)
			{
				m_client.FrameMove();
			}

			// 키가 입력이 되었는데 입력된 키의 KeyCode 가 Escape 라면.(안드로이드 상에서 뒤로가기 버튼)
			if(Input.GetKeyDown(KeyCode.Escape)) 
			{
				Application.Quit (); // 어플리케이션 종료.
			}
		}

		void OnGUI()
		{

			/*
		 	* GUI label, button 의 폰트 사이즈를 설정.
		 	* */
#if UNITY_ANDROID || UNITY_IPHONE
			GUI.skin.label.fontSize = 30;
			GUI.skin.button.fontSize = 30;
#endif

			GUI.Box(new Rect(m_screenWidth / 2 - m_screenOneW * 2, m_screenOneH, m_screenOneW * 4, m_height), "");

			m_scrollHeight = 0;

			// 채팅 내용이 얼마만큼의 길이로 표시가 되어야 하는지 계산.
			foreach (string str in LogList)
			{
				m_scrollHeight = m_scrollHeight + (int)GUI.skin.label.CalcHeight(new GUIContent(str), (float)(m_screenOneW * 4));
			}

			if (m_height > m_scrollHeight)
			{
				m_scrollHeight = m_height;
			}

			// 채팅창 스크롤바.
			m_scrollPosition = GUI.BeginScrollView(new Rect(m_screenWidth / 2 - m_screenOneW * 2, m_screenOneH, m_screenOneW * 4, m_height), m_scrollPosition, new Rect(m_screenWidth / 2 - m_screenOneW * 2, m_screenOneH, m_screenOneW * 4, m_scrollHeight), GUIStyle.none, GUI.skin.verticalScrollbar);

			float h = 0;

			foreach (string str in LogList)
			{
				float labelHeight = 0;
				labelHeight = GUI.skin.label.CalcHeight(new GUIContent(str), (float)(m_screenOneW * 4));
				GUI.Label(new Rect(m_screenWidth / 2 - m_screenOneW * 2, m_screenOneH + h, m_screenOneW * 4, labelHeight), str);
				h = h + labelHeight;
			}

			// 스크롤바가 그려질 영역이 끝낫음을 의미.
			GUI.EndScrollView();

			if (GUI.Button(new Rect(m_screenWidth / 2 - m_screenOneW, m_screenOneH * 8, m_screenOneW, m_screenOneH), "Send"))
			{
                m_simpleProxy.Chat(HostID.HostID_Server, RmiContext.ReliableSend, "메롱.", 22, 22.33f);
			}

			if (GUI.Button(new Rect(m_screenWidth / 2, m_screenOneH * 8, m_screenOneW, m_screenOneH), "P2PSend"))
			{
                if (m_eventSink.GroupID != HostID.HostID_Server)
					m_simpleProxy.P2PChat(m_eventSink.GroupID, RmiContext.ReliableSend, "Welcome ProudNet!!", 1, 1);
			}

			if (GUI.Button(new Rect(m_screenWidth / 2 + m_screenOneW, m_screenOneH * 8, m_screenOneW, m_screenOneH), "QUIT"))
			{
				Application.Quit(); // 어플리케이션 종료.
			}
		}

		void OnDestroy()
		{
			m_client.Disconnect();
		}


		// RMI stub
		public bool ShowChat(HostID remote, RmiContext rmiContext, string a, int b, float c)
		{
			LogList.Add("[m_client] ShowChat : a=" + a + ", b=" + b + ", c=" + c);
			m_scrollPosition.y = m_scrollHeight;

			return true;
		}

		public bool SystemChat(HostID remote, RmiContext rmiContext, string txt)
		{
			LogList.Add("[m_client] SystemChat : txt=" + txt);
			m_scrollPosition.y = m_scrollHeight;

			return true;
		}

		public bool P2PChat(HostID remote, RmiContext rmiContext, string a, int b, float c)
		{
			string relayed = "";
			if (rmiContext.relayed == true)
			{
				relayed = "true";
			}
			else
			{
				relayed = "false";
			}
			LogList.Add("[m_client] P2PChat relayed:" + relayed + ", a=" + a + ", b=" + b + ", c=" + c);
			m_scrollPosition.y = m_scrollHeight;

			return true;
		}
	}
}