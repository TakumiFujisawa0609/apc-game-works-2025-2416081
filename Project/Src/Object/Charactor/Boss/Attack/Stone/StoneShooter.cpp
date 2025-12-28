#include"StoneShooter.h"

StoneShooter::StoneShooter(const Vector3& bossPos, const Vector3& bossAngle):


	bossPos(bossPos),
	bossAngle(bossAngle)
{
}

StoneShooter::~StoneShooter()
{
}

void StoneShooter::Load(void)
{
	model_ = MV1LoadModel("Data/Model/Boss/Attack/Rock.mv1");

	for (unsigned char i = 0; i < NUM_MAX; i++) {
		stones_[i] = new Stone(model_);
		stones_[i]->Load();
	}
}

void StoneShooter::Init(void)
{
	for (Stone*& stone : stones_) { stone->Init(); }
}

void StoneShooter::Update(void)
{
	for (Stone*& stone : stones_) { stone->Update(); }
}

void StoneShooter::Draw(void)
{
	for (Stone*& stone : stones_) { stone->Draw(); }
}

void StoneShooter::AlphaDraw(void)
{
	for (Stone*& stone : stones_) { stone->AlphaDraw(); }
}

void StoneShooter::Release(void)
{
	for (Stone*& stone : stones_) {
		if (!stone) { continue; }
		stone->Release();
		delete stone;
		stone = nullptr;
	}

	MV1DeleteModel(model_);
}

void StoneShooter::On(void)
{
	Vector3 dir = VTransform(LOCAL_POS, Utility::MatrixAllMultXYZ({ bossAngle }));
	dir = VTransform(dir, MGetRotY(-ONE_DIFF * (ONE_SHOT_NUM / 2)));

	for (int i = 0; i < ONE_SHOT_NUM; i++) {
		Vector3 pos = bossPos + dir;
		Vector3 vec = pos - bossPos;
		vec.y = 0.0f;
		vec.Normalize();

		for (auto& stone : stones_) {
			if (stone->GetJudgeFlg() == false) {
				stone->On(pos, vec);
				break;
			}
		}
		dir.TransMatOwn(MGetRotY(ONE_DIFF));
	}
}
