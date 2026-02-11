#include"RockWallShooter.h"

RockWallShooter::RockWallShooter(const Vector3& bossPos, const Vector3& bossAngle) :
	bossPos(bossPos),
	bossAngle(bossAngle),

	modelId(-1),
	textureId(-1)
{
}

void RockWallShooter::Load(void)
{
	modelId = MV1LoadModel("Data/Model/Rock/Rock.mv1");
	LoadImg(textureId, "Data/Model/Rock/Rock.png");

	for (unsigned char i = 0; i < WALL_MAX_NUM; i++) {
		walls[i] = new RockWall(modelId, textureId);
	}
	for (RockWall*& wall : walls) { wall->Load(); }
}

void RockWallShooter::Init(void)
{
	for (RockWall*& wall : walls) { wall->Init(); }
}

void RockWallShooter::Update(void)
{
	for (RockWall*& wall : walls) { wall->Update(); }
}

void RockWallShooter::Draw(void)
{
	for (RockWall*& wall : walls) { wall->Draw(); }
}

void RockWallShooter::AlphaDraw(void)
{
	for (RockWall*& wall : walls) { wall->AlphaDraw(); }
}

void RockWallShooter::Release(void)
{
	for (RockWall*& wall : walls) {
		if (!wall) { continue; }
		wall->Release();
		delete wall;
		wall = nullptr;
	}
	DeleteGraph(textureId);
	MV1DeleteModel(modelId);
}

void RockWallShooter::On(void)
{
	Vector3 vec = VTransform(CREATE_ST_LOCAL_POS, MatrixAllMultY({ bossAngle }));
	Vector3 pos = bossPos + vec;

	vec = vec.Normalized() * ONE_DISTANCE;

	unsigned char serchNum = 0;

	for (unsigned char i = 0; i < CREATE_WALL_NUM; i++) {

		for (unsigned char j = serchNum; j < WALL_MAX_NUM; j++) {

			if (!walls[j]->GetJudgeFlg()) {
				walls[j]->On(pos);
				pos += vec;
				serchNum = j + 1;
				break;
			}

		}
	}
}