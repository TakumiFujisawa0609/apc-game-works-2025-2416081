#include"PsychoRockShooter.h"

#include"../../../../Manager/Collision/Collision.h"

PsychoRockShooter::PsychoRockShooter(const VECTOR& bossPos, const VECTOR& playerPos):
	bossPos(bossPos),
	playerPos(playerPos),

	model_(-1)

{
}

PsychoRockShooter::~PsychoRockShooter()
{
}

void PsychoRockShooter::Load(void)
{
	model_ = MV1LoadModel("Data/Model/Boss/Attack/Rock.mv1");

	rocks_.reserve(ONE_SHOT_NUM * 2);
}

void PsychoRockShooter::Update(void)
{
	for (auto& rock : rocks_) { rock->Update(); }
}

void PsychoRockShooter::Draw(void)
{
	for (auto& rock : rocks_) { rock->Draw(); }
}

void PsychoRockShooter::Release(void)
{
	for (auto& rock : rocks_) {
		if (!rock) { continue; }
		rock->Release();
		delete rock;
		rock = nullptr;
	}
	rocks_.clear();

	MV1DeleteModel(model_);
}

void PsychoRockShooter::Set(void)
{
	for (int i = 0; i < ONE_SHOT_NUM; i++) {
		float x = 0.0f, z = 0.0f;
		RandPos(x, z);

		bool recycle = false;

		for (auto& rock : rocks_) {
			if (rock->GetState() == (int)PsychoRock::STATE::NON) {
				rock->Set(x, z);
				recycle = true;
				break;
			}
		}
		if (recycle) { continue; }

		rocks_.emplace_back(new PsychoRock(model_, playerPos));
		rocks_.back()->Load();
		rocks_.back()->Set(x, z);
		Collision::AddEnemy(rocks_.back());
	}
}

void PsychoRockShooter::On(void)
{
	for (auto& rock : rocks_) {
		if (rock->GetState() == (int)PsychoRock::STATE::RISE_PREPARA) {
			rock->On();
		}
	}
}

void PsychoRockShooter::RandPos(float& x, float& z)
{
	float angle = (float)GetRand(360);
	float radius = (float)GetRand(RANDAM_RADIUS_MAX);

	VECTOR work = VAdd(VTransform({ 0.0f,0.0f,(float)CONST_RADIUS_SUB }, MGetRotY(angle)), VTransform({ 0.0f,0.0f,radius }, MGetRotY(angle)));
	work = VAdd(bossPos, work);

	x = work.x; z = work.z;
}
