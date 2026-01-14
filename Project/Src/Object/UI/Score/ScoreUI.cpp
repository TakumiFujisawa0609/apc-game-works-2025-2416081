#include"ScoreUI.h"

#include"../../../Application/Application.h"

#include"../../../Manager/Score/Score.h"

ScoreUI::ScoreUI(const Vector2& SCORE_POS) :
	SCORE_POS(SCORE_POS),
	displayScore(0),

	addScore(),

	scoreAddInterval(0)
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
	if (++scoreAddInterval > SCORE_ADD_INTERVAL) {
		scoreAddInterval = SCORE_ADD_INTERVAL;

		int add = 0;
		for (int loop = 0; loop < MAX_ADD_SCORE_NUM; loop++) {
			int a = Score::GetIns().AddScoreApplyAndGet();
			if (a == 0) { break; }
			add += a;
		}
		if (add > 0) {
			scoreAddInterval = 0;
			if (!addScore.empty()) { addScore.back().pos.y -= 15.0f; }
			addScore.emplace_back(AddScoreInfo(add, SCORE_POS));
		}
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
	//SetFontSize(48);

	// スコア表示
	DrawFormatString((int)SCORE_POS.x, (int)SCORE_POS.y, 0xffffff, "SCORE : %08d", displayScore);

	//SetFontSize(32);

	// 加算スコアを表示
	for (AddScoreInfo& add : addScore) { add.Draw(); }

	// コンボ数表示
	DrawFormatString(5, (int)(App::SCREEN_SIZE_Y * 0.5f - 16), 0xffffff, "AtCombo：%02d", Score::GetIns().DamageCombo());
	DrawFormatString(5, (int)(App::SCREEN_SIZE_Y * 0.5f + 16), 0xffffff, "BrCombo：%02d", Score::GetIns().BreakCombo());
}

void ScoreUI::Release(void)
{

}