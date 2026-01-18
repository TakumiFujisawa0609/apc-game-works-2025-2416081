#include "Ranking.h"

#include<sstream>
#include<fstream>
#include<iostream>

#include"../../Utility/Utility.h"

Ranking* Ranking::ins = nullptr;

Ranking::Ranking() : 
	rankingList(),
	lastAddScoreRankIndex(-1),

	faileOpenErr(false)
{
}

void Ranking::Load(void)
{
	// 最大ランキング数分の領域を確保
	rankingList.resize(RANKING_MAX);

#pragma region ランキングデータを読み込む
	// CSVファイルを開く
	std::ifstream ifs = std::ifstream("Data/Ranking/Ranking.csv");

	// エラーフラグをfalseに初期化
	faileOpenErr = false;

	// 例外処理：ファイルが開けなかった場合
	if (!ifs) {
		// エラーメッセージを表示
		std::cerr << "ランキングデータの読み込みに失敗しました。" << '\n';

		// エラーフラグを立てる
		faileOpenErr = true;

		return;
	}

	// ファイルから2行読み込み
	std::string scoreLine, nameLine;
	getline(ifs, scoreLine); getline(ifs, nameLine);

	// 読み込んだ行をカンマ区切りで分割し、配列に格納
	std::vector<std::string> scores = Utility::Split(scoreLine, ',');
	std::vector<std::string> names = Utility::Split(nameLine, ',');

	// 配列に格納されたデータをランキングリストに格納
	for (size_t i = 0; i < scores.size() && i < names.size() && i < rankingList.size(); i++) {
		rankingList[i].score = std::stoi(scores[i]);
		rankingList[i].name = names[i];
	}

	// ファイルを閉じる
	ifs.close();
#pragma endregion
}

void Ranking::Save(void)
{
	// ロードに失敗していたらすでにあるデータを破壊してしまう恐れがあるのでセーブを行わない
	if (faileOpenErr) {
		// ランキングリストをクリア
		rankingList.clear();
		return;
	}

#pragma region ランキングデータを保存する
	// CSVファイルを開く
	std::ofstream ofs = std::ofstream("Data/Ranking/Ranking.csv");

	// 例外処理：ファイルが開けなかった場合
	if (!ofs) {
		// エラーメッセージを表示
		std::cerr << "ランキングデータの保存に失敗しました。" << '\n';
		// ランキングリストをクリア
		rankingList.clear();
		return;
	}

	// ランキングリストのスコアの内容をカンマ区切りで1行にまとめて保存する
	for (auto& score : rankingList) { ofs << score.score << ","; }
	// 最後のカンマを削除する
	ofs.seekp(-1, std::ios_base::end);

	// 改行を入れる
	ofs << '\n';

	// 次にランキングリストの名前の内容をカンマ区切りで1行にまとめて保存する
	for (auto& score : rankingList) { ofs << score.name << ","; }
	// 最後のカンマを削除する
	ofs.seekp(-1, std::ios_base::end);

	// ファイルを閉じる
	ofs.close();
#pragma endregion

	// 保存を終えたらリストをクリア
	rankingList.clear();
}

void Ranking::AddScore(int score)
{
	// 最終追加スコアランクインデックスをリセット
	lastAddScoreRankIndex = -1;

	// 上から探索し、スコアを挿入する位置を見つける
	for (size_t i = 0; i < rankingList.size(); i++) {

		// 未設定を発見した場合、シンプルにそこに挿入して終了する
		if (rankingList[i].score == -1) {
			// スコアを挿入する
			rankingList[i].score = score;

			// スコア追加位置を記録する
			lastAddScoreRankIndex = i;

			// ソートは必要ないのでそのまま処理を終了
			break;
		}

		// 新規スコアが現在のスコアより大きい場合
		if(score > rankingList[i].score) {
			// 現在スコアを一時変数に保持しておく
			int work = rankingList[i].score;

			// 現在スコアを新規スコアで上書きする
			rankingList[i].score = score;

			// スコア追加位置を記録する
			lastAddScoreRankIndex = i;

			// 以降のスコアを１つずつ後ろにずらして並べ替える
			for (size_t j = rankingList.size() - 1; j > i; j--) { rankingList[j] = rankingList[j - 1]; }

			// 一時変数に保持しておいたスコアを挿入する
			if (i < rankingList.size() - 2) { rankingList[i + 1].score = work; }

			// 終了
			break;
		}

	}
}

void Ranking::SetLastAddScoreName(const std::string& name)
{
	// 最後に追加したスコアのインデックスが有効な場合のみ名前を設定する
	if (lastAddScoreRankIndex != -1 && 0 <= lastAddScoreRankIndex && lastAddScoreRankIndex < (int)(rankingList.size())) {
		rankingList[lastAddScoreRankIndex].name = name;
	}
}

void Ranking::AllDeleteRankingList(void)
{
	// 全て未設定(-1,--)にする
	for (auto& score : rankingList) { score.score = -1; score.name = "--"; }
}

void Ranking::DeleteRankingList(int rankIndex)
{
	// 指定されたランクインデックスが範囲内であるか確認（例外処理）
	if (rankIndex < 0 || (int)(rankingList.size()) <= rankIndex) { return; }

	// 指定されたランクインデックスのスコアを削除（未設定にする）
	rankingList[rankIndex].score = -1;
	rankingList[rankIndex].name = "--";

	// 削除後、ランキングリストを詰める
	for (size_t i = rankIndex; i < rankingList.size() - 1; i++) { rankingList[i] = rankingList[i + 1]; }

	// 最後の要素を未設定にする
	rankingList[rankingList.size() - 1].score = -1;
	rankingList[rankingList.size() - 1].name = "--";
}
