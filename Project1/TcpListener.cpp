#include "Listener.h"



CTcpListener::CTcpListener(std::string ipAddress, int port, MessageRecievedHandler handler)
	: m_ipAddress(ipAddress), m_port(port), MessageReceived(handler)
{

}

CTcpListener::~CTcpListener()
{
	Cleanup();
}


// Send a message to the specified client
void CTcpListener::Send(int clientSocket, std::string msg)
{
	send(clientSocket, msg.c_str(), msg.size() + 1, 0);
}

// Initialize winsock
bool CTcpListener::Init()
{
	WSAData data;
	WORD ver = MAKEWORD(2, 2);

	int wsInit = WSAStartup(ver, &data);
	// TODO : Inform caller the error that occured

	return wsInit == 0;


}


// The main processing loop
void CTcpListener::Run()
{
	char buf[MAX_BUFFER_SIZE];

	while (true)
	{
		// Create a listening socket
		// 연결을 끊을 때마다(데이터를 모두 수신한다 해도)
		// 계속 서버에 연결할 수 있어야 하기 때문에
		// 다른 소켓을 생성한다.
		SOCKET listening = CreateSocket();
		if (listening == INVALID_SOCKET)
		{
			break;
		}

		SOCKET client = WaitForConnection(listening);
		if (client == INVALID_SOCKET)
		{
			// 클라이언트와 연결되면 청취하는 소켓을 닫음. <아무도 연결할 수 없음>
			// 왜냐하면 클라이언트가 연결되어 있는 동안 데이터를 수신하고 메인 클래스로 보낼 것이기 때문에
			closesocket(listening);

			int bytesReceived = 0;
			do
			{
				// 무언가를 받기 시작한다. 메모리를 0으로 만든다.
				ZeroMemory(buf, MAX_BUFFER_SIZE);
				// 내 클라이언트 소켓에 있는 소켓을 지정해야 한다.

				bytesReceived = recv(client, buf, MAX_BUFFER_SIZE, 0);
				if (bytesReceived > 0)
				{
					// 메세지를 메인프로그램에 보내어 확인시키고 싶다.
					// 생성자 부근에서 MessageReceived(handler) 가 메인프로그램에 말하는 방식
					// 하지만 메세지를 확인해야한다. 널값인지, 제대로된 메세제인지
					if (MessageReceived != NULL)
					{
						// 클라이언트가 누구인지, 실제 메세지, 메세지를 구성하는 방식
						// 문자열에 복사할 문자 수의 시작 인덱스와
						// 그 크기를 지정하여 수신된 바이트 수를 말하고
						MessageReceived(this, client, std::string(buf, 0, bytesReceived));
					}
				}

			} while (bytesReceived > 0); // 받은 바이트가 0보다 크면 계속 반복. 즉 0이되면 종료

			closesocket(client); // 메세지를 다 받으면 소켓닫기. 메시지를 다받으면 클라이언트 연결 종료
		}

	}

}

void CTcpListener::Cleanup()
{
	WSACleanup();
}

SOCKET CTcpListener::CreateSocket()
{

	// 1. 청취 소켓 생성
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0); // IP 버전 4 유형 TCP 프로토콜, 프로토콜이 채워지기 때문에 0을 지정

	// 2. 유효하지 않은지 확인.
	if (listening != INVALID_SOCKET)
	{
		// 3. 해당 ip주소와 포트번호를 소켓에 바인딩하는데 사용할 구조변수

		sockaddr_in hint; // ip 4유형을 씀을 알려주기 때문에 hint라는 이름으로 지정
		hint.sin_family = AF_INET;
		hint.sin_port = htons(m_port); // 위에서 생성자 지정한 m_port
		// 문자열의 주소를 지정하기, 이부분이 중요함. c++과 다른 언어에 알리기 위한 방법
		inet_pton(AF_INET, m_ipAddress.c_str(), &hint.sin_addr);

		// 소켓이 이제 해당 IP 주소 및 포트 튜플에 연결되도록 하여 해당 소켓을 참조할 때 실제로 참조하는 것이다.

		int bindOk = bind(listening, (sockaddr*)&hint, sizeof(hint));

		if (bindOk != SOCKET_ERROR)
		{
			// 오류가 발생하지 않으면 바인딩을 시도한 다음
			// 해당 소켓에서 수신을 시도
			int listenOk = listen(listening, SOMAXCONN);
			if (listenOk == SOCKET_ERROR)
			{
				// 실패하면 -1을 반환.
				return -1;
			}
		}
		else
		{
			return -1;
		}
	}

	// 청취 소켓은 -1을 포함한다. 어떤 의미지인지 이해를 잘해야함.
	return listening;
}
// wait for a connection
SOCKET CTcpListener::WaitForConnection(SOCKET listening)
{
	// 연결을 기다린다면
	// 클라이언트가 수락하는 소켓이 될 것이다.
	SOCKET client = accept(listening, NULL, NULL);

	return client;
}