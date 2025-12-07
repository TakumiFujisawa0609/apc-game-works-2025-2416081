#include"FallManager.h"

#include"../../../../../Utility/Utility.h"

FallManager::FallManager(const Vector3& playerPos):
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

	for (unsigned char i = 0; i < FALL_NUM_MAX; i++) {
		falls_[i] = new Fall(model);
	}

	for (Fall*& fall : falls_) { fall->Load(); }
}

void FallManager::Init(void)
{
	for (Fall*& fall : falls_) { fall->Init(); }
}

void FallManager::Update(void)
{
	for (Fall*& fall : falls_) { fall->Update(); }
}

void FallManager::Draw(void)
{
	for (Fall*& fall : falls_) { fall->Draw(); }
}
void FallManager::AlphaDraw(void)
{
	for (Fall*& fall : falls_) { fall->AlphaDraw(); }
}

void FallManager::Release(void)
{
	for (Fall*& fall : falls_) {
		if (!fall) { continue; }
		fall->Release();
		delete fall;
		fall = nullptr;
	}

}

void FallManager::Set(void)
{
	for (Fall*& fall : falls_) {
		if (!fall->GetJudgeFlg()) {
			fall->Set(playerPos + LOCAL_POS);
			return;
		}
	}
}

void FallManager::On(void)
{
	for (auto& fall : falls_) {
		if (fall->GetState() == Fall::STATE::IDLE) {
			fall->On();
		}
	}
}
