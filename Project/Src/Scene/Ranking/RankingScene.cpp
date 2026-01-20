#include "RankingScene.h"

#include"../../Utility/Utility.h"

#include"../../Application/Application.h"

#include"../SceneManager/SceneManager.h"
#include"../../Manager/Input/KeyManager.h"

RankingScene::RankingScene() :
	rankingData(Ranking::GetIns().GetRankingList()),

	scrollHeight(),

	font(),

	backImg(-1),
	frameImg(-1)
{
}

void RankingScene::Load(void)
{
#pragma region フォントを作成
	font = CreateFontToHandle("x10y12pxDonguriDuel", FONT_SIZE, -1, DX_FONTTYPE_EDGE);
#pragma endregion

	backImg = Utility::LoadImg("Data/Image/Ranking/RankingImageBack.png");
	frameImg = Utility::LoadImg("Data/Image/Ranking/RankingImageFrame.png");
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
	scrollHeight = Utility::Clamp(scrollHeight, -((int)(rankingData.size() - 7) * (int)FONT_SIZE), 0);
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
	DrawGraph(0, 0, backImg, true);
	// ランキング表示
	for (size_t i = 0; i < rankingData.size(); i++) {
		Vector2I drawPos = Vector2I(250, 230 + (i * FONT_SIZE) + scrollHeight);
		int dispScore = rankingData[i].score;

		DrawFormatStringToHandle(drawPos.x, drawPos.y, 0xffffff, font, "%02d.", i + 1);
		drawPos.x += 100;
		DrawStringToHandle(drawPos.x, drawPos.y, rankingData[i].name.c_str(), 0xffffff, font);
		drawPos.x += 400;
		DrawFormatStringToHandle(drawPos.x, drawPos.y, 0xffffff, font, (dispScore != -1) ? "SCORE:%08d" : "SCORE:--------", dispScore);
	}
	DrawGraph(0, 0, frameImg, true);
}

void RankingScene::Release(void)
{
	DeleteGraph(backImg);
	DeleteGraph(frameImg);

	DeleteFontToHandle(font);
}
