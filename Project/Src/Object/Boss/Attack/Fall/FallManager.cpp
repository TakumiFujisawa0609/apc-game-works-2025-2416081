#include"FallManager.h"

#include"../../../../Utility/Utility.h"

#include"../../../../Manager/Collision/Collision.h"

FallManager::FallManager(const VECTOR& playerPos):
	model(-1),
	falls_(),

	playerPos(playerPos)
{
}

FallManager::~FallManager()
{
}

void FallManager::Load(void)
{
	model = MV1LoadModel("Data/Model/Boss/Attack/Rock.mv1");
	MV1SetScale(model, { 2.0f,2.0f,2.0f });

	falls_.reserve(3);
}

void FallManager::Init(void)
{

}

void FallManager::Update(void)
{
	for (auto& fall : falls_) { fall->Update(); }
}

void FallManager::Draw(void)
{
	for (auto& fall : falls_) { fall->Draw(); }
}

void FallManager::Release(void)
{
	for (auto& fall : falls_) {
		if (!fall) { continue; }
		fall->Release();
		delete fall;
		fall = nullptr;
	}
	falls_.clear();

}

void FallManager::On(void)
{
	VECTOR pos = VAdd(playerPos, LOCAL_POS);


	for (auto& fall : falls_) {
		if (fall->GetUnit().isAlive_ == false) {
			fall->On(pos);
			return;
		}
	}

	falls_.emplace_back(new Fall());
	falls_.back()->ModelLoad(model);
	falls_.back()->Load();
	falls_.back()->Init();
	falls_.back()->On(pos);

	Collision::AddEnemy(falls_.back());
}
