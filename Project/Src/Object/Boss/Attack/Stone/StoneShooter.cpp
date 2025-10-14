#include"StoneShooter.h"

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
}

void StoneShooter::Init(void)
{
}

void StoneShooter::Update(void)
{
}

void StoneShooter::Draw(void)
{
}

void StoneShooter::Release(void)
{
	MV1DeleteModel(model_);
}

void StoneShooter::On(void)
{
	VECTOR dir = VTransform(LOCAL_POS, Utility::MatrixAllMultXYZ({ bossAngle }));
	dir = VTransform(dir, MGetRotY(ONE_DIFF * (ONE_SHOT_NUM / 2)));

	for (int i = 0; i < ONE_SHOT_NUM; i++) {

		VECTOR pos = VAdd(bossPos, dir);
		VECTOR vec = VSub(pos, bossPos);
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
		dir = VTransform(dir, MGetRotY(ONE_DIFF));
	}
}
