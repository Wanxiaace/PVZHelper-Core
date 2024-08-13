#include "HelperMain.h"
#include "pvzclass.h"
#include <iostream>
using namespace std;

char dllpath[] = "MainDll.dll";
void* lpPvzHpMap = nullptr;

bool InjectDllFixed(const char* dllname)
{
	SETARG(__asm__InjectDll, 1) = PVZ::Memory::Variable + 0x600;
	SETARG(__asm__InjectDll, 19) = PVZ::Memory::Variable;
	int len = strlen(dllname);
	PVZ::Memory::WriteArray<const char>(PVZ::Memory::Variable + 0x600, dllname, len);
	PVZ::Memory::WriteMemory<char>(PVZ::Memory::Variable + 0x600 + len, 0);
	//MessageBoxA(NULL, dllname,"",NULL);
	PVZ::Memory::DLLAddress = PVZ::Memory::Execute(STRING(__asm__InjectDll));
	return PVZ::Memory::DLLAddress != 0;
}

int PVZClientMain(int argc,char** argv)
{
	auto hMap = CreateFileMappingW(
		INVALID_HANDLE_VALUE, 
		NULL, 
		PAGE_READWRITE,
		0, 1024, L"__PVZ_HP_MAP__");
	if (!hMap)
		return -1;
	lpPvzHpMap = MapViewOfFile(hMap,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		0);

	DWORD pid = ProcessOpener::Open();

	if (!pid) {
		cout << "请打开pvz主程序" << endl;
		return -1;
	}

	PVZ::InitPVZ(pid);
	PVZ::Memory::immediateExecute = true;
	char buf[MAX_PATH] = {0};
	GetFullPathNameA(dllpath,MAX_PATH, buf,NULL);
	bool re = InjectDllFixed(buf);

	PVZ::Memory::immediateExecute = false;
	PVZ::QuitPVZ();
	return 0;
}
