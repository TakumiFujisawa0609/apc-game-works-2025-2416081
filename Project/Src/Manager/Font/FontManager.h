#pragma once

class FontManager
{
private:

	FontManager();
	~FontManager() = default;

	static FontManager* ins;

public:
#pragma region シングルトン定義
	// 生成/初期化処理
	static void CreateIns(void) { if (ins == nullptr) { ins = new FontManager(); ins->Init(); } }
	// 取得
	static FontManager& GetIns(void) { return *ins; }
	// 終了処理/削除
	static void DeleteIns(void) { if (ins != nullptr) { ins->Release(); delete ins; ins = nullptr; } }
#pragma endregion

	enum class FONT_KINDS_SIZE
	{
		DEFAULT_64,

		GOKUSYOU_32,
		GOKUSYOU_60,
		GOKUSYOU_110,

		MAX
	};

	// フォント取得
	const int& GetFont(FONT_KINDS_SIZE kinds)const { return font[(int)kinds]; }

	// サイズテーブル
	const unsigned char FONT_SIZE_TABLE[(int)FONT_KINDS_SIZE::MAX] =
	{
		64,		// DEFAULT_64
		32,		// GOKUSYOU_32
		60,		// GOKUSYOU_60
		110,	// GOKUSYOU_110
	};

private:
	// 初期化処理
	void Init(void);
	// 終了処理
	void Release(void);

	// フォント
	int font[(int)FONT_KINDS_SIZE::MAX];

	// フォント名テーブル
	const char* FONT_NAME_TABLE[(int)FONT_KINDS_SIZE::MAX] =
	{
		nullptr,				// DEFAULT_64
		"x10y12pxDonguriDuel",	// GOKUSYOU_32
		"x10y12pxDonguriDuel",	// GOKUSYOU_60
		"x10y12pxDonguriDuel",	// GOKUSYOU_110
	};

	const char* FONT_NAME_ALL[1] =
	{
		"x10y12pxDonguriDuel"
	};

};

using Font = FontManager;
using FontKinds = Font::FONT_KINDS_SIZE;