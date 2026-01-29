#pragma once
#include"../../SceneBase.h"

#include"../../../Utility/Utility.h"

class RankInScene : public SceneBase
{
public:
	RankInScene(const int& mainScreen);
	~RankInScene()override = default;

	void Load(void)override;
	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;
	void Release(void)override;

private:
	const int& mainScreen;

	int tempScreen;


	int backImage;

	float scale;
	const float START_SCALE = 8.0f;
	const float LAST_SCALE = 1.0f;

	float NowScaleRatio(void) { return ValueNormalizeRatio(scale, LAST_SCALE, START_SCALE); }
};