#pragma once

#include<list>

class Score
{
private:

	Score();
	~Score() = default;

	// インスタンス
	static Score* ins;

public:

#pragma region シングルトン
	// 生成/初期化処理
	static void CreateIns(void) { if (ins == nullptr) { ins = new Score(); ins->Reset(); } }
	// 取得
	static Score& GetIns(void) { return *ins; }
	// 終了処理/消去
	static void DeleteIns(void) { if (ins != nullptr) { ins->Reset(); delete ins; ins = nullptr; } }
#pragma endregion

	// リセット
	void Reset(void);

	// 更新処理
	void Update(void);

#pragma region 各ゲット関数

	// スコアの取得
	int TotalScore(void)const { return totalScore; }

	// ダメージコンボ数の取得
	unsigned char DamageCombo(void)const { return damageCombo; }

	// 破壊コンボ数の取得
	unsigned char BreakCombo(void)const { return breakCombo; }

#pragma endregion

#pragma region スコア加算
	// 加算予定スコアに追加（コンボなし）
	void ScoreAddOther(int add) {
		if (add <= 0) { return; }
		addScore.emplace_back((int)((float)add * Rate()));
	}

	// 加算予定スコアに追加（ダメージコンボ）
	void ScoreAddDamage(int add) {
		ScoreAddOther(add);
		damageComboTime = 0;
		if (++damageCombo > DAMAGE_COMBO_MAX) { damageCombo = DAMAGE_COMBO_MAX; }
	}

	// 加算予定スコアに追加（破壊コンボ）
	void ScoreAddBreak(int add) {
		ScoreAddOther(add);
		breakComboTime = 0;
		if (++breakCombo > BREAK_COMBO_MAX) { breakCombo = BREAK_COMBO_MAX; }
	}

	// 加算予定スコアを１つ適用するとともに加算量を取得する
	int AddScoreApplyAndGet(void) {
		// 加算予定スコアがない場合は０を返して終了
		if (addScore.empty()) { return 0; }

		// 加算予定スコアのリストの先頭の数値を 取得/破棄
		int addAmount = addScore.front();
		addScore.pop_front();

		// 現在のスコアに加算
		totalScore += addAmount;

		// 加算量を返す
		return addAmount;
	}
#pragma endregion


private:

	// 現在のスコア
	int totalScore;

	// 加算予定スコア
	std::list<int>addScore;

#pragma region コンボ数 / スコア補正倍率

	// ダメージコンボ～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～

	// ダメージコンボ最大数（定数）
	const unsigned char DAMAGE_COMBO_MAX = 99;

	// ダメージコンボの最大補正倍率（定数）
	const float DAMAGE_COMBO_RATE_MAX = 0.5f;

	// ダメージコンボ１に対しての補正倍率（定数）
	const float DAMAGE_COMBO_RATE = DAMAGE_COMBO_RATE_MAX / (float)DAMAGE_COMBO_MAX;

	// ダメージコンボ継続時間(フレーム数)（定数）
	const unsigned char DAMAGE_COMBO_TIME = 250;

	// ダメージコンボ数（変数）
	unsigned char damageCombo;

	// 最終ダメージコンボからの時間(フレーム数)（変数）
	unsigned char damageComboTime;

	// ～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～ダメージコンボ
	
	// 破壊コンボ～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～

	// 破壊コンボ最大数（定数）
	const unsigned char BREAK_COMBO_MAX = 99;

	// 破壊コンボの最大補正倍率（定数）
	const float BREAK_COMBO_RATE_MAX = 1.0f;

	// 破壊コンボ１に対しての補正倍率（定数）
	const float BREAK_COMBO_RATE = BREAK_COMBO_RATE_MAX / (float)BREAK_COMBO_MAX;

	// 破壊コンボ継続時間(フレーム数)（定数）
	const unsigned char BREAK_COMBO_TIME = 250;

	// 破壊コンボ数（変数）
	unsigned char breakCombo;

	// 最終破壊コンボからの時間(フレーム数)（変数）
	unsigned char breakComboTime;

	// ～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～破壊コンボ

	// デフォルト補正倍率
	const float DEFAULT_RATE = 1.0f;

	// スコア補正倍率（コンボ数からスコア補正倍率を算出する）
	float Rate(void)const {
		// デフォルト補正倍率
		float ret = DEFAULT_RATE;

		// ダメージコンボの補正倍率を加算
		ret += DAMAGE_COMBO_RATE * (float)damageCombo;

		// 破壊コンボの補正倍率を加算
		ret += BREAK_COMBO_RATE * (float)breakCombo;

		return ret;
	}
#pragma endregion
};