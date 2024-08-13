#include <iostream>
#include <Windows.h>
using namespace std;

const char* m_version = "1.1";
const char* m_dll_path = "MainDll.dll";

static bool InjectDllFixed(HANDLE process_handle,const char* dllname)
{
	LPVOID dll_path_address = VirtualAllocEx(process_handle, NULL, strlen(dllname) + 1, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	_ASSERT(dll_path_address);
	WriteProcessMemory(process_handle,dll_path_address,dllname,strlen(dllname) + 1,NULL);
	HMODULE kernel_handle = GetModuleHandleA("Kernel32.dll");
	_ASSERT(kernel_handle);
	PTHREAD_START_ROUTINE load_library_func_address = (PTHREAD_START_ROUTINE)GetProcAddress(kernel_handle, "LoadLibraryA");
	
	HANDLE thread_handle = CreateRemoteThread(process_handle, NULL, 0, load_library_func_address, dll_path_address, 0, NULL);

	return thread_handle ? true : false;
}

int main(int argc,char** argv) {
	if (argc == 1) {
		cout << "Helper cmd\n输入\"-h\" 获得帮助" << endl;
		return -1;
	}

	bool is_client = true;
	std::string room_ip = "";
	std::string room_port = "";
	std::string process_id = "";

	for (size_t i = 1; i < argc; i++)
	{
		std::string command_string = std::string(argv[i]);
		if (command_string == "-h") {
			cout << 
				"-h 显示帮助信息\n"
				"-i [argv] 键入要连接&创建房间的IP地址\n"
				"-p [argv] 键入要连接&创建的房间端口\n"
				"-s 作为房主\n"
				"-c 作为游客\n"
				"-v 检查版本\n"
				"-g [argv] 键入植物大战僵尸的进程id\n"
				<< endl;
		}
		if (command_string == "-v")
			cout << "当前版本: " << m_version << endl;
		else if (command_string == "-c")
			is_client = true;
		else if (command_string == "-s")
			is_client = false;
		else if (command_string == "-i")
			if (i + 1 != argc && argv[i + 1][0] != '-')
				room_ip = argv[i + 1];
			else
			{
				cout << "参数错误" << endl;
				return -1;
			}
		else if (command_string == "-p")
			if (i + 1 != argc && argv[i + 1][0] != '-')
				room_port = argv[i + 1];
			else
			{
				cout << "参数错误" << endl;
				return -1;
			}
		else if (command_string == "-g")
			if (i + 1 != argc && argv[i + 1][0] != '-')
				process_id = argv[i + 1];
			else
			{
				cout << "参数错误" << endl;
				return -1;
			}
	}

	std::string room_key = room_ip + "$" + room_port;
	unsigned int pvz_process_id_integer = atoi(process_id.c_str());
	if (room_ip != "" && room_port != "")
	{
		if (process_id != "") {
			std::string current_identity = is_client ? "Guest" : "Host";
			cout << "你是: " << current_identity << endl;
			cout << "进程ID: " << pvz_process_id_integer << endl;
			if (is_client) {
				cout << "尝试连接至: " << room_key;
			}
			else {
				cout << "尝试以: " << room_key << "创建房间" << endl;
			}
			HANDLE pvz_process_handle = OpenProcess(PROCESS_ALL_ACCESS,FALSE, pvz_process_id_integer);
			if (!pvz_process_handle) {
				cout << "尝试连接游戏进程失败，可能是进程ID无效" << endl;
				return -1;
			}
			else {
				char dll_full_path[MAX_PATH] = { 0 };
				GetFullPathNameA(m_dll_path,MAX_PATH, dll_full_path,NULL);
				InjectDllFixed(pvz_process_handle,dll_full_path);

				HANDLE handle_create_shared_map = CreateFileMappingW(
					INVALID_HANDLE_VALUE,
					NULL,
					PAGE_READWRITE,
					0, 1024, L"__PVZ_HP_MAP__");
				_ASSERT(handle_create_shared_map);
				HANDLE handle_shared_map = MapViewOfFile(handle_create_shared_map,
					FILE_MAP_ALL_ACCESS,
					0,
					0,0);
				_ASSERT(handle_shared_map);

				if (is_client) {
					*(int*)(handle_shared_map) = 114;
					*(int*)(int(handle_shared_map) + 4) = room_key.size() + 1;
					memcpy((void*)(int(handle_shared_map) + 8), room_key.c_str(), room_key.size() + 1);
				}
				else {
					*(int*)(handle_shared_map) = 514;
					*(int*)(int(handle_shared_map) + 4) = room_key.size() + 1;
					memcpy((void*)(int(handle_shared_map) + 8), room_key.c_str(), room_key.size() + 1);
				}
				Sleep(500);
				UnmapViewOfFile(handle_shared_map);
			}

		}
		else {
			cout << "请添加-g参数并正确的键入pvz的进程ID" << endl;
		}
		
	}
	
	
		
	return 0;
}