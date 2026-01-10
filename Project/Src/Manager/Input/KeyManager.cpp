#include"KeyManager.h"

#include"../../Application/Application.h"
#include"../../Utility/Utility.h"

KeyManager* KeyManager::ins = nullptr;

KeyManager::KeyManager():
	keyInfo(),
	keyboardFormat(),
	mouceButtonFormat(),
	controllerButtonFormat(),
	mouceFixed_(false),

	lastInputKinds(false)
{
}

void KeyManager::Init(void)
{
	// キーボードを割り振るとき
#define SET_KEYBOARD(type,key)keyboardFormat[(int)type].emplace_back(key)

	// コントローラーのボタンを割り振るとき
#define SET_C_BUTTON(type,key)controllerButtonFormat[(int)type].emplace_back(key)

	// マウスのボタンを割り振るとき
#define SET_MOUCE_BUTTON(type,key)mouceButtonFormat[(int)type].emplace_back(key)

	// コントローラーのボタン以外(スティックやトリガーなど)を割り振るとき
#define SET_C_OTHERS(type,key)controllerOthersFormat[(int)type].emplace_back(key)

#pragma region プレイヤー操作
	// 移動
	SET_KEYBOARD(KEY_TYPE::PLAYER_MOVE_FRONT, KEY_INPUT_W);
	SET_C_BUTTON(KEY_TYPE::PLAYER_MOVE_FRONT, XINPUT_BUTTON_DPAD_UP);
	SET_C_OTHERS(KEY_TYPE::PLAYER_MOVE_FRONT, CONTROLLER_OTHERS::LEFTSTICK_UP);

	SET_KEYBOARD(KEY_TYPE::PLAYER_MOVE_BACK, KEY_INPUT_S);
	SET_C_BUTTON(KEY_TYPE::PLAYER_MOVE_BACK, XINPUT_BUTTON_DPAD_DOWN);
	SET_C_OTHERS(KEY_TYPE::PLAYER_MOVE_BACK, CONTROLLER_OTHERS::LEFTSTICK_DOWN);

	SET_KEYBOARD(KEY_TYPE::PLAYER_MOVE_RIGHT, KEY_INPUT_D);
	SET_C_BUTTON(KEY_TYPE::PLAYER_MOVE_RIGHT, XINPUT_BUTTON_DPAD_RIGHT);
	SET_C_OTHERS(KEY_TYPE::PLAYER_MOVE_RIGHT, CONTROLLER_OTHERS::LEFTSTICK_RIGHT);

	SET_KEYBOARD(KEY_TYPE::PLAYER_MOVE_LEFT, KEY_INPUT_A);
	SET_C_BUTTON(KEY_TYPE::PLAYER_MOVE_LEFT, XINPUT_BUTTON_DPAD_LEFT);
	SET_C_OTHERS(KEY_TYPE::PLAYER_MOVE_LEFT, CONTROLLER_OTHERS::LEFTSTICK_LEFT);

	// ジャンプ
	SET_KEYBOARD(KEY_TYPE::PLAYER_JUMP, KEY_INPUT_SPACE);
	SET_KEYBOARD(KEY_TYPE::PLAYER_JUMP, KEY_INPUT_B);
	SET_C_BUTTON(KEY_TYPE::PLAYER_JUMP, XINPUT_BUTTON_A);

	// 回避
	SET_KEYBOARD(KEY_TYPE::PLAYER_EVASION, KEY_INPUT_H);
	SET_KEYBOARD(KEY_TYPE::PLAYER_EVASION, KEY_INPUT_LSHIFT);
	SET_C_BUTTON(KEY_TYPE::PLAYER_EVASION, XINPUT_BUTTON_B);
	SET_C_OTHERS(KEY_TYPE::PLAYER_EVASION, CONTROLLER_OTHERS::LEFT_TRIGGER);

	// 通常攻撃（パンチ）
	SET_KEYBOARD(KEY_TYPE::PLAYER_ATTACK, KEY_INPUT_J);
	SET_KEYBOARD(KEY_TYPE::PLAYER_ATTACK, KEY_INPUT_RETURN);
	SET_C_BUTTON(KEY_TYPE::PLAYER_ATTACK, XINPUT_BUTTON_X);
	SET_MOUCE_BUTTON(KEY_TYPE::PLAYER_ATTACK, MOUSE_INPUT_LEFT);
	SET_C_OTHERS(KEY_TYPE::PLAYER_ATTACK, CONTROLLER_OTHERS::RIGHT_TRIGGER);

	// 特殊行動（えぐり取り）
	SET_KEYBOARD(KEY_TYPE::PLAYER_GOUGE, KEY_INPUT_K);
	SET_C_BUTTON(KEY_TYPE::PLAYER_GOUGE, XINPUT_BUTTON_Y);
	SET_C_BUTTON(KEY_TYPE::PLAYER_GOUGE, XINPUT_BUTTON_RIGHT_SHOULDER);
	SET_MOUCE_BUTTON(KEY_TYPE::PLAYER_GOUGE, MOUSE_INPUT_RIGHT);
#pragma endregion


#pragma region カメラ操作
	// 移動
	SET_KEYBOARD(KEY_TYPE::CAMERA_MOVE_FRONT, KEY_INPUT_I);
	SET_C_OTHERS(KEY_TYPE::CAMERA_MOVE_FRONT, CONTROLLER_OTHERS::LEFTSTICK_UP);

	SET_KEYBOARD(KEY_TYPE::CAMERA_MOVE_BACK, KEY_INPUT_K);
	SET_C_OTHERS(KEY_TYPE::CAMERA_MOVE_BACK, CONTROLLER_OTHERS::LEFTSTICK_DOWN);

	SET_KEYBOARD(KEY_TYPE::CAMERA_MOVE_RIGHT, KEY_INPUT_L);
	SET_C_OTHERS(KEY_TYPE::CAMERA_MOVE_RIGHT, CONTROLLER_OTHERS::LEFTSTICK_RIGHT);

	SET_KEYBOARD(KEY_TYPE::CAMERA_MOVE_LEFT, KEY_INPUT_J);
	SET_C_OTHERS(KEY_TYPE::CAMERA_MOVE_LEFT, CONTROLLER_OTHERS::LEFTSTICK_LEFT);

	SET_KEYBOARD(KEY_TYPE::CAMERA_MOVE_UP, KEY_INPUT_O);
	SET_C_BUTTON(KEY_TYPE::CAMERA_MOVE_UP, XINPUT_BUTTON_DPAD_UP);

	SET_KEYBOARD(KEY_TYPE::CAMERA_MOVE_DOWN, KEY_INPUT_U);
	SET_C_BUTTON(KEY_TYPE::CAMERA_MOVE_DOWN, XINPUT_BUTTON_DPAD_DOWN);

	// 回転
	SET_KEYBOARD(KEY_TYPE::CAMERA_ROT_FRONT, KEY_INPUT_UP);
	SET_C_OTHERS(KEY_TYPE::CAMERA_ROT_FRONT, CONTROLLER_OTHERS::RIGHTSTICK_UP);

	SET_KEYBOARD(KEY_TYPE::CAMERA_ROT_BACK, KEY_INPUT_DOWN);
	SET_C_OTHERS(KEY_TYPE::CAMERA_ROT_BACK, CONTROLLER_OTHERS::RIGHTSTICK_DOWN);

	SET_KEYBOARD(KEY_TYPE::CAMERA_ROT_RIGHT, KEY_INPUT_RIGHT);
	SET_C_OTHERS(KEY_TYPE::CAMERA_ROT_RIGHT, CONTROLLER_OTHERS::RIGHTSTICK_RIGHT);

	SET_KEYBOARD(KEY_TYPE::CAMERA_ROT_LEFT, KEY_INPUT_LEFT);
	SET_C_OTHERS(KEY_TYPE::CAMERA_ROT_LEFT, CONTROLLER_OTHERS::RIGHTSTICK_LEFT);
#pragma endregion


#pragma region システム操作
	// 汎用（ポーズ/終了）キー
	SET_KEYBOARD(KEY_TYPE::PAUSE, KEY_INPUT_ESCAPE);
	SET_C_BUTTON(KEY_TYPE::PAUSE, XINPUT_BUTTON_START);

	// 汎用決定キー
	SET_KEYBOARD(KEY_TYPE::ENTER, KEY_INPUT_SPACE);
	SET_KEYBOARD(KEY_TYPE::ENTER, KEY_INPUT_RETURN);
	SET_C_BUTTON(KEY_TYPE::ENTER, XINPUT_BUTTON_B);
	SET_C_BUTTON(KEY_TYPE::ENTER, XINPUT_BUTTON_X);

	// 汎用方向キー
	SET_KEYBOARD(KEY_TYPE::UP, KEY_INPUT_W);
	SET_KEYBOARD(KEY_TYPE::UP, KEY_INPUT_UP);
	SET_C_BUTTON(KEY_TYPE::UP, XINPUT_BUTTON_DPAD_UP);
	SET_C_OTHERS(KEY_TYPE::UP, CONTROLLER_OTHERS::LEFTSTICK_UP);
	SET_C_OTHERS(KEY_TYPE::UP, CONTROLLER_OTHERS::RIGHTSTICK_UP);
	
	SET_KEYBOARD(KEY_TYPE::DOWN, KEY_INPUT_S);
	SET_KEYBOARD(KEY_TYPE::DOWN, KEY_INPUT_DOWN);
	SET_C_BUTTON(KEY_TYPE::DOWN, XINPUT_BUTTON_DPAD_DOWN);
	SET_C_OTHERS(KEY_TYPE::DOWN, CONTROLLER_OTHERS::LEFTSTICK_DOWN);
	SET_C_OTHERS(KEY_TYPE::DOWN, CONTROLLER_OTHERS::RIGHTSTICK_DOWN);

	SET_KEYBOARD(KEY_TYPE::RIGHT, KEY_INPUT_D);
	SET_KEYBOARD(KEY_TYPE::RIGHT, KEY_INPUT_RIGHT);
	SET_C_BUTTON(KEY_TYPE::RIGHT, XINPUT_BUTTON_DPAD_RIGHT);
	SET_C_OTHERS(KEY_TYPE::RIGHT, CONTROLLER_OTHERS::LEFTSTICK_RIGHT);
	SET_C_OTHERS(KEY_TYPE::RIGHT, CONTROLLER_OTHERS::RIGHTSTICK_RIGHT);

	SET_KEYBOARD(KEY_TYPE::LEFT, KEY_INPUT_A);
	SET_KEYBOARD(KEY_TYPE::LEFT, KEY_INPUT_LEFT);
	SET_C_BUTTON(KEY_TYPE::LEFT, XINPUT_BUTTON_DPAD_LEFT);
	SET_C_OTHERS(KEY_TYPE::LEFT, CONTROLLER_OTHERS::LEFTSTICK_LEFT);
	SET_C_OTHERS(KEY_TYPE::LEFT, CONTROLLER_OTHERS::RIGHTSTICK_LEFT);
#pragma endregion


#pragma region デバッグキー
	// デバッグオブジェクト操作
	SET_KEYBOARD(KEY_TYPE::DEBUG_OBJECT_FRONT, KEY_INPUT_T);
	SET_KEYBOARD(KEY_TYPE::DEBUG_OBJECT_BACK, KEY_INPUT_G);
	SET_KEYBOARD(KEY_TYPE::DEBUG_OBJECT_RIGHT, KEY_INPUT_H);
	SET_KEYBOARD(KEY_TYPE::DEBUG_OBJECT_LEFT, KEY_INPUT_F);
	SET_KEYBOARD(KEY_TYPE::DEBUG_OBJECT_UP, KEY_INPUT_Y);
	SET_KEYBOARD(KEY_TYPE::DEBUG_OBJECT_DOWN, KEY_INPUT_R);

	// デバッグ描画スイッチ
	SET_KEYBOARD(KEY_TYPE::DEBUG_DRAW_SWITCH, KEY_INPUT_8);

	// デバッグシーンの切り替え
	SET_KEYBOARD(KEY_TYPE::DEBUG_MODE_SWITCH, KEY_INPUT_9);

	// デバッグモード時元のシーンの更新
	SET_KEYBOARD(KEY_TYPE::DEBUG_MODE_TOPUPDATE, KEY_INPUT_0);
#pragma endregion
}

void KeyManager::Update(void)
{
	KeyUpdate();
	MouceUpdate();
}

void KeyManager::Release(void)
{
	for (auto& input : keyboardFormat) { input.clear(); }			keyboardFormat->clear();
	for (auto& input : controllerButtonFormat) { input.clear(); }	controllerButtonFormat->clear();
	for (auto& input : mouceButtonFormat) { input.clear(); }		mouceButtonFormat->clear();
	for (auto& input : controllerOthersFormat) { input.clear(); }	controllerOthersFormat->clear();
}

void KeyManager::KeyUpdate(void)
{
	for (int i = 0; i < (int)KEY_TYPE::MAX; i++) {
		keyInfo[i].prev = keyInfo[i].now;

		bool b = false;

		for (auto& input : keyboardFormat[i]) {
			if (b) { break; }

			if (CheckHitKey(input) != 0) { b = true; lastInputKinds = false; }
		}
		for (auto& input : controllerButtonFormat[i]) {
			if (b) { break; }

			XINPUT_STATE state = {};
			if (GetJoypadXInputState(DX_INPUT_PAD1, &state) != 0) { state = {}; }

			if (state.Buttons[input] != 0) { b = true; lastInputKinds = true; }
		}
		for (auto& input : mouceButtonFormat[i]) {
			if (b) { break; }
			if (GetMouseInput() & input) { b = true; lastInputKinds = false; }
		}
		for (CONTROLLER_OTHERS input : controllerOthersFormat[i]) {
			if (b) { break; }
			if (ControllerOthersInput(input)) { b = true; lastInputKinds = true; }
		}

		keyInfo[i].now = b;

		keyInfo[i].up = (keyInfo[i].prev && !keyInfo[i].now);
		keyInfo[i].down = (!keyInfo[i].prev && keyInfo[i].now);
	}
}

bool KeyManager::ControllerOthersInput(const CONTROLLER_OTHERS& input)
{
	XINPUT_STATE state = {};
	if (GetJoypadXInputState(DX_INPUT_PAD1, &state) != 0) { state = {}; }

	short lenge = 10000;

	switch (input)
	{
	case KeyManager::CONTROLLER_OTHERS::LEFTSTICK_UP:
		if (state.ThumbLY > lenge) { return true; }
		break;
	case KeyManager::CONTROLLER_OTHERS::LEFTSTICK_DOWN:
		if (state.ThumbLY < -lenge) { return true; }
		break;
	case KeyManager::CONTROLLER_OTHERS::LEFTSTICK_RIGHT:
		if (state.ThumbLX > lenge) { return true; }
		break;
	case KeyManager::CONTROLLER_OTHERS::LEFTSTICK_LEFT:
		if (state.ThumbLX < -lenge) { return true; }
		break;


	case KeyManager::CONTROLLER_OTHERS::RIGHTSTICK_UP:
		if (state.ThumbRY > lenge) { return true; }
		break;
	case KeyManager::CONTROLLER_OTHERS::RIGHTSTICK_DOWN:
		if (state.ThumbRY < -lenge) { return true; }
		break;
	case KeyManager::CONTROLLER_OTHERS::RIGHTSTICK_RIGHT:
		if (state.ThumbRX > lenge) { return true; }
		break;
	case KeyManager::CONTROLLER_OTHERS::RIGHTSTICK_LEFT:
		if (state.ThumbRX < -lenge) { return true; }
		break;


	case KeyManager::CONTROLLER_OTHERS::LEFT_TRIGGER:
		if (state.LeftTrigger > 0) { return true; }
		break;
	case KeyManager::CONTROLLER_OTHERS::RIGHT_TRIGGER:
		if (state.RightTrigger > 0) { return true; }
		break;
	default:
		return false;
		break;
	}

	return false;
}

void KeyManager::MouceUpdate(void)
{
	if (mouceFixed_) {

		mouceInfo.prev = { Application::SCREEN_SIZE_X / 2,Application::SCREEN_SIZE_Y / 2 };

		GetMousePoint(&mouceInfo.now.x, &mouceInfo.now.y);

		Vector2 move = mouceInfo.now.ToVector2() - mouceInfo.prev.ToVector2();

		mouceInfo.move = (move.Length() > MOUCE_THRESHOLD) ? (mouceInfo.now - mouceInfo.prev).Normalized() : Vector2(0.0f, 0.0f);

		SetMousePoint(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2);

	}
	else {

		mouceInfo.prev = mouceInfo.now;

		GetMousePoint(&mouceInfo.now.x, &mouceInfo.now.y);

		Vector2 move = mouceInfo.now.ToVector2() - mouceInfo.prev.ToVector2();

		mouceInfo.move = (move.Length() > MOUCE_THRESHOLD) ? (mouceInfo.now - mouceInfo.prev).Normalized() : Vector2(0.0f, 0.0f);
	}
}

bool KeyManager::GetControllerConnect(void) const
{
	return (GetJoypadNum() > 0);
}

Vector2 KeyManager::GetRightStickVec(void) const
{
	XINPUT_STATE state = {};
	if (GetJoypadXInputState(DX_INPUT_PAD1, &state) != 0) { return { 0.0f,0.0f }; }
	short lenge = 10000;

	Vector2 vec = { (abs(state.ThumbRX) > lenge) ? (float)state.ThumbRX : 0.0f,(abs(state.ThumbRY) > lenge) ? (float)-state.ThumbRY : 0.0f };

	if (vec == 0.0f) { return{ 0.0f,0.0f }; }

	return vec / sqrtf(vec.x * vec.x + vec.y * vec.y);
}


Vector2 KeyManager::GetLeftStickVec(void) const
{
	XINPUT_STATE state = {};
	if (GetJoypadXInputState(DX_INPUT_PAD1, &state) != 0) { return { 0.0f,0.0f }; }
	short lenge = 10000;

	Vector2 vec = { (abs(state.ThumbLX) > lenge) ? (float)state.ThumbLX : 0.0f,(abs(state.ThumbLY) > lenge) ? (float)-state.ThumbLY : 0.0f };

	if (vec == 0.0f) { return{ 0.0f,0.0f }; }

	return vec / sqrtf(vec.x * vec.x + vec.y * vec.y);
}

void KeyManager::SetMouceFixed(bool fixed)
{
	mouceFixed_ = fixed;
	SetMouseDispFlag(!fixed);
}