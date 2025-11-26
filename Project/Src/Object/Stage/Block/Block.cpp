#include "Block.h"
#include"BlockManager.h"
#include<DxLib.h>

#include"../../../Manager/Sound/SoundManager.h"

#include"../../../Scene/Game/GameScene.h"

Block::Block(TYPE type, int baseModelId, int textureId, int mapX, int mapY, int mapZ) :
	type_(type)
{
	// モデルを複製する
	ModelDuplicate(baseModelId);
	
	// ボクセルメッシュ生成に必要な情報を設定する
	VoxelInfoInit(
		Vector3(
			BlockManager::SIZE_BLOCK,
			BlockManager::SIZE_BLOCK,
			BlockManager::SIZE_BLOCK),
		textureId,
		20.0f,
		Vector3(0.0f, 0.0f, 0.0f),
		0.1f
	);

	// 1ブロックあたりの大きさ
	const float SIZE_BLOCK = BlockManager::SIZE_BLOCK;

	// 1ブロックあたりの半分の大きさ
	const float SIZE_HALF_BLOCK = (SIZE_BLOCK / 2.0f);

	// 引数で指定されたマップ座標から座標を計算する
	trans_.pos = {
		((float)mapX * SIZE_BLOCK) + SIZE_HALF_BLOCK,
		((float)mapY * SIZE_BLOCK) + SIZE_HALF_BLOCK,
		((float)mapZ * SIZE_BLOCK) + SIZE_HALF_BLOCK
	};

	// 大きさ設定
	trans_.scale = SCALE;

	// モデル制御情報を反映する
	trans_.Attach();
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