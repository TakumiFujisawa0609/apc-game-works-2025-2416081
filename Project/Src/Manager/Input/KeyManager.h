#pragma once

#include<string>
#include<vector>
#include"../../Common/Vector2.h"

#include"InputTextManager.h"

class KeyManager
{
private:
	
	// コンストラクタ/デストラクタ
	KeyManager();
	~KeyManager() = default;

	// コピー・ムーブ操作を禁止
	KeyManager(const KeyManager&) = delete;
	KeyManager& operator=(const KeyManager&) = delete;
	KeyManager(KeyManager&&) = delete;
	KeyManager& operator=(KeyManager&&) = delete;

	static KeyManager* ins;

public:

#pragma region シングルトン定義
	// 生成/初期化処理
	static void CreateIns(void) { if (ins == nullptr) { ins = new KeyManager(); ins->Init(); } }
	// 取得
	static KeyManager& GetIns(void) { return *ins; }
	// 終了処理/削除
	static void DeleteIns(void) { if (ins != nullptr) { ins->Release(); delete ins; ins = nullptr; } }
#pragma endregion

	void Update(void);

	enum class KEY_TYPE
	{
		NON = -1,

#pragma region プレイヤー操作
		// 移動
		PLAYER_MOVE_FRONT,
		PLAYER_MOVE_BACK,
		PLAYER_MOVE_RIGHT,
		PLAYER_MOVE_LEFT,

		// ジャンプ
		PLAYER_JUMP,

		// 回避
		PLAYER_EVASION,

		// 通常攻撃（パンチ）
		PLAYER_ATTACK,

		// 特殊行動（えぐり取り）
		PLAYER_GOUGE,
#pragma endregion

#pragma region カメラ操作
		// 移動
		CAMERA_MOVE_FRONT,
		CAMERA_MOVE_BACK,
		CAMERA_MOVE_RIGHT,
		CAMERA_MOVE_LEFT,
		CAMERA_MOVE_UP,
		CAMERA_MOVE_DOWN,

		// 回転
		CAMERA_ROT_FRONT,
		CAMERA_ROT_BACK,
		CAMERA_ROT_RIGHT,
		CAMERA_ROT_LEFT,
#pragma endregion

#pragma region システム操作
		// 汎用（ポーズ/終了）キー
		PAUSE,

		// 汎用決定キー
		ENTER,

		// 汎用方向キー
		UP,
		DOWN,
		LEFT,
		RIGHT,

		// テキスト入力時の(日本語/英語)を切り替えるキー
		TEXT_INPUT_LANGUAGE_SWITCH,
#pragma endregion

#pragma region デバッグキー
		// デバッグオブジェクト操作
		DEBUG_OBJECT_FRONT,
		DEBUG_OBJECT_BACK,
		DEBUG_OBJECT_RIGHT,
		DEBUG_OBJECT_LEFT,
		DEBUG_OBJECT_UP,
		DEBUG_OBJECT_DOWN,

		// デバッグ描画スイッチ
		DEBUG_DRAW_SWITCH,

		// デバッグシーンの切り替え
		DEBUG_MODE_SWITCH,

		// デバッグモード時元のシーンの更新
		DEBUG_MODE_TOPUPDATE,
#pragma endregion

		MAX,
	};

	struct KEY_INFO { bool prev = false, now = false, up = false, down = false; };

	const KEY_INFO GetInfo(KEY_TYPE k) { return keyInfo[(int)k]; }

	bool GetControllerConnect(void) const;

	Vector2 GetRightStickVec(void) const;
	Vector2 GetLeftStickVec(void) const;

	struct MOUCE_INFO
	{
		Vector2I now = {};
		Vector2I prev = {};
		Vector2 move = {};
	};

	const MOUCE_INFO GetMouceInfo(void)const { return mouceInfo; }
	const Vector2 GetMouceMove(void)const { return mouceInfo.move; }

	const Vector2I GetMoucePoint(void)const { return mouceInfo.now; }

	/// <summary>
	/// マウスカーソルを中心に固定する設定
	/// (固定した場合GetMouceMove()にてマウスの単位ベクトルを受け取れる)
	/// </summary>
	/// <param name="fixed">true=固定する、false=固定しない</param>
	void SetMouceFixed(bool fixed);

	// 最後の入力の種類(キーボード,マウスorコントローラー)を判別するフラグ（true = コントローラー、false = キーボード,マウス）
	bool LastInputKinds(void)const { return lastInputKinds; }

	// キーボードによるテキスト入力管理を実行するかどうか
	bool IsInputText(void)const { return isInputText; }
	// キーボードによるテキスト入力管理を実行するかどうかを切り替える
	void IsInputTextSwitch(void) { isInputText = !isInputText; }

	// キーボードによるテキスト入力管理クラスの取得
	InputTextManager& InputText(void) { return *inputTextManager; }

private:
	void Init(void);
	void Release(void);

	void KeyUpdate(void);

	KEY_INFO keyInfo[(int)KEY_TYPE::MAX];

	std::vector<int>keyboardFormat[(int)KEY_TYPE::MAX];
	std::vector<int>controllerButtonFormat[(int)KEY_TYPE::MAX];
	std::vector<int>mouceButtonFormat[(int)KEY_TYPE::MAX];
	enum class CONTROLLER_OTHERS
	{
		LEFTSTICK_UP,
		LEFTSTICK_DOWN,
		LEFTSTICK_RIGHT,
		LEFTSTICK_LEFT,

		RIGHTSTICK_UP,
		RIGHTSTICK_DOWN,
		RIGHTSTICK_RIGHT,
		RIGHTSTICK_LEFT,

		LEFT_TRIGGER,
		RIGHT_TRIGGER,
	};
	std::vector<CONTROLLER_OTHERS>controllerOthersFormat[(int)KEY_TYPE::MAX];
	bool ControllerOthersInput(const CONTROLLER_OTHERS& input);


	void MouceUpdate(void);
	MOUCE_INFO mouceInfo;
	bool mouceFixed;
	const float MOUCE_THRESHOLD = 1.0f;

	// 最後の入力の種類(キーボード,マウスorコントローラー)を判別するフラグ（true = コントローラー、false = キーボード,マウス）
	bool lastInputKinds;

#pragma region キーボードによるテキスト入力管理クラス
	bool isInputText;
	InputTextManager* inputTextManager;
#pragma endregion
};

using Key = KeyManager;
using KEY_TYPE = Key::KEY_TYPE;