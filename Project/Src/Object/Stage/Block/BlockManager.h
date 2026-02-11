#pragma once
#include<map>
#include<vector>

#include "Block.h"

class BlockManager : public ActorBase
{
public:
	// ブロック用のモデル種類
	static constexpr int NUM_BLOCK_MODELS = 14;

	//// 1ブロックあたりのサイズ
	static constexpr float SIZE_BLOCK = 100.0f;

	// コンストラクタ
	BlockManager(void);
	// デストラクタ
	~BlockManager(void);

	void Load(void)override;
	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;
	void AlphaDraw(void)override;
	void Release(void)override;

	// 生成したブロック全てのコライダーを取得
	std::vector<ColliderBase*> GetCollider(void)const override;

	// 生成したブロック全てのメッシュを取得
	const std::vector<MeshBatch> GetMesh(void)const;

	// プレイヤーリスポーンによる地形復活
	void StageRevival(void);

private:
	// ブロック用のテクスチャハンドル
	std::map<int, int> textures;
	// ブロック用のモデルハンドル
	std::map<int, int>models;

	// ブロック群のインスタンス格納配列
	std::vector<Block*>blocks;

	// マップデータ読み込み、ブロック生成
	void LoadMapCsvData(void);
};

