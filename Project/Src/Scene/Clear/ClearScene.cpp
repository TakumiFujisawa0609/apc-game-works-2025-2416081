#include "ClearScene.h"

#include"../../Utility/Utility.h"

#include"../../Application/Application.h"

#include"../SceneManager/SceneManager.h"
#include"../../Manager/Input/KeyManager.h"
#include"../../Manager/Camera/Camera.h"
#include"../../Manager/Score/Score.h"
#include"../../Manager/Score/Ranking.h"

#include"Rankin/RankInScene.h"

#include"../../Object/Stage/Block/BlockManager.h"

#include"../../Object/SkyDome/SkyDome.h"
#include"../../Object/UI/Score/ResultScore.h"

ClearScene::ClearScene(std::vector<VoxelBase::MeshBatch> stageBatches, const char* stageTexturePath) :
	mainScreen(-1),

	img_(-1),

	objects(),

	rankinJudge(false),

	stageBatches(stageBatches),
	stageTexture(stageTexturePath != nullptr ? LoadGraph(stageTexturePath) : -1)
{
}

ClearScene::~ClearScene()
{
}

void ClearScene::Load(void)
{
	KEY::GetIns().SetMouceFixed(false);
	Utility::LoadImg(img_, "Data/Image/Clear/GameClear.png");

	mainScreen = MakeScreen(App::SCREEN_SIZE_X, App::SCREEN_SIZE_Y, true);

	// 初期化も含めたオブジェクト生成のラムダ関数
	auto ObjAdd = [&](ActorBase* newClass)->void {
		// 配列の末尾に追加
		objects.emplace_back(newClass);
		// 共通の読み込み処理
		objects.back()->Load();
		};

	ObjAdd(new SkyDome());
	ObjAdd(new ResultScore());
}

void ClearScene::Init(void)
{
	Camera::GetIns().ChangeModeDisplay(Vector3::XZonly(1000.0f, 1000.0f), Vector3::YZonly(800.0f, -2000.0f), Utility::Deg2RadF(0.1f));

	Ranking::GetIns().AddScore(Score::GetIns().TotalScore());

	for (ActorBase*& obj : objects) { obj->Init(); }

	rankinJudge = false;
}

void ClearScene::Update(void)
{
	if (KEY::GetIns().GetInfo(KEY_TYPE::ENTER).down ||
		KEY::GetIns().GetInfo(KEY_TYPE::PAUSE).down) {
		if (!rankinJudge) { ObjSerch<ResultScore>()->EasingSkip(); }
		else {
			SceneManager::GetIns().ChangeScene(SCENE_ID::TITLE);
			return;
		}
	}

	for (ActorBase*& obj : objects) { obj->Update(); }

	// スコア表示のイージングが終わったらランクインしたかの判定を行う
	if (!rankinJudge && ObjSerch<ResultScore>()->EasingEnd()) {

		// ランクイン判定
		if (Ranking::GetIns().GetLastAddScoreRankIndex() != -1) {
			// ランクインしたので名前入力用のシーンを積む
			SceneManager::GetIns().PushScene(std::make_shared<RankInScene>(mainScreen));
		}

		// 判定は1度限りにする
		rankinJudge = true;
	}
}

void ClearScene::Draw(void)
{
	int x = Application::SCREEN_SIZE_X;
	int y = Application::SCREEN_SIZE_Y;

	SetDrawScreen(mainScreen);
	ClearDrawScreen();

	// メッシュ描画
	for (auto& b : stageBatches) {
		if (b.i.empty()) { continue; }
		DrawPolygonIndexed3D(
			b.v.empty() ? b.v.data() : b.v.data(),
			(int)(b.v.empty() ? b.v.size() : b.v.size()),
			b.i.data(), (int)(b.i.size() / 3),
			(stageTexture != -1) ? stageTexture : DX_NONE_GRAPH, true
		);
	}

	for (ActorBase*& obj : objects) { obj->Draw(); }

	DrawExtendGraph(0, 0, x, y, img_, true);

	for (ActorBase*& obj : objects) { obj->UiDraw(); }

	SetDrawScreen(DX_SCREEN_BACK);
	DrawGraph(0, 0, mainScreen, true);
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
	if (stageTexture != -1) { DeleteGraph(stageTexture); }

	DeleteGraph(mainScreen);
}