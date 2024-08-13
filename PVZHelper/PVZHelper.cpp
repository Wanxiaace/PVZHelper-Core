#include <iostream>
#include <string>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <Windows.h>
#include "HelperMain.h"
#include "SocketInterface.h"

const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

// 填充字符
const char padding = '=';
using namespace std;

std::string base64Encode(std::string& src)
{
    std::string encoded;
    int val = 0, valb = -6;
    unsigned int i = 0;

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
        if (inet_ntop(ptr->ai_family, addr, ipStringBuffer, sizeof(ipStringBuffer)) != nullptr) {
            ipAddress.push_back(ipStringBuffer);
        }
    }

    freeaddrinfo(result);
    WSACleanup();

    return ipAddress;
}

#pragma comment(lib,"ws2_32.lib")
unsigned short port = 1000;
using namespace std;

int DoServer() {

    cout << "请输入创建的端口号: \n(注意，端口号的取值范围1到65535，请不要使用特殊端口号（可以百度查）)" << endl;
    cin >> port;

    auto v = GetLocalIP();
    std:string p;
    for (auto & x : v)
    {
        p = x.substr(0, 3);
        if (p == "192") {
            p = x;
            break;
        }
    }
    cout << "本机IP: " << p << endl;
    auto key = p + "$" + to_string(port);

    /**(int*)(lpPvzHpMap) = 514;
    *(int*)(int(lpPvzHpMap) + 4) = port;*/

    *(int*)(lpPvzHpMap) = 514;
    *(int*)(int(lpPvzHpMap) + 4) = key.size() + 1;
    memcpy((void*)(int(lpPvzHpMap) + 8), key.c_str(), key.size() + 1);

    cout << "联机码: " << base64Encode(key) << endl;

    /*Server sr = Server();
    sr.Init(port);
    auto v = GetLocalIP();
    std:string p = "";
    for (auto x : v)
    {
        p = x.substr(0, 3);
        if (p == "192") {
            p = x;
            break;
        }
    }
    cout << "本机IP: " << p << endl;
    auto key = p + "$" + to_string(port);
    cout << "联机码: " << base64Encode(key) << endl;

    ServerListen(&sr);
    ServerAccept(&sr);
    
    while (true);*/
    cout << "可以在pvz弹出加载成功的弹窗后关闭此程序..."<< endl;
    while (true);
    return 0;
}

int DoClient() {

    std::string key;
    cout << "请输入连接的房间码" << endl;
    cin >> key;
    std::string keyStr = base64Decode(key);

    *(int*)(lpPvzHpMap) = 114;
    *(int*)(int(lpPvzHpMap) + 4) = keyStr.size() + 1;
    memcpy((void*)(int(lpPvzHpMap) + 8),keyStr.c_str(),keyStr.size() + 1);

    /*auto s_info = GetServerInfoByKeyStr(keyStr);

    Client cl = Client();
    cl.Init(s_info);

    cl.ClientSendUserNames("连接到房间");
    while (true) {
        cl.ClientSendUserNames("Hello World");
        Sleep(1000);
    }*/

    cout << "可以在pvz弹出dll加载成功的弹窗后关闭此程序..." << endl;
    while (true);
    return 0;
}

int main(int argc, char** argv)
{
    int ans = PVZClientMain(argc, argv);
    if (ans != 0)
        return ans;

    cout << "选择操作： 连接房间(1)  创建房间(2)" << endl;
    int mode;
    cin >> mode;
    if (mode == 1) {
        return DoClient();
    }
    else if (mode == 2) {
        return DoServer();
    }

    UnmapViewOfFile(lpPvzHpMap);
    

    return 0;
}
