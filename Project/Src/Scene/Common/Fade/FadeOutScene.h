#pragma once

#include<memory>

#include"../../SceneBase.h"

class FadeOutScene : public SceneBase
{
public:
	FadeOutScene(unsigned short FADE_TIME, unsigned int FADE_COLOR);
	~FadeOutScene()override = default;

	void Load(void)override {}
	void Init(void) override {}
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override {}

private:
	// チェンジ先のシーン
	std::shared_ptr<SceneBase> scene;

	// フェード時間
	const unsigned short FADE_TIME;
	// フェードカラー
	const unsigned int FADE_COLOR;

	// フェードカウンター
	unsigned short fadeCounter;

	// フェードの合計の時間(FADE_TIME)とカウンター(fadeCounter)から現在の時間経過の割合を算出する
	float FadeRatio(void) { return ((float)fadeCounter / (float)FADE_TIME); }

	const int ALPHA_MAX = 255;

	// フェード時間経過の割合から実際のアルファ値を返す
	int FadeAlpha(void) { return (int)((float)ALPHA_MAX * FadeRatio()); }
};