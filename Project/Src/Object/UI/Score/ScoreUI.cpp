#include"ScoreUI.h"

#include"../../../Manager/Score/Score.h"

ScoreUI::ScoreUI(const Vector2& SCORE_POS) :
	SCORE_POS(SCORE_POS),
	displayScore(0)
{
}

void ScoreUI::Load(void)
{

}

void ScoreUI::Init(void)
{
	displayScore = 0;
}

void ScoreUI::Update(void)
{
	// 加算予定スコアの適用と加算スコアの取得
	for (int loop = 0; loop < MAX_ADD_SCORE_NUM; loop++) {
		int add = Score::GetIns().AddScoreApplyAndGet();
		if (add == 0) { break; }
		addScore.emplace_back(AddScoreInfo(add, SCORE_POS));
	}

	// 加算スコアの更新
	for (auto it = addScore.begin(); it != addScore.end(); ) {
		if (!it->AliveUpdate()) { it = addScore.erase(it); }
		else { ++it; }
	}

	// 表示用スコアを現在のスコアへ近づける（徐々に）
	int smoothScoreSub = (int)((Score::GetIns().TotalScore() - displayScore) * DISPLAY_SCORE_SMOOTH);
	displayScore += (smoothScoreSub > 0) ? smoothScoreSub : Score::GetIns().TotalScore() - displayScore;
}

void ScoreUI::UiDraw(void)
{
	// スコア表示
	SetFontSize(48);
	DrawFormatStringF(SCORE_POS.x, SCORE_POS.y, 0xffffff, "SCORE : %08d", displayScore);

	// 加算スコアを表示
	SetFontSize(32);
	for (AddScoreInfo& add : addScore) { add.Draw(); }

	SetFontSize(16);
}

void ScoreUI::Release(void)
{

}
