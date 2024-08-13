#include "SocketInterface.h"
#include "../MainDll/App.h"
#include <thread>
#include <string>
#include "../MainDll/DllMain.h"
#include "../MainDll/Board.h"

std::vector<std::thread*> ThreadPool;
int TokenNum = 100;

Socket::Socket()
{
    mSockAddr = sockaddr_in();
}

Socket::~Socket()
{
    if (mSocket != INVALID_SOCKET) {
        closesocket(mSocket);
        mSocket = INVALID_SOCKET;
    }
}

void Socket::Init(unsigned short port)
{
    mIsLoaded = false;
    mPort = port;

    //TCP
    mSocket = socket(AF_INET, SOCK_STREAM, 0);
    //if(mSocket == INVALID_SOCKET){}
    int nNetTimeout = 3000;//1秒，
    //设置发送超时
    setsockopt(mSocket,SOL_SOCKET,SO_SNDTIMEO,(char*) & nNetTimeout, sizeof(int));

    mSockAddr.sin_family = AF_INET;
    mSockAddr.sin_port = htons(mPort);
    
}

void SocketInit()
{
    WSADATA wsaData;
    int iResult;
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup 错误: %d\n", iResult);
        return;
    }
}

void SocketQuit()
{

}

ServerInfo GetServerInfoByKeyStr(std::string& src)
{
    int pos = src.find("$");
    ServerInfo re;
    auto port = src.substr(pos + 1);
    auto ip = src.substr(0, pos);
    re.mPort = (unsigned short)atoi(port.c_str());
    re.mIP = ip;
    return re;
}

static void ServerThread(unsigned int m_sk,Server* ser) 
{
    std::string tk_Msg = "TK:";
    if (g_is_playing || ((ser->mClientMap.size() + 1) >= g_settingdlg.mStruct.mConnetPlayersMax) )
        tk_Msg += "-1";
    else
        tk_Msg += std::to_string(TokenNum);
    send(m_sk, tk_Msg.c_str(), tk_Msg.size(), NULL);

    if (g_is_playing) {
        closesocket(m_sk);
        return;
    }

    ser->mClientMap[TokenNum] = m_sk;
    ser->mClientStates[TokenNum].mScore = 0;

    g_ser.All_SendSettingsStruct();

    TokenNum++;

    while (true)
    {
        int msgSize = sizeof(GameMessage);
        char* databuf = new char[msgSize];
        memset(databuf, 0, msgSize);
        int re = recv(m_sk, databuf, msgSize, NULL);
        if (!re || re == SOCKET_ERROR) {
            delete[] databuf;
            closesocket(m_sk);
            break;
        }

        GameMessage* gmsg = (GameMessage*)databuf;
        if (gmsg->m_Simple != 0x1919810)
            continue;
        if (gmsg) {
            std::cout << "接受到客户端消息" << std::endl;
            switch (gmsg->m_MsgID) {
            case GameMessageID::MSG_USERNAME_WITH: {
                std::cout << "接受到用户名:" << std::endl;
                int strSize = gmsg->m_param1;
                char* strbuf = new char[strSize];
                memset(strbuf, 0, strSize);
                recv(m_sk, strbuf, strSize, NULL);
                ser->mClientStates[gmsg->m_Token].mUserName = strbuf;
                std::cout << strbuf << std::endl;
                delete[] strbuf;
                break;
            }
            case GameMessageID::MSG_POST_CLIENT_SCORE: {
                ser->mClientStates[gmsg->m_Token].mScore = gmsg->m_param1;
                break;
            }
            case GameMessageID::MSG_POST_CLIENT_GAME_FINISH: {
                ser->mClientStates[gmsg->m_Token].mFinishGame = true;
                break;
            }
            case GameMessageID::MSG_SPAWN_TOMB: {
                auto p = PVZ::GetBoard();
                if (!p)
                    break;
                Board::SpawnTomb(gmsg->m_param1, gmsg->m_param2);

                for (auto& x:g_ser.mClientMap)
                {
                    if (x.first == gmsg->m_Token)
                        continue;
                    GameMessage msg;

                    msg.m_MsgID = GameMessageID::MSG_SPAWN_TOMB;
                    msg.m_param1 = gmsg->m_param1;
                    msg.m_param2 = gmsg->m_param2;
                    GameSendMessage(msg, x.second);
                }
                break;
            }
            }
        }

        delete[] databuf;
        
        //龟速响应
        Sleep(10);
    }
    //添加Wait，减少崩溃
    App::WaitPvZ();
    int dtk = 0;
    for (auto x = ser->mClientMap.begin();x != ser->mClientMap.end();x ++)
    {
        if ((*x).second != m_sk)
            continue;
        dtk = x->first;
        ser->mClientMap.erase(x);
        break;
    }
    for (auto x = ser->mClientStates.begin(); x != ser->mClientStates.end(); x++)
    {
        if ((*x).first != dtk)
            continue;
        ser->mClientStates.erase(x);
        break;
    }
    App::EndWait();
}

void ServerListen(Server* ser)
{
    listen(ser->mSocket,25);
}

void ServerAccept(Server* ser)
{
    SOCKET g_sk = accept(ser->mSocket, nullptr, nullptr);
    auto th = new std::thread(ServerThread, g_sk, ser);
    ThreadPool.push_back(th);
}


int GameSendMessage(GameMessage msg,SOCKET sk)
{
    msg.m_Simple = 0x1919810;
    return send(sk,(const char*) & msg, sizeof(GameMessage), 0);
}

int GameSendBinaries(const char* data, int len, int Token,SOCKET sk)
{
    GameMessage msg = { 0 };
    msg.m_MsgID = GameMessageID::MSG_BINARY_WITH;
    msg.m_Token = Token;
    msg.m_param1 = len;
    GameSendMessage(msg,sk);

    return send(sk,data,len, 0);
}

int GameSendString(const char* str, int len,int Token, SOCKET sk)
{
    GameMessage msg = { 0 };
    msg.m_MsgID = GameMessageID::MSG_USERNAME_WITH;
    msg.m_param1 = len;
    msg.m_Token = Token;
    GameSendMessage(msg, sk);

    return send(sk, str, len, 0);
}

GameMessage GameGetMessage()
{
    return GameMessage();
}

Server::Server() :Socket()
{

}

void Server::SendEnterGameMessage(int modeId, int levelId, int Token)
{
    GameMessage msg = {0};
    msg.m_MsgID = MSG_HOST_ENTERGAME;
    msg.m_param1 = modeId;
    msg.m_param2 = levelId;
    GameSendMessage(msg,mClientMap[Token]);
}

void Server::All_SendEnterGameMessage(int modeId, int levelId)
{
    for (auto & x : mClientMap)
    {
        SendEnterGameMessage(modeId,levelId,x.first);
    }
}

void Server::All_SendEnterCheckOut()
{
    for (auto& x : mClientMap)
    {
        GameMessage msg = { 0 };
        msg.m_MsgID = MSG_CHECK_OUT_FINISH;
        GameSendMessage(msg, x.second);
    }
}

void Server::All_SendSettingsStruct()
{
    GameMessage msg = { 0 };
    msg.m_Simple = 0x1919810;
    msg.m_MsgID = MSG_HOST_SETTINGS_INFO;
    for (auto& x : mClientMap)
    {
        send(x.second, (const char*)&msg, sizeof(GameMessage), 0);
        send(x.second, (const char*)&g_settingdlg.mStruct, sizeof(SettingStruct), 0);
    }//00413059
}
#include "../MainDll/GameZombie.h"
#include "../MainDll/LawnStruct.h"
void Server::All_SendAZombie(void* srcZom)
{
    GameMessage msg = { 0 };
    msg.m_Simple = 0x1919810;
    msg.m_MsgID = MSG_SEND_ZOMBIE;
    msg.m_param1 = g_zombie_struct_size;
    for (auto& x : mClientMap)
    {
        send(x.second, (const char*)&msg, sizeof(GameMessage), 0);
        send(x.second, (const char*)srcZom, g_zombie_struct_size, 0);
    }
}

void Server::All_SendASeedPack(int seedtype)
{
    GameMessage msg = {0};
    msg.m_MsgID = MSG_SEND_SEEDPACK;
    msg.m_param1 = seedtype;
    for (auto& x : mClientMap)
    {
        GameSendMessage(msg,x.second);
    }
}

void Server::Init(unsigned short port)
{
    Socket::Init(port);
    mSockAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    //htonl(INADDR_ANY);//inet_addr("127.0.0.1");
    mBind = bind(mSocket, (const struct sockaddr*)&mSockAddr, sizeof(sockaddr_in));
    mIsLoaded = true;
}

Client::Client() :Socket()
{

}

void Client::Init(ServerInfo sInfo)
{
    Socket::Init(sInfo.mPort);
    inet_pton(AF_INET, sInfo.mIP.c_str(), &mSockAddr.sin_addr.S_un.S_addr);
    connect(mSocket, (const sockaddr*)&mSockAddr, sizeof(sockaddr_in));
    while (true) {
        char tkmsg[10] = { 0 };
        recv(mSocket, tkmsg, 10, NULL);
        std::string tkstr = tkmsg;
        if (tkstr.substr(0,2) == "TK") {
            mToken = std::atoi(tkstr.substr(3).c_str());
            if (mToken <= 0) {
                _hp_Show_Error("连接失败，可能的原因：房间人数达到上线，房主拒绝了连接或者游戏已开始");
            }
            break;
        }
    }
    mIsLoaded = true;
}

void Client::Update()
{

}

void Client::ClientSendMessage(GameMessage msg)
{
    if (mToken == -1 || !mIsLoaded)
        return;
    //自动标记身份
    msg.m_Token = mToken;
    int p = GameSendMessage(msg,mSocket);
    CheckConnect(p);
}

void Client::ClientSendUserNames(std::string& src)
{
    GameMessage msg = { 0 };
    msg.m_MsgID = GameMessageID::MSG_USERNAME_WITH;
    msg.m_param1 = src.size() + 1;

    int i = GameSendString(src.c_str(),src.size() + 1,mToken,mSocket);
    CheckConnect(i);
}

void Client::ClientSendScore(int score)
{
    GameMessage msg = { 0 };
    msg.m_MsgID = GameMessageID::MSG_POST_CLIENT_SCORE;
    msg.m_param1 = score;
    ClientSendMessage(msg);
}

void Client::ClientSendBinaries(const char* src, size_t len)
{
    int i = GameSendBinaries(src,len,mToken,mSocket);
    CheckConnect(i);
}

void Client::ClientSendFinishGame()
{
    GameMessage msg = {0};
    msg.m_MsgID = MSG_POST_CLIENT_GAME_FINISH;
    ClientSendMessage(msg);
}

void Client::CheckConnect(unsigned short SendBack)
{
    if (SendBack <= 0) {
        mIsLoaded = false;

        App::Safe_DoDialog(0,"Warning","Failed to Connect or Timeout,\nYou can shut down your game and try to reconnect");
    }
}

int Client::UpdateMessage()
{
    int error = -1;
    int msgSize = sizeof(GameMessage);
    char* databuf = new char[msgSize];
    memset(databuf, 0, msgSize);
    error = recv(mSocket, databuf, msgSize, NULL);
    if (error <= 0)
        return error;
    GameMessage* gmsg = (GameMessage*)databuf;
    if (gmsg->m_Simple != 0x1919810)
        return 1;
    if (gmsg) {
        std::cout << "接受到服务端消息" << std::endl;
        switch (gmsg->m_MsgID) {
        case GameMessageID::MSG_USERNAME_WITH:
        {
            std::cout << "接受到字符串:" << std::endl;
            int strSize = gmsg->m_param1;
            char* strbuf = new char[strSize];
            memset(strbuf, 0, strSize);
            error = recv(mSocket, strbuf, strSize, NULL);
            if (error <= 0)
                return error;
            mStrs.push_back(std::string(strbuf));
            delete[] strbuf;
            break;
        }
        case GameMessageID::MSG_HOST_ENTERGAME:
        {
            if (g_LawnDialog) {
                App::Safe_KillDialog(g_LawnDialog);
                g_LawnDialog = 0;
            }
            App::Safe_PreNewGame(gmsg->m_param1, gmsg->m_param2);
            break;
        }
        case GameMessageID::MSG_CHECK_OUT_FINISH: 
        {
            mIsFinishCheckout = true;
            break;
        }
        case GameMessageID::MSG_HOST_SETTINGS_INFO: 
        {
            int strSize = sizeof(SettingStruct);
            SettingStruct* strbuf = new SettingStruct();
            memset(strbuf, 0, strSize);
            error = recv(mSocket, (char*)strbuf, strSize, NULL);
            if (error <= 0)
                return error;

            g_settingdlg.mStruct = *strbuf;
            delete strbuf;
            break;
        }
        case GameMessageID::MSG_SPAWN_TOMB:
        {
            auto p = PVZ::GetBoard();
            if (!p)
                break;
            Board::SpawnTomb(gmsg->m_param1, gmsg->m_param2);
            break;
        }
        case GameMessageID::MSG_SEND_ZOMBIE: 
        {
            auto p = PVZ::GetBoard();
            if (!p)
                break;
            int strSize = gmsg->m_param1;
            char* strbuf = (char*)_alloca(strSize);
            _ASSERT(strbuf);
            memset(strbuf, 0, strSize);
            error = recv(mSocket, (char*)strbuf, strSize, NULL);
            Lawn::_Zombie* zom = (Lawn::_Zombie*)strbuf;
            Lawn::_Zombie* zom_spawn = Board::AddZombieInRow(zom->mRow,zom->mZombieType);
            
            zom_spawn->mPosX = zom->mPosX;
            zom_spawn->mPosY = zom->mPosY;
            zom_spawn->mBodyHealth = zom->mBodyHealth;
            zom_spawn->mBodyMaxHealth = zom->mBodyMaxHealth;

            zom_spawn->mHelmHealth = zom->mHelmHealth;
            zom_spawn->mHelmType = zom->mHelmType;
            zom_spawn->mHelmMaxHealth = zom->mHelmMaxHealth;

            zom_spawn->mShieldHealth = zom->mShieldHealth;
            zom_spawn->mShieldMaxHealth = zom->mShieldMaxHealth;
            zom_spawn->mShieldType = zom->mShieldType;

            zom_spawn->mZombiePhase = zom->mZombiePhase;
            zom_spawn->mScaleZombie = zom->mScaleZombie;
            
            zom_spawn->mVelX = zom->mVelX;
            zom_spawn->mAnimTicksPerFrame = zom->mAnimTicksPerFrame;

            zom_spawn->UpdateAnimSpeed();

            if (error <= 0)
                return error;
            break;
        }
        case GameMessageID::MSG_SEND_SEEDPACK: {
            if (g_is_playing)
                Board::SpawnCardAt(rand()%500 + 100,rand() % 300 + 100, gmsg->m_param1);
            break;
        }
        }

    }

    delete[] databuf;
    return error;
}


#include <WinSock2.h>
#include <ws2tcpip.h>
using namespace std;

#pragma comment(lib, "ws2_32.lib")

vector<string> GetLocalIP()
{
    vector<string> ipAddress;
    WSADATA WSAData;
    if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0) {
        fprintf(stderr, "Failed to initialize Winsock.\n");
        return ipAddress;
    }

    char hostName[256];
    if (gethostname(hostName, sizeof(hostName)) != 0) {
        WSACleanup();
        fprintf(stderr, "Failed to get hostname.\n");
        return ipAddress;
    }

    addrinfo hints = { 0 };
    hints.ai_family = AF_UNSPEC;     // Allow IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP socket
    hints.ai_protocol = IPPROTO_TCP; // TCP protocol

    addrinfo* result = nullptr;
    if (getaddrinfo(hostName, NULL, &hints, &result) != 0) {
        WSACleanup();
        fprintf(stderr, "Failed to resolve hostname.\n");
        return ipAddress;
    }


    for (addrinfo* ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
        void* addr = nullptr;
        if (ptr->ai_family == AF_INET) {
            // IPv4 address
            addr = &((sockaddr_in*)ptr->ai_addr)->sin_addr;
        }
        else if (ptr->ai_family == AF_INET6) {
            // IPv6 address
            addr = &((sockaddr_in6*)ptr->ai_addr)->sin6_addr;
        }

        char ipStringBuffer[INET6_ADDRSTRLEN];
        if(addr)
            if (inet_ntop(ptr->ai_family, addr, ipStringBuffer, sizeof(ipStringBuffer)) != nullptr) {
                ipAddress.push_back(ipStringBuffer);
            }
    }

    freeaddrinfo(result);
    WSACleanup();

    return ipAddress;
}

const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

// 填充字符
const char padding = '=';

std::string base64Encode(std::string& src)
{
    std::string encoded;
    int val = 0, valb = -6;
    //unsigned int i = 0;

    for (unsigned char c : src) {
        val = (val << 8) + c;
        valb += 8;

        while (valb >= 0) {
            encoded.push_back(base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }

    if (valb > -6) {
        encoded.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    }

    while (encoded.size() % 4 != 0) {
        encoded.push_back(padding);
    }

    return encoded;
}


std::string base64Decode(std::string& src)
{
    std::string decoded;
    int val = 0, valb = -8;
    for (char c : src) {
        if (c == padding) {
            break;
        }
        if (c && c != padding) {
            val = (val << 6) + base64_chars.find(c);
            valb += 6;
            if (valb >= 0) {
                decoded.push_back((val >> valb) & 0xFF);
                valb -= 8;
            }
        }
    }
    return decoded;
}

void PlayerSendTombSpawnMessage(int x, int y)
{
    GameMessage msg;
    msg.m_MsgID = GameMessageID::MSG_SPAWN_TOMB;
    msg.m_param1 = x;
    msg.m_param2 = y;

    if (g_is_client) {
        msg.m_Token = g_cl.mToken;
        GameSendMessage(msg,g_cl.mSocket);
    }
    else {
        for (auto& i : g_ser.mClientMap) {
            GameSendMessage(msg, i.second);
        }
    }
}
