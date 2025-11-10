#include"SkyDome.h"

SkyDome::SkyDome():
	model_(-1),
	pos_(),
	angle_()
{
}

SkyDome::~SkyDome()
{
}
void SkyDome::Load(void)
{
	model_ = MV1LoadModel("Data/Model/SkyDome/Skydome.mv1");
	MV1SetScale(model_, Utility::FtoV(SCALE));
}

void SkyDome::Update(void)
{
	angle_.y += Utility::Deg2RadF(0.02f);
	if (angle_.y >= Utility::Deg2RadF(360.0f)) { angle_.y = 0.0f; }
}

void SkyDome::Draw(void)
{
	Utility::MV1ModelMatrix(model_, pos_, { angle_ });
	MV1DrawModel(model_);
}

void SkyDome::Release(void)
{
	MV1DeleteModel(model_);
}