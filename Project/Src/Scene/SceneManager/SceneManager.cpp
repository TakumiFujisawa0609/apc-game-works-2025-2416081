#include "SceneManager.h"

#include <DxLib.h>

#include "../../Manager/Loading/Loading.h"

#include"../Title/TitleScene.h"
#include"../Game/GameScene.h"
#include"../Clear/ClearScene.h"
#include"../Over/OverScene.h"

SceneManager* SceneManager::ins_ = nullptr;

// コンストラクタ
SceneManager::SceneManager(void):
	sceneId_(SCENE_ID::NONE)
{
}

// デストラクタ
SceneManager::~SceneManager(void)
{
}

// 初期化
void SceneManager::Init(void)
{
	// ロード画面生成
	Loading::GetInstance()->CreateInstance();
	Loading::GetInstance()->Init();
	Loading::GetInstance()->Load();

	Init3D();

	// 最初はタイトル画面から
	ChangeScene(SCENE_ID::TITLE);
}


// 更新
void SceneManager::Update(void)
{
	// シーンがなければ終了
	if (scenes_.empty()) { return; }

	// ロード中
	if (Loading::GetInstance()->IsLoading())
	{
		// ロード更新
		Loading::GetInstance()->Update();

		// ロードの更新が終了していたら
		if (Loading::GetInstance()->IsLoading() == false)
		{
			// ロード後の初期化
			scenes_.back()->Init();
		}
	}		
	// 通常の更新処理
	else
	{
		// 現在のシーンの更新
		scenes_.back()->Update();
	}
}

// 描画
void SceneManager::Draw(void)
{
	// ロード中ならロード画面を描画
	if (Loading::GetInstance()->IsLoading())
	{
		// ロードの描画
		Loading::GetInstance()->Draw();
	}
	// 通常の更新
	else
	{
		// 積まれているもの全てを描画する
		for (auto& scene : scenes_) 
		{
			scene->Draw();
		}
	}
}

// 解放
void SceneManager::Release(void)
{
	//全てのシーンの解放・削除
	for (auto& scene : scenes_)
	{
		scene->Release();
	}
	scenes_.clear();

	// ロード画面の削除
	Loading::GetInstance()->Release();
	Loading::GetInstance()->DeleteInstance();
}

// 状態遷移関数
void SceneManager::ChangeScene(std::shared_ptr<SceneBase>scene)
{	
	// シーンが空か？
	if (scenes_.empty()) 
	{
		//空なので新しく入れる
		scenes_.push_back(scene);
	}
	else 
	{
		//末尾のものを新しい物に入れ替える
		scenes_.back()->Release();
		scenes_.back() = scene;
	}

	// 読み込み(非同期)
	Loading::GetInstance()->StartAsyncLoad();
	scenes_.back()->Load();
	Loading::GetInstance()->EndAsyncLoad();
}

void SceneManager::ChangeScene(SCENE_ID scene)
{
	switch (scene)
	{
	case SCENE_ID::TITLE:
		ChangeScene(std::make_shared<TitleScene>());
		break;
	case SCENE_ID::GAME:
		ChangeScene(std::make_shared<GameScene>());
		break;
	case SCENE_ID::CLEAR:
		ChangeScene(std::make_shared<ClearScene>());
		break;
	case SCENE_ID::OVER:
		ChangeScene(std::make_shared<OverScene>());
		break;
	default:
		break;
	}
}

void SceneManager::PushScene(std::shared_ptr<SceneBase> scene)
{
	//新しく積むのでもともと入っている奴はまだ削除されない
	scenes_.push_back(scene);
	scenes_.back()->Load();
	scenes_.back()->Init();
}

void SceneManager::PushScene(SCENE_ID scene)
{
	switch (scene)
	{
	case SCENE_ID::TITLE:
		PushScene(std::make_shared<TitleScene>());
		break;
	case SCENE_ID::GAME:
		PushScene(std::make_shared<GameScene>());
		break;
	case SCENE_ID::CLEAR:
		PushScene(std::make_shared<ClearScene>());
		break;
	case SCENE_ID::OVER:
		PushScene(std::make_shared<OverScene>());
		break;
	default:
		break;
	}
}

void SceneManager::PopScene(void)
{
	//積んであるものを消して、もともとあったものを末尾にする
	if (scenes_.size() > 1) 
	{
		scenes_.back()->Release();
		scenes_.pop_back();
	}
}

void SceneManager::JumpScene(std::shared_ptr<SceneBase> scene)
{
	// 全て解放
	for (auto& s : scenes_) { s->Release(); }
	scenes_.clear();

	// 新しく積む
	ChangeScene(scene);
}

void SceneManager::JumpScene(SCENE_ID scene)
{
	switch (scene)
	{
	case SCENE_ID::TITLE:
		JumpScene(std::make_shared<TitleScene>());
		break;
	case SCENE_ID::GAME:
		JumpScene(std::make_shared<GameScene>());
		break;
	case SCENE_ID::CLEAR:
		JumpScene(std::make_shared<ClearScene>());
		break;
	case SCENE_ID::OVER:
		JumpScene(std::make_shared<OverScene>());
		break;
	default:
		break;
	}
}

void SceneManager::Init3D(void)
{
	// 背景色設定
	SetBackgroundColor(0, 139, 139);

	// Zバッファを有効にする
	SetUseZBuffer3D(true);
	// Zバッファへの書き込みを有効にする
	SetWriteZBuffer3D(true);

	// バックカリングを有効にする
	SetUseBackCulling(false);

	// フォグ設定
	SetFogEnable(true);
	// フォグの色
	SetFogColor(100, 100, 100);
	// フォグを発生させる奥行きの最小、最大距離
	SetFogStartEnd(1000.0f, 15000.0f);

	SetTextureAddressMode(DX_TEXADDRESS_WRAP);

	// ---- ライティング全体設定 ----
	SetUseLighting(TRUE);
	ChangeLightTypeDir({ 0.00f, -1.00f, 0.00f });
	SetLightDirection({ 0.00f, -1.00f, 0.00f });
	SetUseSpecular(FALSE);

	// ---- マテリアル（頂点カラー + 白拡散 + そこそこAmbient）----
	MATERIALPARAM m{};
	m.Diffuse = GetColorF(1.0f, 1.0f, 1.0f, 1.0f);
	m.Ambient = GetColorF(0.6f, 0.6f, 0.6f, 1.0f);  // 真っ黒回避
	m.Specular = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
	m.Emissive = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
	m.Power = 0.0f;
	SetMaterialParam(m);

	// 影響しうる「真っ黒系」設定を念のためオフ
	SetGlobalAmbientLight(GetColorF(0, 0, 0, 0));  // 使っていれば好みで
	SetDrawBright(255, 255, 255);               // 明度補正が落ちていないか
}
