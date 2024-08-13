#pragma once
#ifndef __HP_SOCEKET_INTERFACE__
#define __HP_SOCEKET_INTERFACE__

#include "../MainDll/Common.h"
#include "../MainDll/SettingDialog.h"
#include <WinSock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#include <iostream>
#include <vector>
#include <map>


typedef int _HP_Token;
typedef int _HP_S_Code;
const _HP_S_Code CODE_SERVER = 514;
const _HP_S_Code CODE_CLIENT = 114;

class Socket;
class Server;
class Client;

void SocketInit();
void SocketQuit();

enum GameMessageID {
	MSG_NONE,

	//��Ҫ����һ���ַ���
	MSG_USERNAME_WITH,
	//��Ҫ����һ���ֽ���
	MSG_BINARY_WITH,
	//����������Ϸ
	MSG_HOST_ENTERGAME,
	//����ͻ��˷���
	MSG_POST_CLIENT_SCORE,
	//���߷������Ϸ����
	MSG_POST_CLIENT_GAME_FINISH,
	//���߿ͻ����Ѿ���ʼ��Ϸ���޷����뷿��
	MSG_FAILED_TO_JOIN,
	//�������
	MSG_CHECK_OUT_FINISH,
	//������Ϸ���������Ϣ
	MSG_HOST_SETTINGS_INFO,
	//����Ĺ������
	MSG_SPAWN_TOMB,
	//����һֻ��ʬ
	MSG_SEND_ZOMBIE,
	//����һ������
	MSG_SEND_SEEDPACK,
};

struct BinaryMessage {
	int m_Length;
	char* m_Data;
};

struct GameMessage {
	int m_Simple = 0x1919810;
	GameMessageID m_MsgID = MSG_NONE;
	int m_param1 = 0;
	int m_param2 = 0;
	unsigned long long m_param3 = 0;
	unsigned long long m_param4 = 0;

	//��ǿͻ�����ݵ�����
	int m_Token = -1;
};

struct ClientState {
	ScoreInteger mScore = 0;
	std::string mUserName = "";
	bool mFinishGame = false;
};

struct ServerInfo {
	std::string mIP;
	unsigned short mPort;
};

struct ClientMessageObject {
	std::vector<GameMessage>* mMsgArray = nullptr;
	std::vector<char*>* mDataArray = nullptr;
};


ServerInfo GetServerInfoByKeyStr(std::string& src);

class Socket {
public:
	//�˿�
	unsigned short mPort = 0;
	bool mIsLoaded = false;

	SOCKET mSocket = INVALID_SOCKET;
	sockaddr_in mSockAddr;
	std::vector<std::string> mStrs;

public:
	Socket();
	~Socket();
	
	void Init(unsigned short port);
};

class Server : public Socket {
public:
	int mBind = -1;
	std::map<_HP_Token, SOCKET> mClientMap;
	std::map<_HP_Token, ClientState> mClientStates;

public:
	Server();
	~Server() { Socket::~Socket(); };

	void SendEnterGameMessage(int modeId,int levelId,int Token);
	void All_SendEnterGameMessage(int modeId,int levelId);
	void All_SendEnterCheckOut();
	void All_SendSettingsStruct();
	void All_SendAZombie(void* srcZom);
	void All_SendASeedPack(int seedtype);
	void Init(unsigned short port);
};

class Client :public Socket {
public:
	int mToken = -1;
	bool mIsFinishCheckout = false;

public:
	Client();
	~Client() { Socket::~Socket(); };

	void Init(ServerInfo sInfo);
	void Update();

	void ClientSendMessage(GameMessage msg);
	void ClientSendUserNames(std::string& src);
	void ClientSendScore(int score);
	void ClientSendBinaries(const char* src,size_t len);
	void ClientSendFinishGame();

	void CheckConnect(unsigned short SendBack);

	//���ܷ������Ϣ������������Ϣ
	int UpdateMessage();
};

void ServerListen(Server* ser);
void ServerAccept(Server* ser);


int GameSendMessage(GameMessage msg, SOCKET sk);
int GameSendBinaries(const char* data,int len, int Token, SOCKET sk);
int GameSendString(const char* str,int len, int Token, SOCKET sk);
GameMessage GameGetMessage();

std::vector<std::string> GetLocalIP();
std::string base64Encode(std::string& src);
std::string base64Decode(std::string& src);

void PlayerSendTombSpawnMessage(int x,int y);

#endif // !__HP_SOCEKET_INTERFACE__


