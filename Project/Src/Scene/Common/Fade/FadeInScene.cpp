#include"FadeInScene.h"

#include"FadeOutScene.h"

#include"../../../Application/Application.h"

#include"../../SceneManager/SceneManager.h"

FadeInScene::FadeInScene(std::shared_ptr<SceneBase> scene, unsigned short FADE_TIME, unsigned int FADE_COLOR, bool sceneTransitionKinds) :
	scene(scene),
	FADE_TIME(FADE_TIME),
	fadeCounter(FADE_TIME),
	FADE_COLOR(FADE_COLOR),
	sceneTransitionKinds(sceneTransitionKinds)
{
}

void FadeInScene::Update(void)
{
	if (fadeCounter > 0) { fadeCounter--; }
	else {
		// メンバ変数は破棄されるため一時変数に情報を保存
		unsigned short fadeTimeWork = FADE_TIME;
		unsigned int fadeColor = FADE_COLOR;

		if (sceneTransitionKinds) {
			// ChangeScene()

			// 自身(FadeInScene)と遷移前の最後尾のシーンを破棄する
			// その上に目的のシーンを重ねて末尾が入れ替わった状態にする
			SceneManager::GetIns().AnyPopAndChangeScene(1, scene);
		}
		else {
			// JumpScene()
			
			// 現在抱えているシーン全てを破棄して移動先のシーンに遷移する
			SceneManager::GetIns().JumpScene(scene);
		}

		// 目的のシーンを抱えた状態でフェードアウト演出を行うシーンを上に重ねる
		SceneManager::GetIns().PushScene(std::make_shared<FadeOutScene>(fadeTimeWork, fadeColor));

		return;
	}
}

void FadeInScene::Draw(void) 
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, FadeAlpha());
	DrawBox(0, 0, App::SCREEN_SIZE_X, App::SCREEN_SIZE_Y, FADE_COLOR, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}