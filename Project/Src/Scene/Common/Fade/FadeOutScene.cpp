#include "FadeOutScene.h"

#include<DxLib.h>

#include"../../../Application/Application.h"

#include"../../SceneManager/SceneManager.h"

FadeOutScene::FadeOutScene(unsigned short FADE_TIME, unsigned int FADE_COLOR) :
	FADE_TIME(FADE_TIME),
	fadeCounter(FADE_TIME),
	FADE_COLOR(FADE_COLOR)
{
}

void FadeOutScene::Update(void)
{
	if (fadeCounter > 0) { fadeCounter--; }
	else {
		SceneManager::GetIns().PopScene();
		return;
	}
}

void FadeOutScene::Draw(void)
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, FadeAlpha());
	DrawBox(0, 0, App::SCREEN_SIZE_X, App::SCREEN_SIZE_Y, FADE_COLOR, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}