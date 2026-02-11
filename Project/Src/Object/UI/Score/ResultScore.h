#pragma once

#include<list>

#include"../../ActorBase.h"

class ResultScore : public ActorBase
{
public:
	ResultScore();
	~ResultScore()override = default;

	void Load(void)override {}
	void Init(void)override;
	void Update(void)override;
	void Draw(void)override {}
	void AlphaDraw(void)override {}
	void UiDraw(void);
	void Release(void)override {}

	bool EasingEnd(void)const { return totalEasingEnd; }
	void EasingSkip(void);

private:
#pragma region スコア
	// ゲーム中稼いだスコア（最終結果）
	unsigned int baseScore;
	// ゲーム中稼いだスコア（表示用）
	unsigned int baseDisplayScore;
	// ゲーム中稼いだスコアを表示する座標
	const Vector2I BASE_DISPLAY_SCORE_POS = Vector2I(700, 235);
	// ゲーム中稼いだスコア（表示用）の加算が終了したかどうか
	bool baseEasingEnd;

	// クリアタイムや残りHPなどのクリア画面で加算されるボーナススコア（最終結果）
	unsigned int bonusScore;
	// クリアタイムや残りHPなどのクリア画面で加算されるボーナススコア（表示用）
	unsigned int bonusDisplayScore;
	// クリアタイムや残りHPなどのクリア画面で加算されるボーナススコアを表示する座標
	const Vector2I BONUS_DISPLAY_SCORE_POS = Vector2I(700, 377);
	// ボーナススコアに加算された数値
	unsigned int bonusAddScore;
	// クリアタイムや残りHPなどのクリア画面で加算されるボーナススコア（表示用）の加算が終了したかどうか
	bool bonusEasingEnd;

	// 上記2つの合計スコア（最終結果）
	unsigned int totalScore;
	// 上記2つの合計スコア（表示用）
	unsigned int totalDisplayScore;
	//  上記2つの合計スコアを表示する座標
	const Vector2I TOTAL_DISPLAY_SCORE_POS = Vector2I(700, 575);
	// 上記2つの合計スコア（表示用）の加算が終了したかどうか
	bool totalEasingEnd;
#pragma endregion

	// 表示用スコアを現在のスコアへ近づけるのにあたっての補完数値
	const float DISPLAY_SCORE_SMOOTH = 0.1f;

	// 加算スコアを表示させる時間(フレーム数)
	static const unsigned char ADD_SCORE_ALIVE_TIME = 100;

	// メインのボーナススコア表示座標からの相対座標で加算スコアを表示させる位置
	static const Vector2I ADD_BONUS_SCORE_OFFSET_POS;

	// 加算ボーナススコアの演出を管理する構造体
	struct AddBonusScoreInfo {
		// 生成
		AddBonusScoreInfo(int addScore, const Vector2I& SCORE_POS, const int& addScoreFont) :
			addScore(addScore),
			aliveTime(ADD_SCORE_ALIVE_TIME),
			pos(SCORE_POS),
			addScoreFont(addScoreFont)
		{
			pos += ADD_BONUS_SCORE_OFFSET_POS;
		}

		// 表示用フォントハンドル
		const int& addScoreFont;

		// 加算スコアの数値
		int addScore = 0;

		// 生存時間
		unsigned char aliveTime = 0;

		// 描画座標
		Vector2I pos;

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

	// 加算ボーナススコア
	std::list<AddBonusScoreInfo>addBonusScore;
};