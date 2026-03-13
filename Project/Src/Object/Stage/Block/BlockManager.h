#pragma once
#include<map>
#include<vector>

#include "Block.h"

class BlockManager : public ActorBase
{
public:
	// 1ブロックあたりのサイズ
	static constexpr float SIZE_BLOCK = 100.0f;

	// コンストラクタ
	BlockManager(void);
	// デストラクタ
	~BlockManager(void);

	void Load(void)override;
	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;
	void AlphaDraw(void)override {}
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

	// MagicaVoxelで生成したplyファイルからマップデータを読み込む際、最初に無視する行数
	const int IGNORE_LINE = 11;

	// プレイヤーのリスポーン地点のブロックの番号テーブル
	const std::vector<int> RESPAWN_BLOCK_NUM_TABLE = {
		382,
		383,
		384,
		385,
		399,
		400,
		401,
		402,
		418,
		419,
		420,
		421,
		438,
		439,
		440,
		441,
	};
};

