#pragma once

#include<memory>
#include<list>

class SceneBase;

class SceneManager
{
private:
	// コンストラクタ / デストラクタ
	SceneManager(void) :
		scenes()
	{
	}
	~SceneManager(void) = default;

	// インスタンス
	static SceneManager* ins;

	// コピー/ムーブ 操作を禁止
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;
	SceneManager(SceneManager&&) = delete;
	SceneManager& operator=(SceneManager&&) = delete;
public:

#pragma region シングルトン定義
	// 生成 / 初期化処理
	static void CreateIns(void) { if (ins == nullptr) { ins = new SceneManager(); ins->Init(); } };
	// 取得
	static SceneManager& GetIns(void) { return *ins; };
	// 終了処理 / 削除
	static void DeleteIns(void) { if (ins != nullptr) { ins->Release(); delete ins; ins = nullptr; } }
#pragma endregion

public:

	// イベントシーンを除くメインのシーンの列挙型定義
	enum class SCENE_ID
	{
		TITLE,
		RANKING,
		GAME,
		CLEAR,
		OVER,
	};

	// 更新
	void Update(void);
	// 描画
	void Draw(void);


	// 状態遷移（遷移先のシーンのincludeが必要）
	void ChangeScene(std::shared_ptr<SceneBase>scene);
	// 状態遷移（遷移先のシーンのincludeが必要ない。ただしメインのシーンに限る）
	void ChangeScene(SCENE_ID scene);

	// 状態遷移（フェードあり）（遷移先のシーンのincludeが必要）
	void ChangeSceneFade(std::shared_ptr<SceneBase>scene, unsigned short FADE_TIME = 30, unsigned int FADE_COLOR = 0x000000);
	// 状態遷移（フェードあり）（遷移先のシーンのincludeが必要ない。ただしメインのシーンに限る）
	void ChangeSceneFade(SCENE_ID scene, unsigned short FADE_TIME = 30, unsigned int FADE_COLOR = 0x000000);

	// シーンを新しく積む（遷移先のシーンのincludeが必要）
	void PushScene(std::shared_ptr<SceneBase>scene);
	// シーンを新しく積む（遷移先のシーンのincludeが必要ない。ただしメインのシーンに限る）
	void PushScene(SCENE_ID scene);

	// 最後に追加したシーンを削除する
	void PopScene(void);

	// 強制的に特定のシーンに飛ぶ。リセットをかけ特定のシーンのみにする（遷移先のシーンのincludeが必要）
	void JumpScene(std::shared_ptr<SceneBase>scene);
	// 強制的に特定のシーンに飛ぶ。リセットをかけ特定のシーンのみにする（遷移先のシーンのincludeが必要ない。ただしメインのシーンに限る）
	void JumpScene(SCENE_ID scene);

	// 強制的に特定のシーンに飛ぶ。リセットをかけ特定のシーンのみにする（フェードあり）（遷移先のシーンのincludeが必要）
	void JumpSceneFade(std::shared_ptr<SceneBase>scene, unsigned short FADE_TIME = 30, unsigned int FADE_COLOR = 0x000000);
	// 強制的に特定のシーンに飛ぶ。リセットをかけ特定のシーンのみにする（フェードあり）（遷移先のシーンのincludeが必要ない。ただしメインのシーンに限る）
	void JumpSceneFade(SCENE_ID scene, unsigned short FADE_TIME = 30, unsigned int FADE_COLOR = 0x000000);

	// 指定した数の分シーンを吐き出して指定のシーンを上塗りする
	void AnyPopAndChangeScene(char popNum, std::shared_ptr<SceneBase>scene);

private:

	// 初期化処理
	void Init(void);
	// 終了処理
	void Release(void);

	//Drawの関係上Backを最新のシーンとする
	//基本的には要素は一つだけだがポーズシーンなどが積み重なる形
	std::list<std::shared_ptr<SceneBase>>scenes;

	// ３Ｄの初期設定
	void Init3D(void);
};

using SCENE_ID = SceneManager::SCENE_ID;