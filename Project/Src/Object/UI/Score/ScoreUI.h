#pragma once

#include<list>

#include"../../ActorBase.h"

class ScoreUI : public ActorBase
{
public:
	ScoreUI(const Vector2& SCORE_POS = { 20.0f,30.0f });
	~ScoreUI() = default;

	void Load(void)override;
	void Init(void)override;
	void Update(void)override;
	void Draw(void)override {}
	void AlphaDraw(void)override {}
	void UiDraw(void);
	void Release(void)override;

private:

	// スコア表示座標
	const Vector2 SCORE_POS;

	// 表示用スコア
	unsigned int displayScore;

	// 表示用スコアを現在のスコアへ近づけるのにあたっての補完数値
	const float DISPLAY_SCORE_SMOOTH = 0.1f;

	// 加算スコアを表示させる時間(フレーム数)
	static const unsigned char ADD_SCORE_ALIVE_TIME = 100;

	struct AddScoreInfo {

		// 生成
		AddScoreInfo(int addScore, const Vector2& SCORE_POS) :
			addScore(addScore),
			aliveTime(ADD_SCORE_ALIVE_TIME),
			pos(SCORE_POS)
		{
			pos.x += 400.0f;
		}

		// 加算スコアの数値
		int addScore = 0;

		// 生存時間
		unsigned char aliveTime = 0;

		// 描画座標
		Vector2 pos;

		// 生存時間の更新
		bool AliveUpdate(int value = 1) {
			if (aliveTime < value) { return false; }
			aliveTime -= value;
			if (aliveTime <= 0) { return false; }
			return true;
		}

		// 描画
		void Draw(void)const { DrawFormatString((int)pos.x, (int)((pos.y + 20.0f) - ((ADD_SCORE_ALIVE_TIME - aliveTime) * 0.75f)), 0xffffff, "+%d", addScore); }
	};

	// 加算スコア
	std::list<AddScoreInfo>addScore;

	// １度に適用させる最大数（無限ループ防止）
	const unsigned char MAX_ADD_SCORE_NUM = 255;

	// 加算予定スコアを適用させるインターバル（フレーム数）
	const unsigned char SCORE_ADD_INTERVAL = 10;

	unsigned char scoreAddInterval;
};