#include "RankInScene.h"

#include"../../../Application/Application.h"

#include"../../../Manager/Input/KeyManager.h"
#include"../../../Manager/Font/FontManager.h"
#include"../../../Manager/Score/Ranking.h"
#include"../../SceneManager/SceneManager.h"

RankInScene::RankInScene(const int& mainScreen) :
	SceneBase(),

	mainScreen(mainScreen),

	tempScreen(-1),

	backImage(-1),
	scale()
{
}

void RankInScene::Load(void)
{
	tempScreen = MakeScreen(App::SCREEN_SIZE_X, App::SCREEN_SIZE_Y);

	Utility::LoadImg(backImage, "Data/Image/Clear/RankInBack.png");
}

void RankInScene::Init(void)
{
	if (!KEY::GetIns().IsInputText()) { KEY::GetIns().IsInputTextSwitch(); }
	KEY::GetIns().InputText().Reset();

	scale = START_SCALE;
}

void RankInScene::Update(void)
{
	if (KEY::GetIns().GetInfo(KEY_TYPE::ENTER).down) {
		const std::wstring& text = KEY::GetIns().InputText().InputText();
		if (0 < text.size() && text.size() <= 7) {
			SceneManager::GetIns().PopScene();
			return;
		}
		else {
			// 文字数が適切ではない

		}
	}

	if (scale > 1.0f) { scale -= 0.1f; }
	else if (scale < 1.0f) { scale = 1.0f; }
}

void RankInScene::Draw(void)
{
	int x = App::SCREEN_SIZE_X, y = App::SCREEN_SIZE_Y;

	// ランクイン情報の描画をひとまとめにした関数
	auto RankInInfoDraw = [&](void)->void {
		DrawRotaGraph(x / 2, y / 2, 1, 0, backImage, true);

		DrawStringToHandle(
			x / 4, y / 3,
			(std::to_string(Ranking::GetIns().GetLastAddScoreRankIndex() + 1) + "位にランクインしました！").c_str(),
			0xffffff, Font::GetIns().GetFont(FontKinds::DEFAULT_64)
		);

		DrawStringToHandle(
			x / 4, y / 2,
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
	// 画像を解放
	DeleteGraph(backImage);
	DeleteGraph(tempScreen);
	DeleteGraph(mainScreen);

	Ranking::GetIns().SetLastAddScoreName(WStringToString(KEY::GetIns().InputText().InputText()));
	KEY::GetIns().InputText().Reset();
}