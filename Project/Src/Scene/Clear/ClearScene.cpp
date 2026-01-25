#include "ClearScene.h"

#include"../../Utility/Utility.h"

#include"../../Application/Application.h"

#include"../SceneManager/SceneManager.h"
#include"../../Manager/Input/KeyManager.h"
#include"../../Manager/Camera/Camera.h"
#include"../../Manager/Score/Score.h"
#include"../../Manager/Score/Ranking.h"

#include"../../Object/SkyDome/SkyDome.h"
#include"../../Object/Stage/Block/BlockManager.h"

ClearScene::ClearScene() :
	img_(-1),
	objects()
{
}

ClearScene::~ClearScene()
{
}

void ClearScene::Load(void)
{
	KEY::GetIns().SetMouceFixed(false);
	Utility::LoadImg(img_, "Data/Image/Clear/GameClear.png");

	// 初期化も含めたオブジェクト生成のラムダ関数
	auto ObjAdd = [&](ActorBase* newClass)->void {
		// 配列の末尾に追加
		objects.emplace_back(newClass);
		// 共通の読み込み処理
		objects.back()->Load();
		};

	ObjAdd(new SkyDome());
	
}

void ClearScene::Init(void)
{
	Camera::GetIns().ChangeModeFixedPoint(Vector3(), Vector3());

	Ranking::GetIns().AddScore(Score::GetIns().TotalScore());

	for (ActorBase*& obj : objects) { obj->Init(); }
}

void ClearScene::Update(void)
{
	if (KEY::GetIns().GetInfo(KEY_TYPE::ENTER).down ||
		KEY::GetIns().GetInfo(KEY_TYPE::PAUSE).down) {
		SceneManager::GetIns().ChangeScene(SCENE_ID::TITLE);
		return;
	}

	for (ActorBase*& obj : objects) { obj->Update(); }
}

void ClearScene::Draw(void)
{
	int x = Application::SCREEN_SIZE_X;
	int y = Application::SCREEN_SIZE_Y;

	for (ActorBase*& obj : objects) { obj->Draw(); }

	DrawExtendGraph(0, 0, x, y, img_, true);

	SetFontSize(64);

	DrawFormatStringF(x / 2.0f - 300.0f, y / 2.0f + 325.0f, 0xffffff, "SCORE : %08d", Score::GetIns().TotalScore());

	SetFontSize(16);
}

void ClearScene::Release(void)
{
	for (ActorBase*& obj : objects) {
		if (!obj) { continue; }
		obj->Release();
		delete obj;
		obj = nullptr;
	}
	DeleteGraph(img_);
}