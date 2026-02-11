#include"ScoreUI.h"

#include"../../../Application/Application.h"

#include"../../../Manager/Score/Score.h"

const Vector2 ScoreUI::ADD_SCORE_OFFSET_POS = Vector2(430.0f, 0.0f);

ScoreUI::ScoreUI(const Vector2& SCORE_POS) :
	SCORE_POS(SCORE_POS),
	displayScore(0),


	addScore(),

	scoreAddInterval(0),

	comboAnimeIndex(),
	comboAnimeSigned(),
	comboBackImage()
{
}

void ScoreUI::Load(void)
{
	LoadArrayImg("Data/Image/Game/UI/ComboBack.png", 12, 4, 3, 311, 219, comboBackImage);
}

void ScoreUI::Init(void)
{
	displayScore = 0;
	comboAnimeSigned = 0.2f;
	comboAnimeIndex = 0;
}

void ScoreUI::Update(void)
{
	// 加算予定スコアの適用と加算スコアの取得
	if (++scoreAddInterval > SCORE_ADD_INTERVAL) {
		scoreAddInterval = SCORE_ADD_INTERVAL;

		// 加算スコアの合計用一時変数
		int add = 0;

		// 一度に加算するスコアの最大数分ループして加算スコアを取得
		for (int loop = 0; loop < MAX_ADD_SCORE_NUM; loop++) {

			// スコア管理クラスの加算スコア適用関数を呼び出し、加算スコアを取得
			int a = Score::GetIns().AddBaseScoreApplyAndGet();

			// 加算スコアが0ならループを抜ける
			if (a == 0) { break; }

			// 加算スコアを合計用一時変数に加える
			add += a;
		}
		// 加算スコアがあるなら加算スコア表示用配列に追加
		if (add > 0) {
			scoreAddInterval = 0;
			if (!addScore.empty()) { addScore.back().pos.y -= 15.0f; }
			addScore.emplace_back(AddScoreInfo(add, SCORE_POS, Font::GetIns().GetFont(FontKinds::GOKUSYOU_32)));
		}
	}

	// 加算スコアの更新
	for (auto it = addScore.begin(); it != addScore.end(); ) {
		if (!it->AliveUpdate()) { it = addScore.erase(it); }
		else { ++it; }
	}

	// 表示用スコアを現在のスコアへ近づける（徐々に）
	int smoothScoreSub = (int)((Score::GetIns().BaseScore() - displayScore) * DISPLAY_SCORE_SMOOTH);
	displayScore += (smoothScoreSub > 0) ? smoothScoreSub : Score::GetIns().BaseScore() - displayScore;

	if (Score::GetIns().Combo() > 0) {
		comboAnimeIndex += comboAnimeSigned;
		if (comboAnimeIndex <= 0 || comboAnimeIndex >= 12 - 1) { comboAnimeSigned *= -1; }
	}
}


void ScoreUI::UiDraw(void)
{
	// スコア表示
	DrawFormatStringToHandle((int)SCORE_POS.x, (int)SCORE_POS.y, GetScoreDispColor(displayScore), Font::GetIns().GetFont(FontKinds::GOKUSYOU_60), "SCORE:%08d", displayScore);

	// 加算スコアを表示
	for (AddScoreInfo& add : addScore) { add.Draw(); }

	// コンボ数表示
	int comboCount = Score::GetIns().Combo();
	if (comboCount > 0) {
		Vector2I comboPos = Vector2I((int)SCORE_POS.x + 65, (int)SCORE_POS.y + Font::GetIns().FONT_SIZE_TABLE[(int)FontKinds::GOKUSYOU_60] + 65);
		// コンボ背景画像の表示
		DrawRotaGraph(comboPos.x, comboPos.y, 0.5, 0, comboBackImage[(int)comboAnimeIndex], true);

		// コンボ数の表示
		DrawFormatStringToHandle(
			comboPos.x - 33, comboPos.y - 36,
			0xffffff, Font::GetIns().GetFont(FontKinds::GOKUSYOU_60),
			"%02d", comboCount
		);
	}
}

void ScoreUI::Release(void)
{
	for (int& id : comboBackImage) { DeleteGraph(id); }

}

void ScoreUI::AllAddScoreApply(void)
{
	for (int i = 0; i < 1000; i++) { if (Score::GetIns().AddBaseScoreApplyAndGet() == 0) { break; } }
}
