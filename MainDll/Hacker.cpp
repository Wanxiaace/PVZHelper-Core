#include "Hacker.h"
#include "pvzclass.h"
#include "AsmFunctions.h"

using namespace PVZ;

void Hook(int addr,unsigned int eSize,LPVOID Func)
{
	if (eSize < 5)
		return;
	unsigned char* data = new unsigned char[eSize];
	Memory::ReadArray<unsigned char>(addr,data,eSize);

	char* npData = new char[eSize];
	memset(npData, NOP, eSize);
	Memory::WriteArray<char>(addr, npData,eSize);
	delete[] npData;

	Memory::WriteMemory<unsigned char>(addr, 0xE9);//长跳
	int newmem = Memory::AllocMemory(1, 0x40);
	memset((void*)newmem, NOP,0x40);

	//判断是否Hook了JMP LONG
	if (data[0] == 0xE9 || data[0] == 0xE8) {//00413059
		*((unsigned int*)&data[1]) -= (0x15 + newmem - addr);
	}

	Memory::WriteMemory<unsigned int>(addr+1, newmem - addr - 5);
	Memory::WriteArray(newmem + 0x15, data,eSize);
	int jpl = addr + eSize - (newmem + 0x15 + eSize + 5);
	unsigned char JumpBack[] = { JMPFAR(jpl), };
	Memory::WriteArray<unsigned char>(newmem + 0x15 + eSize, JumpBack, 5);


	unsigned char ASM_HOOK_INTERFACE[] = {
	PUSHAD,
	PUSHAD,//传参
	MOV_EAX((int)Func),
	CALL_EUX(REG_EAX),
	ADD_ESP(0x20),
	POPAD,
	};

	Memory::WriteArray<unsigned char>(newmem, ASM_HOOK_INTERFACE, sizeof(ASM_HOOK_INTERFACE));
}

unsigned int HookAtWithoutBehaviour(int addr, unsigned int eSize)
{
	if (eSize < 5)
		return 0;
	unsigned char* data = new unsigned char[eSize];
	Memory::ReadArray<unsigned char>(addr, data, eSize);

	char* npData = new char[eSize];
	memset(npData, NOP, eSize);
	Memory::WriteArray<char>(addr, npData, eSize);
	delete[] npData;

	Memory::WriteMemory<unsigned char>(addr, 0xE9);//长跳
	int newmem = Memory::AllocMemory(1, 0x40);
	memset((void*)newmem, NOP, 0x40);

	//判断是否Hook了JMP LONG
	if (data[0] == 0xE9 || data[0] == 0xE8) {//00413059
		*((unsigned int*)&data[1]) -= (0x15 + newmem - addr);
	}else if (data[0] == 0x0F && data[1] == 0x87) {
		*((unsigned int*)&data[2]) -= (0x15 + newmem - addr);
	}

	Memory::WriteMemory<unsigned int>(addr + 1, newmem - addr - 5);
	Memory::WriteArray(newmem + 0x15, data, eSize);
	int jpl = addr + eSize - (newmem + 0x15 + eSize + 5);
	unsigned char JumpBack[] = { JMPFAR(jpl), };
	Memory::WriteArray<unsigned char>(newmem + 0x15 + eSize, JumpBack, 5);

	return newmem;
}
