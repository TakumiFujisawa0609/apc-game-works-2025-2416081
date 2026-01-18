#pragma once

#include<vector>
#include<string>

class Ranking
{
private:
	// コンストラクタ・デストラクタ
	Ranking();
	~Ranking() = default;

	// インスタンス
	static Ranking* ins;
public:

#pragma region シングルトン定義
	// 生成/初期化処理
	static void CreateIns(void) { if (ins == nullptr) { ins = new Ranking(); ins->Load(); } }
	// 取得
	static Ranking& GetIns(void) { return *ins; }
	// 終了処理/削除
	static void DeleteIns(void) { if (ins != nullptr) { ins->Save(); delete ins; ins = nullptr; } }

	//　コピーコンストラクタ・代入演算子禁止
	Ranking(const Ranking&) = delete;
	Ranking& operator=(const Ranking&) = delete;
#pragma endregion
	
#pragma region 読み込み/保存
	void Load(void);
	void Save(void);
#pragma endregion

	// ランキングデータ構造体
	struct RankingData {
		// スコア
		int score = -1;
		// 名前
		std::string name = "--";
	};

#pragma region メイン処理
	// ランキングを取得
	const std::vector<RankingData>& GetRankingList(void)const { return rankingList; }

	// ランキングにスコアを追加（ランキング外のスコアだった場合はそのまま終了）
	void AddScore(int score);

	// 最終追加スコアの順位を取得（数字は配列の添字(インデックス)で取得）（ランク外は-1）
	int GetLastAddScoreRankIndex(void)const { return lastAddScoreRankIndex; }

	// 最終追加スコアの名前を設定
	void SetLastAddScoreName(const std::string& name);
#pragma endregion

#pragma region 開発者向け機能
	// ランキングリストを全消去
	void AllDeleteRankingList(void);

	// 特定の順位のスコアを消去
	void DeleteRankingList(int rankIndex);

	// 読み込み失敗の検出用フラグを取得（開発者向け機能）
	bool GetFaileOpenErr(void)const { return faileOpenErr; }
#pragma endregion

private:
	// ランキング最大数
	static constexpr int RANKING_MAX = 30;

	// ランキングリスト
	std::vector<RankingData> rankingList;

	// 最終追加スコアの順位を保持するメンバ変数（数字は配列の添字(インデックス)で記憶）（ランク外は-1）
	int lastAddScoreRankIndex;

	// 読み込み失敗の検出用フラグ
	bool faileOpenErr;
};