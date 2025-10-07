#include "Block.h"
#include"BlockManager.h"
#include<DxLib.h>

#include"../../../Manager/Sound/SoundManager.h"

#include"../../../Scene/Game/GameScene.h"

#include"../../Player/Player.h"


Block::Block(void)
{

}

void Block::Create(TYPE type, int baseModelId, int mapX, int mapY, int mapZ)
{
	// ブロックの種類
	type_ = type;

	// モデルのハンドルID
	unit_.model_ = MV1DuplicateModel(baseModelId);

	// 1ブロックあたりの大きさ
	const float SIZE_BLOCK = BlockManager::SIZE_BLOCK;

	// 1ブロックあたりの半分の大きさ
	const float SIZE_HALF_BLOCK = (SIZE_BLOCK / 2.0f);

	// 引数で指定されたマップ座標から座標を計算する
	// 今回の３Ｄモデルの中心座標は、ブロックの中心に位置する
	float x = static_cast<float>(mapX);
	float y = static_cast<float>(mapY);
	float z = static_cast<float>(mapZ);

	// 大きさ設定
	MV1SetScale(unit_.model_, SCALES);

	// 座標設定
	unit_.pos_ = VGet(
		x * SIZE_BLOCK + SIZE_HALF_BLOCK,
		y * SIZE_BLOCK + SIZE_HALF_BLOCK,
		z * SIZE_BLOCK + SIZE_HALF_BLOCK
	);;
	MV1SetPosition(unit_.model_, unit_.pos_);


}

void Block::SubLoad(void)
{
	Utility::LoadImg(textureId_, "Data/Model/Rock/Rock.png");

	unit_.para_.speed = 5.0f;

	unit_.para_.colliShape = CollisionShape::AABB;
	unit_.para_.size = { BlockManager::SIZE_BLOCK ,BlockManager::SIZE_BLOCK ,BlockManager::SIZE_BLOCK };
	unit_.isAlive_ = true;

	aliveNeedRatio_ = 0.01f;
}

void Block::SubInit(void)
{
	unit_.isAlive_ = true;

	unit_.para_.size = VScale(unit_.para_.size, 1.3f);
}

void Block::SubUpdate(void)
{
	VECTOR vec = {};

	if (CheckHitKey(KEY_INPUT_0)) { vec.x++; }
	if (CheckHitKey(KEY_INPUT_9)) { vec.x--; }
	if (CheckHitKey(KEY_INPUT_6)) { vec.y++; }
	if (CheckHitKey(KEY_INPUT_7)) { vec.y--; }

	vec = Utility::Normalize(vec);

	unit_.pos_ = VAdd(unit_.pos_, VScale(vec, unit_.para_.speed));
}

void Block::SubDraw(void)
{
//	DrawSphere3D(VAdd(unit_.pos_, unit_.para_.center), 10.0f, 16, GetColor(255, 0, 0), GetColor(255, 0, 0), true);
//
//	 //デバッグ用に当たり判定の表示
//	VECTOR debugPos[8] =
//	{
//		VAdd(unit_.pos_, VTransform({ -unit_.para_.size.x / 2.0f, -unit_.para_.size.y / 2.0f, -unit_.para_.size.z / 2.0f },Utility::MatrixAllMultY({unit_.angle_}))),
//		VAdd(unit_.pos_, VTransform({  unit_.para_.size.x / 2.0f, -unit_.para_.size.y / 2.0f, -unit_.para_.size.z / 2.0f },Utility::MatrixAllMultY({unit_.angle_}))),
//		VAdd(unit_.pos_, VTransform({ -unit_.para_.size.x / 2.0f,  unit_.para_.size.y / 2.0f, -unit_.para_.size.z / 2.0f },Utility::MatrixAllMultY({unit_.angle_}))),
//		VAdd(unit_.pos_, VTransform({  unit_.para_.size.x / 2.0f,  unit_.para_.size.y / 2.0f, -unit_.para_.size.z / 2.0f },Utility::MatrixAllMultY({unit_.angle_}))),
//		VAdd(unit_.pos_, VTransform({ -unit_.para_.size.x / 2.0f, -unit_.para_.size.y / 2.0f,  unit_.para_.size.z / 2.0f },Utility::MatrixAllMultY({unit_.angle_}))),
//		VAdd(unit_.pos_, VTransform({  unit_.para_.size.x / 2.0f, -unit_.para_.size.y / 2.0f,  unit_.para_.size.z / 2.0f },Utility::MatrixAllMultY({unit_.angle_}))),
//		VAdd(unit_.pos_, VTransform({ -unit_.para_.size.x / 2.0f,  unit_.para_.size.y / 2.0f,  unit_.para_.size.z / 2.0f },Utility::MatrixAllMultY({unit_.angle_}))),
//		VAdd(unit_.pos_, VTransform({  unit_.para_.size.x / 2.0f,  unit_.para_.size.y / 2.0f,  unit_.para_.size.z / 2.0f },Utility::MatrixAllMultY({unit_.angle_})))
//	};
//	for (int i = 0; i < 8; i++) {
//		DrawSphere3D(debugPos[i], 3.0f, 30, GetColor(255, 0, 0), GetColor(255, 0, 0), true);
//	}
//
//	for (auto& vPos : GetVoxelCenters()) {
//		DrawSphere3D(vPos, GetCellSize() / 2, 30, 0xffffff, 0xffffff, true);
//	}
}

void Block::SubRelease(void)
{

}

void Block::OnCollision(UnitBase* other)
{
	if (dynamic_cast<PlayerPunch*>(other)) {
		if (ApplyBrush(other->GetUnit(), 200)) {
			GameScene::Shake();
			Smng::GetIns().Play(SOUND::OBJECT_BREAK, true, 150);
		}
		return;
	}

	if (dynamic_cast<ThrowObjBase*>(other)) {
		if (ApplyBrush(other->GetUnit(), 200)) {
			GameScene::Shake();
			Smng::GetIns().Play(SOUND::OBJECT_BREAK, false, 150);
		}
		return;
	}
}