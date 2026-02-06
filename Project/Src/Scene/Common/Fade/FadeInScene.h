#pragma once

#include<memory>

#include"../../SceneBase.h"

#include"../../../Utility/Utility.h"

class FadeInScene : public SceneBase
{
public:

	FadeInScene(std::shared_ptr<SceneBase>scene, unsigned short FADE_TIME, unsigned int FADE_COLOR, bool sceneTransitionKinds);
	~FadeInScene()override = default;

	void Load(void)override {}
	void Init(void) override {}
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override {}

private:
	// チェンジ先のシーン
	std::shared_ptr<SceneBase> scene;

	// シーン遷移の種類（true = ChangeScene()、false = JumpScene()）
	bool sceneTransitionKinds;

	// フェード時間
	const unsigned short FADE_TIME;
	// フェードカラー
	unsigned int FADE_COLOR;

	// フェードカウンター
	unsigned short fadeCounter;

	// フェードの合計の時間(FADE_TIME)とカウンター(fadeCounter)から現在の時間経過の割合を算出する
	float FadeRatio(void) { return ((float)fadeCounter / (float)FADE_TIME); }

	const int ALPHA_MAX = 255;

	// フェード時間経過の割合から実際のアルファ値を返す
	int FadeAlpha(void) { return (int)((float)ALPHA_MAX * RatioReverse(FadeRatio())); }
};