#include"PsychoRockShooter.h"

#include"../../../../../Manager/Collision/CollisionManager.h"

PsychoRockShooter::PsychoRockShooter(const Vector3& bossPos, const Vector3& playerPos):
	bossPos(bossPos),
	playerPos(playerPos),

	model(-1)

{
}

PsychoRockShooter::~PsychoRockShooter()
{
}

void PsychoRockShooter::Load(void)
{
	model = MV1LoadModel("Data/Model/Boss/Attack/Rock.mv1");

	for (unsigned char i = 0; i < NUM_MAX; i++) {
		rocks[i] = new PsychoRock(model, playerPos);
		rocks[i]->Load();
	}
}

void PsychoRockShooter::Init(void)
{
	for (PsychoRock*& rock : rocks) { rock->Init(); }
}

void PsychoRockShooter::Update(void)
{
	for (PsychoRock*& rock : rocks) { rock->Update(); }
}

void PsychoRockShooter::Draw(void)
{
	for (PsychoRock*& rock : rocks) { rock->Draw(); }
}

void PsychoRockShooter::AlphaDraw(void)
{
	for (PsychoRock*& rock : rocks) { rock->AlphaDraw(); }
}

void PsychoRockShooter::Release(void)
{
	for (PsychoRock*& rock : rocks) {
		if (!rock) { continue; }
		rock->Release();
		delete rock;
		rock = nullptr;
	}

	MV1DeleteModel(model);
}

void PsychoRockShooter::Set(void)
{
	for (int i = 0; i < ONE_SHOT_NUM; i++) {
		float x = 0.0f, z = 0.0f;
		RandPos(x, z);

		for (auto& rock : rocks) {
			if (rock->GetState() == PsychoRock::STATE::NON) {
				rock->Set(x, z);
				break;
			}
		}
	}
}

void PsychoRockShooter::On(void)
{
	for (auto& rock : rocks) {
		if (rock->GetState() == PsychoRock::STATE::RISE_PREPARA) {
			rock->On();
		}
	}
}

void PsychoRockShooter::RandPos(float& x, float& z)
{
	float angle = (float)GetRand(360);
	float radius = (float)GetRand((int)RANDAM_RADIUS_MAX);

	Vector3 work =
		Vector3(0.0f, 0.0f, CONST_RADIUS_SUB).TransMat(MGetRotY(angle)) +
		Vector3(0.0f, 0.0f, radius).TransMat(MGetRotY(angle));
	work += bossPos;

	x = work.x; z = work.z;
}
