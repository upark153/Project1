#pragma once

#include <WS2tcpip.h> // Header file for winsock functions
#include <string>
#pragma comment(lib, "ws2_32.lib") // winsock library file

#define MAX_BUFFER_SIZE (49152)
//Forward declaration of class
class CTcpListener;


// Callback to data received
typedef void (*MessageRecievedHandler)(CTcpListener* listener, int socketId, std::string msg);

class CTcpListener
{
public:

	CTcpListener(std::string ipAddress, int port, MessageRecievedHandler handler);

	~CTcpListener();


	// 특정한 클라이언트에게만 보내기
	void Send(int clientSocket, std::string msg);

	// 소켓 초기화
	bool Init();


	// 클라이언트 연결을 받는 메인 루프
	void Run();

	// 서버를 이용한 뒤에 초기화하기.
	void Cleanup();


private:

	// Create a socket
	SOCKET CreateSocket();
	// wait for a connection
	SOCKET WaitForConnection(SOCKET listening);





	std::string				m_ipAddress;
	int						m_port;
	MessageRecievedHandler  MessageReceived;

};
