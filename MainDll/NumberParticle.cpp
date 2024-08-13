#include "NumberParticle.h"
#include "Graphics.h"

TextParticle::TextParticle() {}

TextParticle::~TextParticle() {}


void TextParticle::Update(unsigned long long delta)
{
	_hp_Check(GetExist());
	mLifeTime -= delta;
	mX += mSpeedX * delta;
	mY += mSpeedY * delta;
	mColor.a = (mLifeTime / mLifeTimeTotal) * 255;
}

void TextParticle::Present(lpGraphics g,Font* m_Font)
{
	_hp_Check((mLifeTime > 0));
	if (mText != "" && (mColor.a != 0))
	{
		Gra::DrawStringPlus(g, mX, mY,mText,mColor, m_Font, mSize);
	}
}

bool TextParticle::GetExist()
{
	return (mLifeTime > 0);
}

TextParticle* SpawnNumParticleAt(int X, int Y, SexyString mStr)
{
	TextParticle* tp = new TextParticle();
	tp->mX = X;
	tp->mY = Y;
	tp->mLifeTimeTotal = 2000;
	tp->mLifeTime = tp->mLifeTimeTotal;
	tp->mSpeedX = float(rand() % 100 -50) / 50.0f * 0.03f;
	tp->mSpeedY = float(rand() % 100 -50) / 50.0f * 0.03f;
	tp->mColor = { 255,0,0,255 };;
	tp->mSize = rand() % 10 + 35;
	tp->mText = mStr;

	return tp;
}
