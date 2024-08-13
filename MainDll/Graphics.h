#pragma once
#include "Common.h"
#include "pvzclass.h"
#include "Font.h"

namespace Gra {
	void SetColor(lpGraphics _gra, const SexyColor TargetColor);
	void SetColorizeImages(lpGraphics _gra,bool IsEnable);
	void FillRect(lpGraphics _gra,int X,int Y,int Width,int Height);
	void SetFont(lpGraphics _gra, const lpFont* Font);

	//使用原版的DrawString，汉化版会出莫名其妙的bug，不能自定义样式
	void DrawStringOri(lpGraphics _gra, int x, int y, SexyString str, SexyColor color, const lpFont font);
	
	void SetOutlined(bool aIsOutlined);
	void DrawStringPlus(lpGraphics _gra, int x, int y, SexyString target_str, SexyColor color, Font* target_font, int font_size = 35, float font_scale = 1.0f);

	void DrawImageScaledF(lpGraphics _gra,float X,float Y,float sX,float sY,lpImage img);

	lpImage LoadMemoryImage(unsigned long* data,int w,int h);
	void FreeMemoryImage(lpImage tar);
}