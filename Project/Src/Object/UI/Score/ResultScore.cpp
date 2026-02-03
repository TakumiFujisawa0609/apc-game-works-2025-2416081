#include"ResultScore.h"

#include"../../../Manager/Score/Score.h"

#include"../../../Manager/Font/FontManager.h"
#include"../../../Manager/Sound/SoundManager.h"

ResultScore::ResultScore() :
	baseScore(),
	baseDisplaScore(),
	baseEasingEnd(false),

	bonusScore(),
	bonusDisplayScore(),
	bonusAddScore(),
	bonusEasingEnd(false),

	totalScore(),
	totalDisplayScore(),
	totalEasingEnd(false)
{
}

void ResultScore::Load(void)
{
}

void ResultScore::Init(void)
{
	baseScore = Score::GetIns().BaseScore();
	baseDisplaScore = 0;
	baseEasingEnd = false;

	bonusScore = Score::GetIns().BonusScore();
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
		int smoothScoreSub = (int)((baseScore - baseDisplaScore) * DISPLAY_SCORE_SMOOTH);
		if (smoothScoreSub > 0) { baseDisplaScore += smoothScoreSub; addSound(); }
		else { baseDisplaScore = baseScore; baseEasingEnd = true; }
	}
	else if (!bonusEasingEnd) {
		int smoothScoreSub = (int)((bonusScore - bonusDisplayScore) * DISPLAY_SCORE_SMOOTH);
		if (smoothScoreSub > 0) { bonusDisplayScore += smoothScoreSub; addSound(); }
		else {
			bonusAddScore = Score::GetIns().AddBonusScoreApplyAndGet();

			if (bonusAddScore > 0) { bonusScore = Score::GetIns().BonusScore(); }
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

	if (!soundPlay) { Snd::GetIns().Stop("ScoreAdd"); }
}

void ResultScore::UiDraw(void)
{
	DrawFormatStringToHandle(700, 235, 0xffffff, Font::GetIns().GetFont(FontKinds::GOKUSYOU_110), "%08d", baseDisplaScore);
	DrawFormatStringToHandle(700, 377, 0xffffff, Font::GetIns().GetFont(FontKinds::GOKUSYOU_110), "%08d", bonusDisplayScore);
	DrawFormatStringToHandle(700, 575, 0xffffff, Font::GetIns().GetFont(FontKinds::GOKUSYOU_110), "%08d", totalDisplayScore);
}

void ResultScore::Release(void)
{
}
