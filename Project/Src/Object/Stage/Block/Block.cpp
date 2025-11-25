#include "Block.h"
#include"BlockManager.h"
#include<DxLib.h>

#include"../../../Manager/Sound/SoundManager.h"

#include"../../../Scene/Game/GameScene.h"

#include"../../Player/Player.h"

#include"../../Boss/Attack/Stone/Stone.h"
#include"../../Boss/Attack/Fall/Fall.h"
#include"../../Boss/Attack/PsychoRock/PsychoRock.h"

Block::Block(void)
{

}

void Block::Create(TYPE type, int baseModelId,int textureId, int mapX, int mapY, int mapZ)
{
	// ブロックの種類
	type_ = type;

	// モデルのハンドルID
	unit_.model_ = MV1DuplicateModel(baseModelId);

	// 
	textureId_ = textureId;

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
	unit_.para_.speed = 5.0f;

	unit_.para_.colliShape = CollisionShape::AABB;
	unit_.para_.size = { BlockManager::SIZE_BLOCK ,BlockManager::SIZE_BLOCK ,BlockManager::SIZE_BLOCK };
	unit_.isAlive_ = true;

	aliveNeedRatio_ = 0.1f;

	cell_ = 20.0f;
}

void Block::SubInit(void)
{
	SetJudge(true);
	SetIsDraw(true);
}

void Block::SubUpdate(void)
{
}

void Block::SubDraw(void)
{
}

void Block::SubRelease(void)
{

}

void Block::OnCollision(const ColliderBase& collider)
{
	switch (collider.GetTag())
	{
	case TAG::PLAYER_PUNCH:
	case TAG::PLAYER_GOUGE:
	case TAG::PLAYER_THROWING:
	case TAG::GOLEM_ATTACK_FALL:
	case TAG::GOLEM_ATTACK_PSYCHOROCK:
	case TAG::GOLEM_ATTACK_STONE: {

		// ブロックを壊す
		ApplyBrush(255);

		// 画面を揺らす
		GameScene::Shake();

		// 音を鳴らす
		Smng::GetIns().Play(SOUND::OBJECT_BREAK, true, 150);

		break;
	}
	default: { break; }	// それ以外は何もしない
	}

	// 当たったセルのインデックスをクリアする
	ColliderSerch<VoxelCollider>().back()->ClearHitCellIdxs();
}