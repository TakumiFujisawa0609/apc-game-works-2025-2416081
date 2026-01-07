#pragma once

#include<string>

#include"../../../../../Common/Vector2.h"

class PlayerOperationUI
{
public:
	PlayerOperationUI(const int& playerState);
	~PlayerOperationUI() = default;

#pragma region 定数 （private/public）

private:
	// アクションを表現したイラストの読み込み時のサイズ
	static constexpr unsigned short ACTION_IMG_LOAD_SIZE_X = 256;
	static constexpr unsigned char ACTION_IMG_LOAD_SIZE_Y = 197;

	// アクションを表現したイラストの描画時の拡大縮小倍率
	//static constexpr float ACTION_IMG_DRAW_SCALE = 0.35f;
	static constexpr float ACTION_IMG_DRAW_SCALE = 0.390625f;

	// アクションを表現したイラストの描画時のサイズ
	static constexpr unsigned char ACTION_IMG_SIZE_X = (unsigned char)(ACTION_IMG_LOAD_SIZE_X * ACTION_IMG_DRAW_SCALE);
	static constexpr unsigned char ACTION_IMG_SIZE_Y = (unsigned char)(ACTION_IMG_LOAD_SIZE_Y * ACTION_IMG_DRAW_SCALE);


	// 操作ボタンイラスト(キーボード)の描画時の拡大縮小倍率
	static constexpr float OPERATION_KEYBOARD_IMG_DRAW_SCALE = 0.175f;

	// 操作ボタンイラスト(キーボード)の描画時のオフセット
	const float OPERATION_KEYBOARD_IMG_DRAW_OFFSET_X = (float)ACTION_IMG_SIZE_X / 2.0f;
	const float OPERATION_KEYBOARD_IMG_DRAW_OFFSET_Y = 0.0f;


	// 操作ボタンイラスト(コントローラー)の描画時の拡大縮小倍率
	static constexpr float OPERATION_CONTROLLER_IMG_DRAW_SCALE = 0.175f;

	// 操作ボタンイラスト(キーボード)の描画時のオフセット
	const float OPERATION_CONTROLLER_IMG_DRAW_OFFSET_X = (float)ACTION_IMG_SIZE_X / 2.0f;
	const float OPERATION_CONTROLLER_IMG_DRAW_OFFSET_Y = 0.0f;

	// 全アクションの列挙型定義
	enum class ACTION
	{
		MOVE,
		JUMP,
		EVASION,
		PUNCH,
		GOUGE,
		THROWING,

		MAX
	};

	// 配置の列挙型定義
	enum class SORT
	{
		MOVE,
		JUMP,
		EVASION,
		ATTACK,
		GOUGE,

		MAX
	};


public:
	
	// フレームまで含めた全体の大きさ
	static constexpr float WHOLE_SIZE_X = (float)(ACTION_IMG_SIZE_X * (int)SORT::MAX);
	static constexpr float WHOLE_SIZE_Y = (float)ACTION_IMG_SIZE_Y;

#pragma endregion

public:

	void Load(void);
	void Init(const Vector2& pos);
	void Update(void);
	void Draw(void);
	void Release(void);

private:

#pragma region 画像ハンドル
	// アクションを表現したイラスト
	int actionImg[(int)ACTION::MAX];
	// ↑パス
	const std::string ACTION_IMG_PATH = "Data/Image/Game/UI/PlayerOperationUI/ActionImage/";
	const std::string ACTION_IMG_NAME[(int)ACTION::MAX] = { "Move","Jump","Evasion","Punch","Gouge","Throwing" };

	// 操作ボタンイラスト(キーボード)
	int operationKeyboardImg[(int)SORT::MAX];
	// ↑パス
	const std::string OPERATION_KEYBOARD_IMG_PATH = "Data/Image/Game/UI/PlayerOperationUI/OperationKeyboard/";
	const std::string OPERATION_KEYBOARD_IMG_NAME[(int)SORT::MAX] = { "WASD","Space","Lshift","J","K" };

	// 操作ボタンイラスト(コントローラー)
	int operationControllerImg[(int)SORT::MAX];
	// ↑パス
	const std::string OPERATION_CONTROLLER_IMG_PATH = "Data/Image/Game/UI/PlayerOperationUI/OperationController/";
	const std::string OPERATION_CONTROLLER_IMG_NAME[(int)SORT::MAX] = { "Lstick","A","Ltrigger","X","Rtrigger" };

	// 共通のファイル形式
	const std::string COMMON_IMG_FILE_NAME = ".png";
#pragma endregion

	// 左上の始点座標
	Vector2 pos;

	// プレイヤーの状態の参照
	const int& playerState;

	// 配置番号からアクション番号に変換する関数
	int SortNumToActionNum(int sort);

	//
	bool actionPossibleFlg[(int)SORT::MAX];
};