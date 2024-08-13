#pragma once
#include <iostream>
#include <map>
#include <unordered_map>

extern bool Font_UsingCache;
extern int Font_MaxCache;

struct Font {
	unsigned int mSize = 0;
	void* mTrueTypeFont = nullptr;

};
Font* LoadFontFromMemory(const char* font_id);
unsigned long* GenerateStringImage(Font* font, int font_size, const char* text, unsigned int* dst_width, unsigned int* dst_height);

