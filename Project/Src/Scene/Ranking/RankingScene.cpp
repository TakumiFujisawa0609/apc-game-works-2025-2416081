#include "RankingScene.h"

#include"../../Application/Application.h"

#include"../SceneManager/SceneManager.h"
#include"../../Manager/Input/KeyManager.h"

RankingScene::RankingScene() :
	rankingData(Ranking::GetIns().GetRankingList()),

	scrollHeight()
{
}

void RankingScene::Load(void)
{
#pragma region フォントを作成
	font = CreateFontToHandle("x10y12pxDonguriDuel", FONT_SIZE, -1, DX_FONTTYPE_EDGE);
#pragma endregion
}

void RankingScene::Init(void)
{
	KEY::GetIns().SetMouceFixed(false);

}

void RankingScene::Update(void)
{
#pragma region スクロール処理
	if (KEY::GetIns().GetInfo(KEY_TYPE::UP).now) { scrollHeight += 10; }
	if (KEY::GetIns().GetInfo(KEY_TYPE::DOWN).now) { scrollHeight -= 10; }
#pragma endregion

	if (KEY::GetIns().GetInfo(KEY_TYPE::ENTER).down) {
		SceneManager::GetIns().ChangeScene(SCENE_ID::GAME);
		return;
	}
	if (KEY::GetIns().GetInfo(KEY_TYPE::PAUSE).down) {
		SceneManager::GetIns().ChangeScene(SCENE_ID::TITLE);
		return;
	}
}

void RankingScene::Draw(void)
{
	// ランキング表示
	for (size_t i = 0; i < rankingData.size(); i++) {
		Vector2I drawPos = Vector2I(100, 100 + (i * FONT_SIZE) + scrollHeight);
		std::string dispContect = rankingData[i].name + "  SCORE:";
		int dispScore = rankingData[i].score;

		//DrawFormatStringToHandle(drawPos.x, drawPos.y, 0xffffff, font, ("%02d.  " + dispContect + ((dispScore != -1) ? "%08d" : "--------")).c_str(), i + 1, dispScore);
		DrawStringToHandle(drawPos.x, drawPos.y, (std::to_string(i + 1) + ".").c_str(), 0xffffff, font);
		drawPos.x += 100;
		DrawStringToHandle(drawPos.x, drawPos.y, rankingData[i].name.c_str(), 0xffffff, font);
		drawPos.x += 500;
		DrawFormatStringToHandle(drawPos.x, drawPos.y, 0xffffff, font, (dispScore != -1) ? "SCORE:%08d" : "SCORE:--------", dispScore);
	}
}

void RankingScene::Release(void)
{
	DeleteFontToHandle(font);
}
