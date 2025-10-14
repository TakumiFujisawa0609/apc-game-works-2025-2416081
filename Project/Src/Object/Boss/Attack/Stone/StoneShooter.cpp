#include"StoneShooter.h"

#include"../../../../Manager/Collision/Collision.h"

StoneShooter::StoneShooter(const VECTOR& bossPos, const VECTOR& bossAngle):


	bossPos(bossPos),
	bossAngle(bossAngle)
{
}

StoneShooter::~StoneShooter()
{
}

void StoneShooter::Load(void)
{
	stones_.reserve(ONE_SHOT_NUM * 2);

	model_ = MV1LoadModel("Data/Model/Boss/Attack/Rock.mv1");
	MV1SetScale(model_, { 0.3f,0.3f,0.3f });
}

void StoneShooter::Init(void)
{
}

void StoneShooter::Update(void)
{
	for (auto& stone : stones_) { stone->Update(); }
}

void StoneShooter::Draw(void)
{
	for (auto& stone : stones_) { stone->Draw(); }
}

void StoneShooter::Release(void)
{
	for (auto& stone : stones_) {
		if (!stone) { continue; }
		stone->Release();
		delete stone;
		stone = nullptr;
	}
	stones_.clear();

	MV1DeleteModel(model_);
}

void StoneShooter::On(void)
{
	VECTOR dir = VTransform(LOCAL_POS, Utility::MatrixAllMultXYZ({ bossAngle }));
	dir = VTransform(dir, MGetRotY(-ONE_DIFF * (ONE_SHOT_NUM / 2)));

	for (int i = 0; i < ONE_SHOT_NUM; i++) {

		VECTOR pos = VAdd(bossPos, dir);
		VECTOR vec = VSub(pos, bossPos);
		vec.y = 0.0f;
		vec = Utility::Normalize(vec);

		bool recycle = false;

		for (auto& stone : stones_) {
			if (stone->GetUnit().isAlive_ == false) {
				stone->On(pos, vec);
				recycle = true;
				break;
			}
		}

		if (recycle) {
			dir = VTransform(dir, MGetRotY(ONE_DIFF));
			continue;
		}

		stones_.emplace_back(new Stone());
		stones_.back()->ModelLoad(model_);
		stones_.back()->Load();
		stones_.back()->Init();
		stones_.back()->On(pos, vec);

		Collision::AddEnemy(stones_.back());

		dir = VTransform(dir, MGetRotY(ONE_DIFF));
	}
}
