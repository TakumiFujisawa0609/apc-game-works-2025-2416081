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

	//// 1ブロックあたりのサイズ
	static constexpr float SIZE_BLOCK = 100.0f;

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
	int textureId_;

	std::vector<Block*>blocks_;

	// ブロック用のモデルハンドルID
	std::map<int, int>models_;

	void LoadMapCsvData(void);
};

