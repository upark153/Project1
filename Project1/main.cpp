#include <iostream>
#include <string>

#include "Listener.h"

using namespace std;

void Listener_MessageReceived(CTcpListener* listener, int client, string msg);

/*  
	1. 서버 생성
	2. 서버 초기화
	3. 실행
	4. 메시지를 수신할 때마다 일종의 작업 수행

*/
void main()
{
	CTcpListener server("127.0.0.1", 9001, Listener_MessageReceived);

	if (server.Init())
	{
		server.Run();
	}


}

// 실제 TcpListener.cpp 에서 수신완료한 메세지를 아래 함수에게 최종적으로 전달한다.
void Listener_MessageReceived(CTcpListener* listener, int client, string msg)
{
	listener->Send(client, msg);
}
