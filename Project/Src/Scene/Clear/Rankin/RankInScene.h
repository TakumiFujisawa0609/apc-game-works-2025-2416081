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

	// 「〇位にランクインしました」の表示座標
	const Vector2I RANK_POP_POS = { 220, 270 };

	// 「ランキングにのせる名前を入力してね！」の表示座標
	const Vector2I NAME_PROMPT_POS = { 268, 400 };

	// 「（１～７文字）」の表示座標
	const Vector2I NAME_LENGTH_NOTE_POS = { 528, 465 };

	// 入力中の名前の表示座標
	const Vector2I NAME_DISPLAY_POS = { 383, 540 };

	float genericCounter;
};