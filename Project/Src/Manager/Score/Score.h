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

#pragma region シングルトン定義
	// 生成/初期化処理
	static void CreateIns(void) { if (ins == nullptr) { ins = new Score(); ins->Reset(); } }
	// 取得
	static Score& GetIns(void) { return *ins; }
	// 終了処理/消去
	static void DeleteIns(void) { if (ins != nullptr) { ins->Reset(); delete ins; ins = nullptr; } }

	//　コピーコンストラクタ・代入演算子禁止
	Score(const Score&) = delete;
	Score& operator=(const Score&) = delete;
#pragma endregion

	// リセット
	void Reset(void);

	// 更新処理
	void Update(void);

#pragma region 各ゲット関数

	// スコアの取得
	int TotalScore(void)const { return totalScore; }

	// ダメージコンボ数の取得
	unsigned char DamageCombo(void)const { return attackCombo; }

	// 破壊コンボ数の取得
	unsigned char BreakCombo(void)const { return breakCombo; }

	// 合計コンボ数の取得
	unsigned char TotalCombo(void)const { return attackCombo + breakCombo; }

#pragma endregion

#pragma region スコア加算
	// 加算予定スコアに追加（コンボなし）
	bool ScoreAddOther(int add);

	// 加算予定スコアに追加（ダメージコンボ）
	void ScoreAddAttack(int add);

	// 加算予定スコアに追加（破壊コンボ）
	void ScoreAddBreak(int add);

	// 加算予定スコアを１つ適用するとともに加算量を取得する
	int AddScoreApplyAndGet(void);
#pragma endregion

private:

	// 現在のスコア
	int totalScore;

	// 加算予定スコア
	std::list<int>addScore;

#pragma region コンボ数 / スコア補正倍率

	// アタックコンボ～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～

	// アタックコンボ最大数（定数）
	const unsigned char ATTACK_COMBO_MAX = 99;

	// アタックコンボの最大補正倍率（定数）
	const float ATTACK_COMBO_RATE_MAX = 1.25f;

	// アタックコンボ１に対しての補正倍率（定数）
	const float ATTACK_COMBO_RATE = ATTACK_COMBO_RATE_MAX / (float)ATTACK_COMBO_MAX;

	// アタックコンボ継続時間(フレーム数)（定数）
	const unsigned char ATTACK_COMBO_TIME = 250;

	// アタックコンボ加算インターバル（定数）
	const unsigned char ATTACK_COMBO_INTERVAL = 10;

	// アタックコンボ数（変数）
	unsigned char attackCombo;

	// 最終アタックコンボからの時間(フレーム数)（変数）
	unsigned char attackComboTime;

	// アタックコンボ加算インターバル（変数）
	unsigned char attackComboInterval;

	// ～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～アタックコンボ
	
	// 破壊コンボ～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～

	// 破壊コンボ最大数（定数）
	const unsigned char BREAK_COMBO_MAX = 99;

	// 破壊コンボの最大補正倍率（定数）
	const float BREAK_COMBO_RATE_MAX = 0.75f;

	// 破壊コンボ１に対しての補正倍率（定数）
	const float BREAK_COMBO_RATE = BREAK_COMBO_RATE_MAX / (float)BREAK_COMBO_MAX;

	// 破壊コンボ継続時間(フレーム数)（定数）
	const unsigned char BREAK_COMBO_TIME = 250;

	// 破壊コンボ加算インターバル（定数）
	const unsigned char BREAK_COMBO_INTERVAL = 10;

	// 破壊コンボ数（変数）
	unsigned char breakCombo;

	// 最終破壊コンボからの時間(フレーム数)（変数）
	unsigned char breakComboTime;

	// 破壊コンボ加算インターバル（変数）
	unsigned char breakComboInterval;

	// ～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～破壊コンボ

	// デフォルト補正倍率
	const float DEFAULT_RATE = 1.0f;

	// スコア補正倍率（コンボ数からスコア補正倍率を算出する）
	float Rate(void)const {
		// デフォルト補正倍率
		float ret = DEFAULT_RATE;

		// ダメージコンボの補正倍率を加算
		ret += ATTACK_COMBO_RATE * (float)attackCombo;

		// 破壊コンボの補正倍率を加算
		ret += BREAK_COMBO_RATE * (float)breakCombo;

		return ret;
	}

#pragma endregion
};