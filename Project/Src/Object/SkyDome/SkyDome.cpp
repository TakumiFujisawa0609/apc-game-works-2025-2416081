#include"SkyDome.h"

void SkyDome::Load(void)
{
	trans_.Load("SkyDome/Skydome");
	trans_.scale = SCALE;
}

void SkyDome::Update(void)
{
	trans_.AddAngleYDeg(0.02f);
	if (trans_.angle.y >= Utility::Deg2RadF(360.0f)) { trans_.angle.y = 0.0f; }
}

void SkyDome::Draw(void)
{
	trans_.Draw();
}