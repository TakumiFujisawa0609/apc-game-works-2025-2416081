#include"PsychoRockShooter.h"

#include"../../../../../Manager/Collision/CollisionManager.h"

PsychoRockShooter::PsychoRockShooter(const Vector3& bossPos, const Vector3& playerPos):
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

	rocks_.reserve(NUM_MAX);

	for (unsigned char i = 0; i < NUM_MAX; i++) {
		rocks_.emplace_back(new PsychoRock(model_, playerPos));
	}

	for (PsychoRock*& rock : rocks_) { rock->Load(); }
}

void PsychoRockShooter::Init(void)
{
	for (PsychoRock*& rock : rocks_) { rock->Init(); }
}

void PsychoRockShooter::Update(void)
{
	for (PsychoRock*& rock : rocks_) { rock->Update(); }
}

void PsychoRockShooter::Draw(void)
{
	for (PsychoRock*& rock : rocks_) { rock->Draw(); }
}

void PsychoRockShooter::AlphaDraw(void)
{
	for (PsychoRock*& rock : rocks_) { rock->AlphaDraw(); }
}

void PsychoRockShooter::Release(void)
{
	for (PsychoRock*& rock : rocks_) {
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

		//bool recycle = false;

		for (auto& rock : rocks_) {
			if (rock->GetState() == PsychoRock::STATE::NON) {
				rock->Set(x, z);
				//recycle = true;
				break;
			}
		}
		//if (recycle) { continue; }

		//rocks_.emplace_back(new PsychoRock(model_, playerPos));
		//rocks_.back()->Load();
		//rocks_.back()->Set(x, z);
		//Collision::AddEnemy(rocks_.back());
	}
}

void PsychoRockShooter::On(void)
{
	for (auto& rock : rocks_) {
		if (rock->GetState() == PsychoRock::STATE::RISE_PREPARA) {
			rock->On();
		}
	}
}

void PsychoRockShooter::RandPos(float& x, float& z)
{
	float angle = (float)GetRand(360);
	float radius = (float)GetRand(RANDAM_RADIUS_MAX);

	Vector3 work =
		Vector3(0.0f, 0.0f, CONST_RADIUS_SUB).TransMat(MGetRotY(angle)) +
		Vector3(0.0f, 0.0f, radius).TransMat(MGetRotY(angle));
	work += bossPos;

	x = work.x; z = work.z;
}
