#pragma once

// クラスの前方宣言
class FPS;

class Application
{
public:
	// スクリーンサイズ
	static constexpr int SCREEN_SIZE_X = 1200;	// スクリーン横幅;
	static constexpr int SCREEN_SIZE_Y = 750;	// スクリーン縦幅;

public:
	// シングルトン（生成・取得・削除）
	static void CreateIns(void) { if (ins_ == nullptr) { ins_ = new Application(); ins_->Init(); } }
	static Application& GetIns(void) { return *ins_; }
	static void DeleteIns(void) { if (ins_ != nullptr) delete ins_; ins_ = nullptr; }

public:

	void Init(void);	// 初期化
	void Run(void);		// ゲームループの開始
	void Release(void);	// 解放

	// 初期化成功／失敗の判定
	bool IsInitFail(void) const { return isInitFail_; };

	// 解放成功／失敗の判定
	bool IsReleaseFail(void) const { return isReleaseFail_; };

	// ゲーム終了
	void GameEnd(void) { gameEnd_ = true; }

private:
	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	Application(void);

	// デストラクタも同様
	~Application(void);

	// コピー・ムーブ操作を禁止
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
	Application(Application&&) = delete;
	Application& operator=(Application&&) = delete;

	// 下記をコンパイルエラーさせるため 上記を追加
	// Application copy = *Application::GetInstance();
	// Application copied(*Application::GetInstance());
	// Application moved = std::move(*Application::GetInstance());
private:

	// 静的インスタンス
	static Application* ins_;

	// 初期化失敗
	bool isInitFail_;

	// 解放失敗
	bool isReleaseFail_;

	// ゲーム終了
	bool gameEnd_;

	// FPS
	FPS* fps_;
};

// Applicationの省略形
using App = Application; 