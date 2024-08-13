#include "LawnStruct.h"
#include "Common.h"

void Lawn::_Zombie::UpdateAnimSpeed()
{
	lpFunc func = 0x52F050;
	__asm {
		mov esi, this
		call func
	}
}

bool Lawn::_Zombie::IsOnBoard()
{
	lpFunc func = 0x52AE40;
	bool result = false;
	__asm {
		mov eax,this
		call func
		mov result,al
	}
	return result;
}
