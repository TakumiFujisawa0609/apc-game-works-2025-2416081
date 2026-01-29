#include "RankInScene.h"

#include"../../../Application/Application.h"

#include"../../../Manager/Input/KeyManager.h"
#include"../../../Manager/Sound/SoundManager.h"
#include"../../../Manager/Font/FontManager.h"
#include"../../../Manager/Score/Ranking.h"

#include"../../SceneManager/SceneManager.h"

RankInScene::RankInScene(const int& mainScreen) :
	SceneBase(),

	mainScreen(mainScreen),

	tempScreen(-1),

	backImage(-1),

	scale(),

	genericCounter()
{
}

void RankInScene::Load(void)
{
	tempScreen = MakeScreen(App::SCREEN_SIZE_X, App::SCREEN_SIZE_Y);

	Utility::LoadImg(backImage, "Data/Image/Clear/RankInBack.png");

	Smng::GetIns().Load(SOUND::RANK_IN);
}

void RankInScene::Init(void)
{
	if (!KEY::GetIns().IsInputText()) { KEY::GetIns().IsInputTextSwitch(); }
	KEY::GetIns().InputText().Reset();
	KEY::GetIns().InputText().SetInputTextMax(8);

	scale = START_SCALE;

	genericCounter = 0.0f;
}

void RankInScene::Update(void)
{
	if (KEY::GetIns().GetInfo(KEY_TYPE::ENTER).down) {
		const std::wstring& text = KEY::GetIns().InputText().InputText();
		if (0 < text.size() && text.size() <= 7) {
			SceneManager::GetIns().PopScene();
			Smng::GetIns().Play(SOUND::SE_SYSTEM_BUTTON, true, 150);
			return;
		}
		else {
			// 文字数が適切ではない

		}
	}

	if (scale > 1.0f) { scale -= 0.11f; }
	else if (scale < 1.0f) {
		Smng::GetIns().Play(SOUND::RANK_IN, true, 150);
		scale = 1.0f;
	}

	if (scale == 1.0f) {
		genericCounter += 0.05f;
		if (genericCounter > 100000.0f) { genericCounter = 0.0f; }
	}
}

void RankInScene::Draw(void)
{
	int x = App::SCREEN_SIZE_X, y = App::SCREEN_SIZE_Y;

	static int drawX = x / 5, drawY = y / 3;
	if (CheckHitKey(KEY_INPUT_UP)) { drawY -= 5; }
	if (CheckHitKey(KEY_INPUT_DOWN)) { drawY += 5; }
	if (CheckHitKey(KEY_INPUT_LEFT)) { drawX -= 5; }
	if (CheckHitKey(KEY_INPUT_RIGHT)) { drawX += 5; }

	// ランクイン情報の描画をひとまとめにした関数
	auto RankInInfoDraw = [&](void)->void {
		DrawRotaGraph(x / 2, y / 2, 1, 0, backImage, true);

		int alpha = (int)fabsf(sinf(genericCounter) * 255.0f);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

		DrawStringToHandle(
			RANK_POP_POS.x, RANK_POP_POS.y,
			(std::to_string(Ranking::GetIns().GetLastAddScoreRankIndex() + 1) + "位にランクインしました").c_str(),
			0xffff00, Font::GetIns().GetFont(FontKinds::MARUMINYA_80)
		);

		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		DrawStringToHandle(
			NAME_PROMPT_POS.x, NAME_PROMPT_POS.y,
			"ランキングにのせる名前を入力してね！",
			0xffffff, Font::GetIns().GetFont(FontKinds::MARUMINYA_50)
		);

		DrawStringToHandle(
			NAME_LENGTH_NOTE_POS.x, NAME_LENGTH_NOTE_POS.y,
			"（１～７文字）",
			0xffffff, Font::GetIns().GetFont(FontKinds::MARUMINYA_50)
		);

		DrawStringToHandle(
			NAME_DISPLAY_POS.x, NAME_DISPLAY_POS.y,
			(((int)(genericCounter * 20) / 30) % 2 == 0) ?
			WStringToString(KEY::GetIns().InputText().InputText() + L"_").c_str() :
			WStringToString(KEY::GetIns().InputText().InputText()).c_str(),
			0xffffff, Font::GetIns().GetFont(FontKinds::GOKUSYOU_110)
		);

		};

	if (scale > 1.0f) {
		// 演出中

		// 描画先を変更
		SetDrawScreen(tempScreen);
		ClearDrawScreen();

		// 情報を描画
		RankInInfoDraw();

		// 描画先を変更
		SetDrawScreen(DX_SCREEN_BACK);
		ClearDrawScreen();

		DrawRotaGraph(x / 2, y / 2, 1, 0, mainScreen, true);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(200 * RatioReverse(NowScaleRatio())));
		DrawRotaGraph(x / 2, y / 2, scale, 0, tempScreen, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); 
	}
	else {
		// 演出終了

		// クリアシーンの描画しなおし
		DrawGraph(0, 0, mainScreen, true);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
		DrawBox(0, 0, x, y, 0x000000, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		// 情報を描画
		RankInInfoDraw();
	}
}

void RankInScene::Release(void)
{
	Smng::GetIns().Delete(SOUND::RANK_IN);
	// 画像を解放
	DeleteGraph(backImage);
	DeleteGraph(tempScreen);
	DeleteGraph(mainScreen);

	Ranking::GetIns().SetLastAddScoreName(WStringToString(KEY::GetIns().InputText().InputText()));
	KEY::GetIns().InputText().Reset();
}