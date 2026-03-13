#pragma once

class FontManager
{
private:

	// コンストラクタ/デストラクタ
	FontManager();
	~FontManager() = default;

	// コピー・ムーブ操作を禁止
	FontManager(const FontManager&) = delete;
	FontManager& operator=(const FontManager&) = delete;
	FontManager(FontManager&&) = delete;
	FontManager& operator=(FontManager&&) = delete;

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
		DEFAULT_45,
		DEFAULT_64,

		GOKUSYOU_32,
		GOKUSYOU_60,
		GOKUSYOU_64,
		GOKUSYOU_80,
		GOKUSYOU_110,

		MARUMINYA_50,
		MARUMINYA_80,

		MAX
	};

	// フォント取得
	const int& GetFont(FONT_KINDS_SIZE kinds)const { return font[(int)kinds]; }

	// サイズテーブル
	const unsigned char FONT_SIZE_TABLE[(int)FONT_KINDS_SIZE::MAX] =
	{
		45,		// DEFAULT_45
		64,		// DEFAULT_64
		32,		// GOKUSYOU_32
		60,		// GOKUSYOU_60
		64,		// GOKUSYOU_64
		80,		// GOKUSYOU_80
		110,	// GOKUSYOU_110
		50,		// MARUMINYA_50
		80,		// MARUMINYA_80
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
		nullptr,				// DEFAULT_45
		nullptr,				// DEFAULT_64
		"x10y12pxDonguriDuel",	// GOKUSYOU_32
		"x10y12pxDonguriDuel",	// GOKUSYOU_60
		"x10y12pxDonguriDuel",	// GOKUSYOU_64
		"x10y12pxDonguriDuel",	// GOKUSYOU_80
		"x10y12pxDonguriDuel",	// GOKUSYOU_110
		"x12y12pxMaruMinya",	// MARUMINYA_50
		"x12y12pxMaruMinya",	// MARUMINYA_80
	};

	// フォントの太さテーブル
	const int FONT_THICK_TABLE[(int)FONT_KINDS_SIZE::MAX] =
	{
		-1,	// DEFAULT_45
		-1,	// DEFAULT_64
		-1,	// GOKUSYOU_32
		-1,	// GOKUSYOU_60
		-1,	// GOKUSYOU_64
		-1,	// GOKUSYOU_80
		-1,	// GOKUSYOU_110
		-1,	// MARUMINYA_50
		20,	// MARUMINYA_80
	};

#pragma region フォントのタイプ一覧
	// 0x00	-> ノーマルフォント
	// 0x01	-> エッジつきフォント
	// 0x02	-> アンチエイリアスフォント( 標準機能アンチエイリアス )
	// 0x12	-> アンチエイリアスフォント( 4x4サンプリング )
	// 0x22	-> アンチエイリアスフォント( 8x8サンプリング )
	// 0x32	-> アンチエイリアスフォント( 16x16サンプリング )
	// 0x03	-> アンチエイリアス＆エッジ付きフォント( 標準機能アンチエイリアス )
	// 0x13	-> アンチエイリアス＆エッジ付きフォント( 4x4サンプリング )
	// 0x23	-> アンチエイリアス＆エッジ付きフォント( 8x8サンプリング )
	// 0x33	-> アンチエイリアス＆エッジ付きフォント( 16x16サンプリング )
#pragma endregion

	// フォントタイプテーブル
	const int FONT_TYPE_TABLE[(int)FONT_KINDS_SIZE::MAX] =
	{
		0x01,	// DEFAULT_45
		0x01,	// DEFAULT_64
		0x01,	// GOKUSYOU_32
		0x01,	// GOKUSYOU_60
		0x01,	// GOKUSYOU_64
		0x01,	// GOKUSYOU_80
		0x01,	// GOKUSYOU_110
		0x01,	// MARUMINYA_50
		0x01,	// MARUMINYA_80
	};


	// フォントをウィンドウズに一時保持するためのフォントデータパス
	const char* FONT_PATH_TABLE[2] =
	{
		"Data/Font/x10y12pxDonguriDuel.ttf",
		"Data/Font/x12y12pxMaruMinya.ttf",
	};

};

using Font = FontManager;
using FontKinds = Font::FONT_KINDS_SIZE;