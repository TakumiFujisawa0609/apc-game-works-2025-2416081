#pragma once

#include<vector>

#include"../SceneBase.h"

#include"../../Object/ActorBase.h"
#include"../../Object/VoxelBase.h"


class ClearScene : public SceneBase
{
public:
	ClearScene(std::vector<VoxelBase::MeshBatch> stageBatches = {}, const char* stageTexturePath = nullptr);
	~ClearScene()override;

	void Load(void)override;
	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;
	void Release(void)override;

private:
	int mainScreen;

	int img_;

	std::vector<ActorBase*>objects;
	// 配列の中から特定のオブジェクトを探す
	template<typename T>
	T* ObjSerch(void) {
		for (ActorBase*& obj : objects) {
			if (!obj) continue;
			if (auto* ptr = dynamic_cast<T*>(obj)) {
				return ptr;
			}
		}
		return nullptr;
	}

	// ランクイン判定
	bool rankinJudge;

	const std::vector<VoxelBase::MeshBatch> stageBatches;
	const int stageTexture;
};
