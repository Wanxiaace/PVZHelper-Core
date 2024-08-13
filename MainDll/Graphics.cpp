#include "Graphics.h"
#include "pvzclass.h"
#include "App.h"

void Gra::SetColor(lpGraphics _gra, const SexyColor TargetColor)
{
	const SexyColor* tgc = &TargetColor;
	__asm {
		mov eax, tgc
		mov ecx, _gra
		mov edx, 0x586CC0
		call edx//Sexy::Graphics::SetColor
	}
}

void Gra::SetColorizeImages(lpGraphics _gra, bool IsEnable)
{
	lpFunc Func = 0x586D10;
	__asm {
		mov cl,IsEnable
		mov eax, _gra
		call Func
	}
}

void Gra::FillRect(lpGraphics _gra, int X, int Y, int Width, int Height)
{
	_hp_CheckGraphics
	__asm {
		mov eax, _gra
		push Height//Height
		push Width//Width
		push Y//Y
		push X//x
		mov ecx, 0x586D50//Sexy::Graphics::FillRect()
		call ecx
	}
}

void Gra::SetFont(lpGraphics _gra, const lpFont* Font)
{
	__asm {
		mov eax, _gra
		mov ecx,Font
		mov edx, 0x586CB0//Sexy::Graphics::SetFont()
		call edx
	}
}

void Gra::DrawStringOri(lpGraphics _gra, int x, int y, SexyString str,SexyColor color,const lpFont font)
{
	_hp_CheckGraphics
	unsigned int pstr = Draw::ToString(str.c_str());
	lpFunc Func = 0x511CE0;
	__asm {
		push color.a
		push color.b
		push color.g
		push color.r
		push 0
		mov ebx,font
		mov ebx,[ebx]
		push y
		mov ecx,x
		mov edx,pstr
		push _gra
		call Func
		add esp,0x1c
	}
	PVZ::Memory::FreeMemory(pstr);
}

volatile bool m_is_outlined = false;
volatile bool is_shadowd = false;


void Gra::SetOutlined(bool aIsOutlined)
{
	m_is_outlined = aIsOutlined;
}

void Gra::DrawStringPlus(lpGraphics _gra, int x, int y, SexyString target_str,SexyColor color, Font* target_font, int font_size, float font_scale)
{
	
	_hp_CheckGraphics;
	SetColorizeImages(_gra,true);
	unsigned int w, h;
	unsigned long* data = GenerateStringImage(target_font, font_size, target_str.c_str(), &w, &h);
	lpImage img = Gra::LoadMemoryImage(data, w, h);

	if (m_is_outlined) {
		SetColor(_gra, { 0,0,0,color.a });
		Gra::DrawImageScaledF(_gra, x + 1.0f, y + 1.0f, font_scale, font_scale, img);
		Gra::DrawImageScaledF(_gra, x - 1.0f, y + 1.0f, font_scale, font_scale, img);
		Gra::DrawImageScaledF(_gra, x + 1.0f, y - 1.0f, font_scale, font_scale, img);
		Gra::DrawImageScaledF(_gra, x - 1.0f, y - 1.0f, font_scale, font_scale, img);
		//std::cout << color.r;
	}

	SetColor(_gra, color);
	Gra::DrawImageScaledF(_gra, x, y, font_scale, font_scale, img);

	delete[] data;
	Gra::FreeMemoryImage(img);
	SetColorizeImages(_gra, false);
}

void Gra::DrawImageScaledF(lpGraphics _gra,float X, float Y, float sX, float sY, lpImage img)
{
	_hp_CheckGraphics
	lpFunc Func = 0x512950;
	__asm {
		push sY
		push sX
		push Y
		push X
		mov eax,img
		mov ecx, _gra
		call Func
		add esp,0x10
	}
}

lpImage Gra::LoadMemoryImage(unsigned long* data, int w, int h)
{
	lpImage img = PVZNew(0x6C);
	//new()
	//Sexy::MemoryImage::MemoryImage(SexyAppBase * theApp, MemoryImage * this)
	lpFunc Func = 0x590170;
	//Sexy::MemoryImage::SetBits(bool commitBits, int theHeight, int theWidth, unsigned long* theBits, ecx = MemoryImage * this)
	lpFunc Func2 = 0x592E10;
	unsigned int App = PVZ_BASE;

	__asm {
		push App
		push img
		call Func

		push 0
		push h
		push w
		push data
		mov ecx, img
		call Func2
	}

	return img;
}

void Gra::FreeMemoryImage(lpImage tar)
{
	unsigned int Func = 0x590430;
	__asm {
		mov ecx,tar
		call Func
	}
	PVZDelete((void*)tar);
}
