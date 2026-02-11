#include"FallManager.h"

#include"../../../../../Utility/Utility.h"

FallManager::FallManager(const Vector3& playerPos):
	model(-1),
	falls(),

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
		falls[i] = new Fall(model);
	}

	for (Fall*& fall : falls) { fall->Load(); }
}

void FallManager::Init(void)
{
	for (Fall*& fall : falls) { fall->Init(); }
}

void FallManager::Update(void)
{
	for (Fall*& fall : falls) { fall->Update(); }
}

void FallManager::Draw(void)
{
	for (Fall*& fall : falls) { fall->Draw(); }
}
void FallManager::AlphaDraw(void)
{
	for (Fall*& fall : falls) { fall->AlphaDraw(); }
}

void FallManager::Release(void)
{
	for (Fall*& fall : falls) {
		if (!fall) { continue; }
		fall->Release();
		delete fall;
		fall = nullptr;
	}

}

void FallManager::Set(void)
{
	for (Fall*& fall : falls) {
		if (!fall->GetJudgeFlg()) {
			fall->Set(playerPos + LOCAL_POS);
			return;
		}
	}
}

void FallManager::On(void)
{
	for (auto& fall : falls) {
		if (fall->GetState() == Fall::STATE::IDLE) {
			fall->On();
		}
	}
}
