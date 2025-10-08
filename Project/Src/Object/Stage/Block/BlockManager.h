#pragma once
#include <DxLib.h>

#include<map>
#include<vector>

class Block;

class BlockManager
{
public:
	// ブロック用のモデル種類
	static constexpr int NUM_BLOCK_MODELS = 14;

	// ブロックの数(マップのサイズ)
	static constexpr int NUM_BLOCK_X = 20;
	static constexpr int NUM_BLOCK_Z = 20;
	static constexpr int NUM_BLOCK_Y = 2;

	// 1ブロックあたりのサイズ
	static constexpr float SIZE_BLOCK = 100.0f;

	// マップ全体のサイズ
	static constexpr float WORLD_SIZE_X =
		SIZE_BLOCK * static_cast<float>(NUM_BLOCK_X);

	static constexpr float WORLD_SIZE_Z =
		SIZE_BLOCK * static_cast<float>(NUM_BLOCK_Z);

	// コンストラクタ
	BlockManager(void);
	// デストラクタ
	~BlockManager(void);

	void Load(void);
	void Update(void);
	void Draw(void);
	void Release(void);

	const std::vector<UnitBase*>GetBlocks(void)const;

	void SetCamera(Camera* c);


private:

	// マップデータ
	Block* blocks_[NUM_BLOCK_Y][NUM_BLOCK_Z][NUM_BLOCK_X];

	// ブロック用のモデルハンドルID
	std::map<int, int>models_;

	void LoadMapCsvData(void);
};

