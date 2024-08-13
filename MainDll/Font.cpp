#include "Font.h"
#include "stb_truetype.h"
#include "Common.h"
#include <Windows.h>

#define STB_TRUETYPE_IMPLEMENTATION

//bool Font_UsingCache = true;
//int Font_MaxCache = 20;

Font* LoadFontFromMemory(const char* font_id)
{
	HMODULE h_module = GetModuleHandleA("MainDll.dll");
	auto h_font_resource = FindResourceA(h_module, (char*)font_id, "TTF");
	_ASSERT(h_font_resource);
	auto h_font = LoadResource(h_module, h_font_resource);
	int h_font_size = SizeofResource(h_module, h_font_resource);


	Font* result = new Font;
	result->mSize = h_font_size;
	stbtt_fontinfo* info = new stbtt_fontinfo;
	result->mTrueTypeFont = info;

	
	auto error = stbtt_InitFont(info, (unsigned char*)h_font, 0);
	if (!error) {
		_hp_Show_Error("字体资源加载错误");
	}
	return result;
}

unsigned long* GenerateStringImage(Font* font, int font_size, const char* text, unsigned int* dst_width, unsigned int* dst_height)
{

	//获得字体缩放
    auto ttfinfo = (const stbtt_fontinfo*)font->mTrueTypeFont;
	float scale = stbtt_ScaleForPixelHeight(ttfinfo, font_size); // 32px high
	int x_be = 0.0f; // 开始位置的 x 坐标
	int y_be = 0.0f; // 开始位置的 y 坐标

	int ascent = 0;
	int descent = 0;
	int lineGap = 0;
	stbtt_GetFontVMetrics(ttfinfo, &ascent, &descent, &lineGap);

	/* 根据缩放调整字高 */
	ascent = roundf(ascent * scale);
	descent = roundf(descent * scale);

	int bitmap_w = 0; /* 位图的宽 */
	int bitmap_h = font_size; /* 位图的高 */
	

	for (int i = 0; i < strlen(text); ++i)
	{
		int advanceWidth = 0;
		int leftSideBearing = 0;
		stbtt_GetCodepointHMetrics(ttfinfo, text[i], &advanceWidth, &leftSideBearing);

		bitmap_w += roundf(advanceWidth * scale);

		int kern;
		kern = stbtt_GetCodepointKernAdvance(ttfinfo, text[i], text[i + 1]);
		bitmap_w += roundf(kern * scale);
	}

	unsigned char* bitmap = new unsigned char[bitmap_w * bitmap_h + 1];
	memset(bitmap, 0, bitmap_w * bitmap_h);
	for (int i = 0; i < strlen(text); ++i)
	{
		int advanceWidth = 0;
		int leftSideBearing = 0;
		stbtt_GetCodepointHMetrics(ttfinfo, text[i], &advanceWidth, &leftSideBearing);
		int c_x1, c_y1, c_x2, c_y2;
		stbtt_GetCodepointBitmapBox(ttfinfo, text[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);
		int y = ascent + c_y1;
		int byteOffset = x_be + roundf(leftSideBearing * scale) + (y * bitmap_w);
		stbtt_MakeCodepointBitmap(ttfinfo, bitmap + byteOffset, c_x2 - c_x1, c_y2 - c_y1, bitmap_w, scale, scale, text[i]);
		x_be += roundf(advanceWidth * scale);
		int kern;
		kern = stbtt_GetCodepointKernAdvance(ttfinfo, text[i], text[i + 1]);
		x_be += roundf(kern * scale);
	}
	*dst_width = bitmap_w;
	*dst_height = bitmap_h;

	unsigned char* bitmap_render = new unsigned char[bitmap_w*bitmap_h*4];
	for (size_t i = 0; i < bitmap_w* bitmap_h; i++)
	{
		bitmap_render[i * 4] = 255;
		bitmap_render[i * 4 + 1] = 255;
		bitmap_render[i * 4 + 2] = 255;
		bitmap_render[i * 4 + 3] = bitmap[i];
		if (bitmap[i])
			auto c = 0;
	}
	delete[] bitmap;
	return (unsigned long*)bitmap_render;
}

#undef STB_TRUETYPE_IMPLEMENTATION
