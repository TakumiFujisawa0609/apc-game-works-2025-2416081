#include"RockWallShooter.h"

RockWallShooter::RockWallShooter(const Vector3& bossPos, const Vector3& bossAngle) :
	bossPos(bossPos),
	bossAngle(bossAngle),

	modelId_(-1),
	textureId_(-1)
{
}

void RockWallShooter::Load(void)
{
	modelId_ = MV1LoadModel("Data/Model/Rock/Rock.mv1");
	Utility::LoadImg(textureId_, "Data/Model/Rock/Rock.png");

	for (unsigned char i = 0; i < WALL_MAX_NUM; i++) {
		walls_[i] = new RockWall(modelId_, textureId_);
	}
	for (RockWall*& wall : walls_) { wall->Load(); }
}

void RockWallShooter::Init(void)
{
	for (RockWall*& wall : walls_) { wall->Init(); }
}

void RockWallShooter::Update(void)
{
	for (RockWall*& wall : walls_) { wall->Update(); }
}

void RockWallShooter::Draw(void)
{
	for (RockWall*& wall : walls_) { wall->Draw(); }
}

void RockWallShooter::AlphaDraw(void)
{
	for (RockWall*& wall : walls_) { wall->AlphaDraw(); }
}

void RockWallShooter::Release(void)
{
	for (RockWall*& wall : walls_) {
		if (!wall) { continue; }
		wall->Release();
		delete wall;
		wall = nullptr;
	}
}

void RockWallShooter::On(void)
{
	Vector3 vec = VTransform(CREATE_ST_LOCAL_POS, Utility::MatrixAllMultY({ bossAngle }));
	Vector3 pos = bossPos + vec;

	vec = vec.Normalized() * ONE_DISTANCE;

	unsigned char serchNum = 0;

	for (unsigned char i = 0; i < CREATE_WALL_NUM; i++) {

		for (unsigned char j = serchNum; j < WALL_MAX_NUM; j++) {

			if (!walls_[j]->GetJudgeFlg()) {
				walls_[j]->On(pos);
				pos += vec;
				serchNum = j + 1;
				break;
			}

		}
	}
}