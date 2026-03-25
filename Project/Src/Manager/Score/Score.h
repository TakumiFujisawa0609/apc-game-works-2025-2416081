#pragma once

#include<list>

#include"../../Object/Common/DataLoad/ParameterLoad.h"

class Score
{
private:

	Score();
	~Score() = default;

	// インスタンス
	static Score* ins;

	// 外部ファイル管理
	ParameterLoad parameter;

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

	// コンボ数の最高記録をコンボ上限数で割った最大コンボ数割合を取得
	float BestRecordComboRatio(void)const { return ((float)bestRecordCombo / (float)COMBO_MAX); }

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


#pragma region スコア値の定数定義

	// ベーススコア～～～～～～～～～～～～～～～～～～～～～～～～

	// ボスのHP減少によるスコア加算の倍率（HP1につきのスコア）
	const unsigned char BOSS_HP_SHARPEN_SCORE_RATE = (unsigned char)parameter.GetParameter("BossHpSharpenScoreRate");

	// ボスのライフロストによるスコア加算
	const unsigned short BOSS_LIFE_LOST_SCORE = (unsigned short)parameter.GetParameter("BossLifeLostScore");

	// プレイヤーがステージを破壊したときのスコア加算倍率（1セルごとのスコア）
	const unsigned short STAGE_PER_CELL_SCORE = (unsigned short)parameter.GetParameter("StagePerCellScore");

	// ～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～


	// ボーナススコア～～～～～～～～～～～～～～～～～～～～～

	// クリアタイムによるボーナススコアのベース値(理論値)
	// 計算方法 <ベース値 - ( (ベース値 ÷ 下記減少量レート) × クリアタイム )>
	const int TIME_BONUS_SCORE_BASE = (int)parameter.GetParameter("TimeBonusScoreBase");

	// クリアタイムによるボーナススコアの減少量において
	// 何フレームかけてボーナススコアが0になるか
	const int TIME_BONUS_SCORE_DECREASE_SPEED = (int)parameter.GetParameter("TimeBonusScoreDecreaseSpeed");


	// プレイヤーの残りHPによるボーナススコアのベース値(理論値)
	// 計算方法 <ベース値 × プレイヤーの残りHP割合>
	const int PLAYER_HP_BONUS_SCORE_BASE = (int)parameter.GetParameter("PlayerHpBonusScoreBase");

	// 最大コンボ数によるボーナススコアのベース値(理論値)
	// 計算方法 <ベース値 × (最大コンボ数 ÷ 上限コンボ数)>
	const int MAX_COMBO_BONUS_SCORE_BASE = (int)parameter.GetParameter("MaxComboBonusScoreBase");

	// ～～～～～～～～～～～～～～～～～～～～～～～～～～～～

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