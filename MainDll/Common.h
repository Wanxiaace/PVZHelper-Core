#pragma once
#include <iostream>
#ifndef __HP_COMMON__
#define __HP_COMMON__

typedef unsigned int _Reg;
typedef std::string SexyString;
typedef unsigned int lpSexyString;
typedef unsigned int lpGraphics;
typedef unsigned int lpFont;
typedef unsigned int lpImage;
typedef unsigned int lpFunc;
typedef unsigned int lpBoard;
typedef unsigned int lpCutscene;
typedef unsigned int lpCrater;
typedef unsigned int lpLawnDialog;
typedef unsigned int lpPlant;
typedef unsigned int lpZombie;
typedef unsigned int ScoreInteger;

const lpFont Font1 = 0x6a74b0;
const lpFont Font2 = 0x6A7630;

struct SexyRect {
	int mX;
	int mY;
	int mWidth;
	int mHeight;
};

struct SexyColor
{
	unsigned int r;
	unsigned int g;
	unsigned int b;
	unsigned int a;
};


const int LEVELS_PER_AREA = 10;
#define _hp_LEVEL(area, sub) (((area) - 1) * LEVELS_PER_AREA + (sub))
#define _hp_CheckGraphics if(!_gra) return;
#define _hp_Check(a) if(!a) return;
#define _hp_Check_WithResult(a,b) if(!a) return b;
#define _hp_Assert(a) if(!a) __debugbreak();

#define _hp_Show_Tip(text) MessageBox(NULL,TEXT(text),TEXT("Ã· æ"),MB_SYSTEMMODAL);
#define _hp_Show_Error(text) MessageBox(NULL,TEXT(text),TEXT("¥ÌŒÛ"),MB_SYSTEMMODAL);


#endif // !__HP_COMMON__

