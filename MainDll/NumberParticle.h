#pragma once
#include "Common.h"
#include "Font.h"

class TextParticle;

TextParticle* SpawnNumParticleAt(int X,int Y,SexyString mStr);

class TextParticle {
public:
	float mX = 0;
	float mY = 0;
	SexyColor mColor = { 255,255,255,255 };
	SexyString mText = "";
	float mSpeedX = 0;
	float mSpeedY = 0;
	float mLifeTime = 0;
	float mLifeTimeTotal = 0;
	float mSize = 30;

public:
	TextParticle();
	~TextParticle();

	void Update(unsigned long long delta);
	void Present(lpGraphics g, Font* m_Font);
	bool GetExist();
};