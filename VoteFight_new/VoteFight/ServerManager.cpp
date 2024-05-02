#include "pch.h"

#include "./ImaysNet/ImaysNet.h"
#include "./ImaysNet/PacketQueue.h"

#include "ServerManager.h"
#pragma comment(lib, "WS2_32.LIB")

// ���� IP
//char* CServerManager::SERVERIP;
char* CServerManager::m_SERVERIP = "127.0.0.1";

// ����� mutex
recursive_mutex CServerManager::m_mutex;

// ���� ��ü
shared_ptr<Socket> CServerManager::m_tcpSocket;

// Ŭ���̾�Ʈ ID
int	CServerManager::m_id{ -1 };

// ���� ���� �ݹ� �Լ�
void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD recv_flag)
{
	// ���ŵ� �����͸� ���� ���� ���� ���ۿ� ���ŵ� ������ ����
	char* recv_buf = reinterpret_cast<EXP_OVER*>(recv_over)->m_buf;
	int recv_buf_Length = num_bytes;

	{ 
		// ���ŵ� ������ ó��
		int remain_data = recv_buf_Length + CServerManager::m_tcpSocket->m_prev_remain;
		while (remain_data > 0) {    					// ��Ŷ�� ũ�⸦ Ȯ���Ͽ� ó��
			unsigned char packet_size = recv_buf[0];	// ��Ŷ ũ�� ����
		
			if (packet_size > remain_data)				// ��Ŷ ũ�Ⱑ ���� �����ͺ��� ũ�� ����
				break;
			else if (packet_size == 0) {				// ��Ŷ ũ�Ⱑ 0�̾ ����
				remain_data = 0;
				break;
			}

			CServerManager::PacketProcess(recv_buf);   // ��Ŷ ó��

			recv_buf += packet_size;					// ���� ��Ŷ���� �̵�
			remain_data -= packet_size;					// ���� ������ ����
		}

		// ���� ������ ����
		CServerManager::m_tcpSocket->m_prev_remain = remain_data;

		// ���� �����Ͱ� ������ �� ������ ����ü�� ����
		if (remain_data > 0) {
			memcpy(CServerManager::m_tcpSocket->m_recvOverlapped.m_buf, recv_buf, remain_data);
		}
	}

	// ���� ������ ������ �޸� �ʱ�ȭ
	memset(CServerManager::m_tcpSocket->m_recvOverlapped.m_buf + CServerManager::m_tcpSocket->m_prev_remain, 0,
		sizeof(CServerManager::m_tcpSocket->m_recvOverlapped.m_buf) - CServerManager::m_tcpSocket->m_prev_remain);
	
	// ������ ����ü �ʱ�ȭ
	memset(&CServerManager::m_tcpSocket->m_recvOverlapped.m_wsa_over, 0, sizeof(CServerManager::m_tcpSocket->m_recvOverlapped.m_wsa_over));
	
	// ���� ������ ����
	CServerManager::Do_Recv();
}

// �۽� �ݹ� �Լ�
void CALLBACK send_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED send_over, DWORD recv_flag)
{
	// �۽��� �Ϸ�� �� ȣ��Ǵ� �ݹ� �Լ�
	// ������ ����ü �����Ͽ� �޸� ���� ����
	delete reinterpret_cast<EXP_OVER*>(send_over);
}

void CServerManager::ConnectServer()	// ���� ���� �Լ�
{
	m_tcpSocket = make_shared<Socket>(SocketType::Tcp);

	/*std::cout << std::endl << " [ =========== Login =========== ] " << std::endl << std::endl;

	std::cout << std::endl << "Input Connect Server IP (ex 100.xxx.xxx.xxx) : " << std::endl;
	std::string server_s;
	std::cin >> server_s;
	m_SERVERIP = new char[server_s.size() + 1];
	m_SERVERIP[server_s.size()] = '\0';
	strncpy(m_SERVERIP, server_s.c_str(), server_s.size());*/

	m_tcpSocket->Bind(Endpoint::Any);
	CServerManager::Connetion();		// ����
	CServerManager::Do_Recv();			// ������ ���� ����
}

void CServerManager::Tick()				//�ֱ����� �۾� ���� �Լ�
{
	SleepEx(0, true);					

	// ��Ŷ ť Ȯ�� �� ������ ����
	if (PacketQueue::m_SendQueue.empty() || m_tcpSocket->m_fd == INVALID_SOCKET)
		return;

	while (!PacketQueue::m_SendQueue.empty()) {
		//  ��Ŷ ť���� ��Ŷ�� ������ �۽�
		char* send_buf = PacketQueue::m_SendQueue.front();
		int buf_size{};
		while (1) {
			// ������ ��Ŷ�� ũ�� ���
			if (buf_size + send_buf[buf_size] > MAX_BUFSIZE_CLIENT || send_buf[buf_size] == 0)
				break;
			buf_size += send_buf[buf_size];
		}
		Do_Send(send_buf, buf_size);		// ������ �۽�
		PacketQueue::PopSendPacket();		// �۽��� ��Ŷ ����
	}
}

void CServerManager::Connetion()			// Connect �Լ�
{
	m_tcpSocket->Connect(Endpoint(m_SERVERIP, SERVER_PORT));
}

void CServerManager::Do_Recv()				// ������ ���� �Լ�
{
	m_tcpSocket->m_readFlags = 0;			// ���Ͽ� ���� �б� �÷��� �ʱ�ȭ
	
	// ������ ����ü �ʱ�ȭ
	ZeroMemory(&m_tcpSocket->m_recvOverlapped.m_wsa_over, sizeof(m_tcpSocket->m_recvOverlapped.m_wsa_over));
	
	// ���� ���� ����
	m_tcpSocket->m_recvOverlapped.m_wsa_buf.len = MAX_SOCKBUF - m_tcpSocket->m_prev_remain;
	m_tcpSocket->m_recvOverlapped.m_wsa_buf.buf = m_tcpSocket->m_recvOverlapped.m_buf + m_tcpSocket->m_prev_remain;

	// �񵿱� ���� ����
	WSARecv(m_tcpSocket->m_fd, &(m_tcpSocket->m_recvOverlapped.m_wsa_buf), 1, 0,
		&m_tcpSocket->m_readFlags, &(m_tcpSocket->m_recvOverlapped.m_wsa_over), recv_callback);
}

void CServerManager::Do_Send(const char* _buf, short _buf_size)		// ������ �۽� �Լ�
{
	EXP_OVER* send_over = new EXP_OVER(_buf, _buf_size);			// // �۽ſ� ������ ����ü ����
	
	// �񵿱� �۽� ����
	WSASend(m_tcpSocket->m_fd, &send_over->m_wsa_buf, 1, 0, 0,
		&send_over->m_wsa_over, send_callback);
}

void CServerManager::PacketProcess(char* _Packet)	// ��Ŷ ó�� �Լ�
{
	// Packet Types Processing
	switch (_Packet[1])
	{
	case  PACKET_TYPE::P_SC_INIT_PACKET: {
		
		break;
	}
	default:
		break;
	}
}