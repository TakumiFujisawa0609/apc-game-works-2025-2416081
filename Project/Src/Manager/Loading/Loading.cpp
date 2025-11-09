#include "Loading.h"

#include <DxLib.h>

#include"../../Application/Application.h"
#include"../../Utility/Utility.h"

Loading* Loading::instance_ = nullptr;

// コンストラクタ
Loading::Loading()
	: isLoading_(false)
	, loadTimer_(0)
	, animInterval_()
	, animCounter_()
{}

// デストラクタ
Loading::~Loading()
{}

// 初期化
void Loading::Init(void)
{
	loadTimer_ = 0;
	isLoading_ = false;

	animCounter_ = 0;
	animInterval_ = 0;
}

// 読み込み
void Loading::Load(void)
{
	//Utility::LoadArrayImg("Data/Image/Player/まぼ.png", 7, 7, 1, 42, 66, handle_);
}

// 更新
void Loading::Update(void)
{
	loadTimer_++;

	// 読込中のものがなくなったら or 最低ロード時間経過
	if (GetASyncLoadNum() == 0 && loadTimer_ >= MIN_LOAD_TIME)
	{
		// ロード終了
		Init();
	}
	// 読み込み中
	else
	{
		// ロード画面を動作させるならここに記述
		if (++animInterval_ >= 10) {
			animInterval_ = 0;
			if (++animCounter_ >= 4) {
				animCounter_ = 0;
			}
		}
	}
}

// 描画
void Loading::Draw(void)
{
	using app = Application;
	int xx = app::SCREEN_SIZE_X;
	int yy = app::SCREEN_SIZE_Y;
	int x = xx / 2;
	int y = yy / 2;

	DrawBox(0, 0, xx, yy, 0x000000, true);
	std::string st = "Loading";
	for (int i = 0; i < animCounter_; i++) { st += "."; }
	int fontSize = 45;
	SetFontSize(fontSize);
	DrawString(xx - fontSize * 6, yy - fontSize, st.c_str(), 0xffffff);
	SetFontSize(16);
}

// 解放
void Loading::Release(void)
{
}

// 非同期読み込みに切り替える
void Loading::StartAsyncLoad(void)
{
	isLoading_ = true;
	// 非同期読み込み開始
	SetUseASyncLoadFlag(true);
}

// 同期読み込みに切り替える
void Loading::EndAsyncLoad(void)
{
	// 非同期読み込み終了
	SetUseASyncLoadFlag(false);
}
