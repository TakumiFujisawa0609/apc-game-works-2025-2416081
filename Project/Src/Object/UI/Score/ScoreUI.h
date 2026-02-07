#pragma once

#include<list>

#include"../../ActorBase.h"

#include"../../../Manager/Font/FontManager.h"

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

	void AllAddScoreApply(void);

private:

	// スコア表示座標
	const Vector2 SCORE_POS;

	// 表示用スコア
	unsigned int displayScore;

	// 表示用スコアを現在のスコアへ近づけるのにあたっての補完数値
	const float DISPLAY_SCORE_SMOOTH = 0.1f;

	// 加算スコアを表示させる時間(フレーム数)
	static const unsigned char ADD_SCORE_ALIVE_TIME = 100;

	// メインのスコア表示座標からの相対座標で加算スコアを表示させる位置
	static const Vector2 ADD_SCORE_OFFSET_POS;

	struct AddScoreInfo {

		// 生成
		AddScoreInfo(int addScore, const Vector2& SCORE_POS, const int& addScoreFont) :
			addScore(addScore),
			aliveTime(ADD_SCORE_ALIVE_TIME),
			pos(SCORE_POS),
			addScoreFont(addScoreFont)
		{
			pos += ADD_SCORE_OFFSET_POS;
		}

		// 表示用フォントハンドル
		const int& addScoreFont;

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
		void Draw(void)const {
			DrawFormatStringToHandle(
				(int)pos.x, (int)((pos.y + 20.0f) - ((ADD_SCORE_ALIVE_TIME - aliveTime) * 0.75f)),
				0xffff00, addScoreFont,
				"+%d", addScore
			);
		}
	};

	// 加算スコア
	std::list<AddScoreInfo>addScore;

	// １度に適用させる最大数（無限ループ防止）
	const unsigned char MAX_ADD_SCORE_NUM = 255;

	// 加算予定スコアを適用させるインターバル（フレーム数）
	const unsigned char SCORE_ADD_INTERVAL = 10;
	// 加算予定スコアを適用させるインターバルを計測するためのカウンター
	unsigned char scoreAddInterval;

	// スコア表示のステップ数
	static const unsigned char SCORE_DISP_STEP = 6;

	// スコア表示の色テーブル
	const unsigned int SCORE_DISP_COLOR_TABLE[SCORE_DISP_STEP] = {
		// 白、緑、青、黄、赤、紫の順番
		0xffffff, 0x00ff00, 0x0000ff, 0xffff00, 0xff0000, 0xff00ff
	};

	// スコア表示の色のしきい値テーブル
	const unsigned int SOCRE_DISP_THRESHOLD_TABLE[SCORE_DISP_STEP] = { 0, 100000, 300000, 500000, 1000000, 1500000 };

	// スコア表示の色を取得
	unsigned int GetScoreDispColor(unsigned int score)const {
		for(int i = SCORE_DISP_STEP - 1; i >= 0; --i) {
			if (score >= SOCRE_DISP_THRESHOLD_TABLE[i]) { return SCORE_DISP_COLOR_TABLE[i]; }
		}
		return SCORE_DISP_COLOR_TABLE[SCORE_DISP_STEP - 1];
	}

	int comboBackImage[12];
	float comboAnimeIndex;
	float comboAnimeSigned;
};