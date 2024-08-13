#pragma once
#ifndef __HELPER_HACKER__
#define __HELPER_HACKER__
#include <Windows.h>

void Hook(int addr, unsigned int eSize, LPVOID Func);
unsigned int HookAtWithoutBehaviour(int addr, unsigned int eSize);
#endif