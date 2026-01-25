#pragma once

#include"../../ActorBase.h"

class ResultScore : public ActorBase
{
public:
	ResultScore();
	~ResultScore()override = default;

	void Load(void)override;
	void Init(void)override;
	void Update(void)override;
	void Draw(void)override {}
	void AlphaDraw(void)override {}
	void UiDraw(void);
	void Release(void)override;

	bool EasingEnd(void)const { return totalEasingEnd; }

private:
#pragma region スコア
	// ゲーム中稼いだスコア（最終結果）
	unsigned int baseScore;
	// ゲーム中稼いだスコア（表示用）
	unsigned int baseDisplaScore;
	// ゲーム中稼いだスコア（表示用）の加算が終了したかどうか
	bool baseEasingEnd;

	// クリアタイムや残りHPなどのクリア画面で加算されるボーナススコア（最終結果）
	unsigned int bonusScore;
	// クリアタイムや残りHPなどのクリア画面で加算されるボーナススコア（表示用）
	unsigned int bonusDisplayScore;
	// クリアタイムや残りHPなどのクリア画面で加算されるボーナススコア（表示用）の加算が終了したかどうか
	bool bonusEasingEnd;

	// 上記2つの合計スコア（最終結果）
	unsigned int totalScore;
	// 上記2つの合計スコア（表示用）
	unsigned int totalDisplayScore;
	// 上記2つの合計スコア（表示用）の加算が終了したかどうか
	bool totalEasingEnd;
#pragma endregion

	// 表示用スコアを現在のスコアへ近づけるのにあたっての補完数値
	const float DISPLAY_SCORE_SMOOTH = 0.1f;
};