#pragma once
#include<map>
#include<vector>

class Block;

#include"../../ActorBase.h"

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

	std::vector<ColliderBase*> GetCollider(void)const override;

	const std::vector<ActorBase*>GetBlocks(void)const;

	void SetCamera(Camera* c);

	void StageRevival(void);


private:
	int textureId_;

	std::vector<Block*>blocks_;

	// ブロック用のモデルハンドルID
	std::map<int, int>models_;

	void LoadMapCsvData(void);

	void SubInit(void)override {}
	void SubUpdate(void)override {}
	void SubDraw(void)override {}
	void SubAlphaDraw(void)override {}
	void SubRelease(void)override {}
};

