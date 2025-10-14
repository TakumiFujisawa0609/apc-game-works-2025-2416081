#pragma once

#include<memory>
#include<list>

class SceneBase;

class SceneManager
{
public:
	// シーン管理用
	enum class SCENE_ID
	{
		NONE = -1,

		TITLE,
		GAME,
		CLEAR,
		OVER,

		MAX
	};

public:
	// シングルトン（生成・取得・削除）
	static void CreateIns(void) { if (ins_ == nullptr) { ins_ = new SceneManager(); } };
	static SceneManager& GetIns(void) { return *ins_; };
	static void DeleteIns(void) { if (ins_ != nullptr) { delete ins_; ins_ = nullptr; } }

private:
	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	SceneManager(void);
	// デストラクタも同様
	~SceneManager(void);

	// コピー・ムーブ操作を禁止
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;
	SceneManager(SceneManager&&) = delete;
	SceneManager& operator=(SceneManager&&) = delete;

	// 下記をコンパイルエラーさせるため 上記を追加
	// SceneManager copy = *SceneManager::GetInstance();
	// SceneManager copied(*SceneManager::GetInstance());
	// SceneManager moved = std::move(*SceneManager::GetInstance());
public:

	void Init(void);	// 初期化
	void Update(void);	// 更新
	void Draw(void);	// 描画
	void Release(void);	// 解放

	// 状態遷移
	void ChangeScene(std::shared_ptr<SceneBase>scene);
	void ChangeScene(SCENE_ID scene);

	// シーンを新しく積む
	void PushScene(std::shared_ptr<SceneBase>scene);
	void PushScene(SCENE_ID scene);

	// 最後に追加したシーンを削除する。
	void PopScene(void);

	// 強制的に特定のシーンに飛ぶ。リセットをかけ特定のシーンのみにする。
	void JumpScene(std::shared_ptr<SceneBase>scene);
	void JumpScene(SCENE_ID scene);

	// シーンIDの取得
	SCENE_ID GetSceneID(void) const { return sceneId_; };

	const float GetDeltaTime(void)const { return (1.0f / 60); }

private:
	// 静的インスタンス
	static SceneManager* ins_;

	//Drawの関係上Backを最新のシーンとする
	//基本的には要素は一つだけだがポーズシーンなどが積み重なる形
	std::list<std::shared_ptr<SceneBase>>scenes_;

	// シーンID
	SCENE_ID sceneId_;

	// ３Ｄの初期設定
	void Init3D(void);
};

using SCENE_ID = SceneManager::SCENE_ID;