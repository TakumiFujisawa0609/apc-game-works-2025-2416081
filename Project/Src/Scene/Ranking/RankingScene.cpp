#include "RankingScene.h"

#include"../../Utility/Utility.h"

#include"../../Application/Application.h"

#include"../../Manager/Input/KeyManager.h"
#include"../../Manager/Sound/SoundManager.h"
#include"../../Manager/Font/FontManager.h"

#include"../SceneManager/SceneManager.h"

RankingScene::RankingScene() :
	rankingData(Ranking::GetIns().GetRankingList()),

	scrollHeight(),

	backImg(-1),
	frameImg(-1)
{
}

void RankingScene::Load(void)
{
	Snd::GetIns().ChangeScene("Ranking");

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
	scrollHeight =
		Utility::Clamp(
			scrollHeight,
			-((int)(rankingData.size() - 7) * (int)Font::GetIns().FONT_SIZE_TABLE[(int)FontKinds::GOKUSYOU_64]),
			0
		);
#pragma endregion

	if (KEY::GetIns().GetInfo(KEY_TYPE::ENTER).down) {
		SceneManager::GetIns().ChangeScene(SCENE_ID::GAME);
		Snd::GetIns().Play("SystemButton");
		return;
	}
	if (KEY::GetIns().GetInfo(KEY_TYPE::PAUSE).down) {
		SceneManager::GetIns().ChangeScene(SCENE_ID::TITLE);
		Snd::GetIns().Play("SystemButton");
		return;
	}
}

void RankingScene::Draw(void)
{
	DrawGraph(0, 0, backImg, true);
	// ランキング表示
	for (size_t i = 0; i < rankingData.size(); i++) {
		Vector2I drawPos = Vector2I(250, 230 + ((int)i * (int)Font::GetIns().FONT_SIZE_TABLE[(int)FontKinds::GOKUSYOU_64]) + scrollHeight);
		int dispScore = rankingData[i].score;

		DrawFormatStringToHandle(drawPos.x, drawPos.y, 0xffffff, Font::GetIns().GetFont(FontKinds::GOKUSYOU_64), "%02d.", i + 1);
		drawPos.x += 100;
		DrawStringToHandle(drawPos.x, drawPos.y, rankingData[i].name.c_str(), 0xffffff, Font::GetIns().GetFont(FontKinds::GOKUSYOU_64));
		drawPos.x += 400;
		DrawFormatStringToHandle(drawPos.x, drawPos.y, 0xffffff, Font::GetIns().GetFont(FontKinds::GOKUSYOU_64), (dispScore != -1) ? "SCORE:%08d" : "SCORE:--------", dispScore);
	}
	DrawGraph(0, 0, frameImg, true);
}

void RankingScene::Release(void)
{
	DeleteGraph(backImg);
	DeleteGraph(frameImg);
}
