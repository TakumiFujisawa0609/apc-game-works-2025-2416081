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

	// ベーススコア(ゲーム中)の取得
	int BaseScore(void)const { return baseScore; }

	// ボーナススコアの取得
	int BonusScore(void)const { return bonusScore; }

	// 合計スコアの取得
	int TotalScore(void)const { return (baseScore + bonusScore); }

	// コンボ数の取得
	unsigned char Combo(void)const { return combo; }

	// コンボ数の最高記録の取得
	unsigned char BestRecordCombo(void)const { return bestRecordCombo; }

#pragma endregion

#pragma region スコア加算
	// 加算予定スコアに追加（コンボ加算なし、倍率補正あり）
	bool ScoreAddOther(int add);

	// 加算予定スコアに追加（コンボ加算あり、倍率補正あり）
	void ScoreAddCombo(int add);

	// 加算予定スコアに追加（コンボ加算なし、倍率補正なし）
	void ScoreAddBonus(int add);

	// 加算予定スコアを１つベーススコアに適用するとともに加算量を取得する
	int AddBaseScoreApplyAndGet(void);

	// 加算予定スコアを１つボーナススコアに適用するとともに加算量を取得する
	int AddBonusScoreApplyAndGet(void);
#pragma endregion

private:

	// ベーススコア(ゲーム中)
	int baseScore;

	// ボーナススコア
	int bonusScore;

	// 加算予定スコア
	std::list<int>addScore;

#pragma region コンボ数 / スコア補正倍率

	// コンボ最大数（定数）
	const unsigned char COMBO_MAX = 99;

	// コンボの最大補正倍率（定数）
	const float COMBO_RATE_MAX = 0.75f;

	// コンボ１に対しての補正倍率（定数）
	const float COMBO_RATE = COMBO_RATE_MAX / (float)COMBO_MAX;

	// コンボ継続時間(フレーム数)（定数）
	const unsigned char COMBO_TIME = 250;

	// コンボ加算インターバル（定数）
	const unsigned char COMBO_INTERVAL = 10;

	// コンボ数（変数）
	unsigned char combo;

	// 最終コンボからの時間(フレーム数)（変数）
	unsigned char comboTime;

	// コンボ加算インターバル（変数）
	unsigned char comboInterval;

	// コンボ数の最高記録（変数）
	unsigned char bestRecordCombo;

	// デフォルト補正倍率
	const float DEFAULT_RATE = 1.0f;

	// スコア補正倍率（コンボ数からスコア補正倍率を算出する）
	float Rate(void)const {
		// デフォルト補正倍率
		float ret = DEFAULT_RATE;

		// コンボの補正倍率を加算
		ret += COMBO_RATE * (float)combo;

		return ret;
	}

#pragma endregion
};