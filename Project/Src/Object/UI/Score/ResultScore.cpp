#include"ResultScore.h"

#include"../../../Manager/Score/Score.h"

#include"../../../Manager/Font/FontManager.h"
#include"../../../Manager/Sound/SoundManager.h"

const Vector2I ResultScore::ADD_BONUS_SCORE_OFFSET_POS = Vector2I(450, 0);

ResultScore::ResultScore() :
	baseScore(),
	baseDisplayScore(),
	baseEasingEnd(false),

	bonusScore(),
	bonusDisplayScore(),
	bonusAddScore(),
	bonusEasingEnd(false),

	totalScore(),
	totalDisplayScore(),
	totalEasingEnd(false),

	addBonusScore()
{
}

void ResultScore::Init(void)
{
	baseScore = Score::GetIns().BaseScore();
	baseDisplayScore = 0;
	baseEasingEnd = false;

	bonusScore = 0;
	bonusDisplayScore = 0;
	bonusAddScore = 0;
	bonusEasingEnd = false;

	totalScore = Score::GetIns().TotalScore();
	totalDisplayScore = 0;
	totalEasingEnd = false;
}

void ResultScore::Update(void)
{
	bool soundPlay = false;
	auto addSound = [&](void)->void {
		Snd::GetIns().Play("ScoreAdd");
		soundPlay = true;
		};

	// 表示用スコアを現在のスコアへ近づける（徐々に）
	if (!baseEasingEnd) {
		int smoothScoreSub = (int)((baseScore - baseDisplayScore) * DISPLAY_SCORE_SMOOTH);
		if (smoothScoreSub > 0) { baseDisplayScore += smoothScoreSub; addSound(); }
		else { baseDisplayScore = baseScore; baseEasingEnd = true; }
	}
	else if (!bonusEasingEnd) {
		int smoothScoreSub = (int)((bonusScore - bonusDisplayScore) * DISPLAY_SCORE_SMOOTH);
		if (smoothScoreSub > 0) { bonusDisplayScore += smoothScoreSub; addSound(); }
		else {
			bonusAddScore = Score::GetIns().AddBonusScoreApplyAndGet();

			if (bonusAddScore > 0) {
				addBonusScore.emplace_back(AddBonusScoreInfo(bonusAddScore, BONUS_DISPLAY_SCORE_POS, Font::GetIns().GetFont(FontKinds::GOKUSYOU_64)));
				bonusScore = Score::GetIns().BonusScore();
			}
			else {
				bonusDisplayScore = bonusScore;
				bonusEasingEnd = true;
				totalScore = Score::GetIns().TotalScore();
			}
		}
	}
	else if (!totalEasingEnd) {
		int smoothScoreSub = (int)((totalScore - totalDisplayScore) * DISPLAY_SCORE_SMOOTH);
		if (smoothScoreSub > 0) { totalDisplayScore += smoothScoreSub; addSound(); }
		else { totalDisplayScore = totalScore; totalEasingEnd = true; }
	}

	// 加算スコアの更新
	for (auto it = addBonusScore.begin(); it != addBonusScore.end(); ) {
		if (!it->AliveUpdate()) { it = addBonusScore.erase(it); }
		else { ++it; }
	}

	if (!soundPlay) { Snd::GetIns().Stop("ScoreAdd"); }
}

void ResultScore::UiDraw(void)
{
	DrawFormatStringToHandle(BASE_DISPLAY_SCORE_POS.x, BASE_DISPLAY_SCORE_POS.y, 0xffffff, Font::GetIns().GetFont(FontKinds::GOKUSYOU_110), "%08d", baseDisplayScore);
	DrawFormatStringToHandle(BONUS_DISPLAY_SCORE_POS.x, BONUS_DISPLAY_SCORE_POS.y, 0xffffff, Font::GetIns().GetFont(FontKinds::GOKUSYOU_110), "%08d", bonusDisplayScore);
	for (AddBonusScoreInfo& add : addBonusScore) { add.Draw(); }
	DrawFormatStringToHandle(TOTAL_DISPLAY_SCORE_POS.x, TOTAL_DISPLAY_SCORE_POS.y, 0xffffff, Font::GetIns().GetFont(FontKinds::GOKUSYOU_110), "%08d", totalDisplayScore);
}

void ResultScore::EasingSkip(void)
{
	baseDisplayScore = baseScore;

	for (int i = 0; i < 1000; i++) { if (Score::GetIns().AddBonusScoreApplyAndGet() == 0) { break; } }
	bonusDisplayScore = bonusScore = Score::GetIns().BonusScore();

	totalDisplayScore = totalScore = Score::GetIns().TotalScore();

	// 全てのフラグを「true」に
	baseEasingEnd = bonusEasingEnd = totalEasingEnd = true;
}