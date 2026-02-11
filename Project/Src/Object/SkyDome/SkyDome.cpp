#include"SkyDome.h"

void SkyDome::Load(void)
{
	trans.Load("SkyDome/Skydome");
	trans.scale = SCALE;
}

void SkyDome::Init(void)
{
	trans.pos = POS;
}

void SkyDome::Update(void)
{
	trans.AddAngleYDeg(0.02f);
	if (trans.angle.y >= Deg2Rad(360.0f)) { trans.angle.y = 0.0f; }
}

void SkyDome::Draw(void)
{
	trans.Draw();
}