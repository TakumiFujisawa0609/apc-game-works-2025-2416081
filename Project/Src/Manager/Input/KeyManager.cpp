#include"KeyManager.h"

#include<DxLib.h>

KeyManager* KeyManager::ins = nullptr;

KeyManager::KeyManager():
	keyInfo(),
	keyboardFormat(),
	controllerButtonFormat()
{
}

void KeyManager::Init(void)
{
	// キーボードを割り振るとき
#define SET_KEYBOARD(type,key)keyboardFormat[(int)type].emplace_back(key)

	// コントローラーのボタンを割り振るとき
#define SET_C_BUTTON(type,key)controllerButtonFormat[(int)type].emplace_back(key)

	// コントローラーのボタン以外(スティックやトリガーなど)を割り振るとき
#define SET_C_OTHERS(type,key)controllerOthersFormat[(int)type].emplace_back(key)

	SET_KEYBOARD(KEY_TYPE::MOVE_UP, KEY_INPUT_W);
	SET_C_OTHERS(KEY_TYPE::MOVE_UP, CONTROLLER_OTHERS::LEFTSTICK_UP);

	SET_KEYBOARD(KEY_TYPE::MOVE_DOWN, KEY_INPUT_S);
	SET_C_OTHERS(KEY_TYPE::MOVE_DOWN, CONTROLLER_OTHERS::LEFTSTICK_DOWN);

	SET_KEYBOARD(KEY_TYPE::MOVE_RIGHT, KEY_INPUT_D);
	SET_C_OTHERS(KEY_TYPE::MOVE_RIGHT, CONTROLLER_OTHERS::LEFTSTICK_RIGHT);

	SET_KEYBOARD(KEY_TYPE::MOVE_LEFT, KEY_INPUT_A);
	SET_C_OTHERS(KEY_TYPE::MOVE_LEFT, CONTROLLER_OTHERS::LEFTSTICK_LEFT);


	SET_KEYBOARD(KEY_TYPE::JUMP, KEY_INPUT_SPACE);
	SET_KEYBOARD(KEY_TYPE::JUMP, KEY_INPUT_B);
	SET_C_BUTTON(KEY_TYPE::JUMP, XINPUT_BUTTON_A);

	SET_KEYBOARD(KEY_TYPE::ATTACK, KEY_INPUT_J);
	SET_KEYBOARD(KEY_TYPE::ATTACK, KEY_INPUT_RETURN);
	SET_C_BUTTON(KEY_TYPE::ATTACK, XINPUT_BUTTON_X);
	SET_C_BUTTON(KEY_TYPE::ATTACK, XINPUT_BUTTON_Y);
	SET_C_OTHERS(KEY_TYPE::ATTACK, CONTROLLER_OTHERS::RIGHT_TRIGGER);

	SET_KEYBOARD(KEY_TYPE::EVASION, KEY_INPUT_K);
	SET_KEYBOARD(KEY_TYPE::EVASION, KEY_INPUT_LSHIFT);
	SET_C_BUTTON(KEY_TYPE::EVASION, XINPUT_BUTTON_B);
	SET_C_OTHERS(KEY_TYPE::EVASION, CONTROLLER_OTHERS::LEFT_TRIGGER);


}

void KeyManager::Update(void)
{
	for (int i = 0; i < (int)KEY_TYPE::MAX; i++) {
		keyInfo[i].prev = keyInfo[i].now;

		bool b = false;

		for (auto& input : keyboardFormat[i]) {
			if (b) { break; }

			if (CheckHitKey(input) != 0) { b = true; }
		}
		for (auto& input : controllerButtonFormat[i]) {
			if (b) { break; }

			XINPUT_STATE state = {};
			if (GetJoypadXInputState(DX_INPUT_PAD1, &state) != 0) { state = {}; }

			if (state.Buttons[i] != 0) { b = true; }
		}
		for (CONTROLLER_OTHERS input : controllerOthersFormat[i]) {
			if (b) { break; }

			XINPUT_STATE state = {};
			if (GetJoypadXInputState(DX_INPUT_PAD1, &state) != 0) { state = {}; }

			switch (input)
			{
			case KeyManager::CONTROLLER_OTHERS::LEFTSTICK_UP:
				if (state.ThumbLY < 0) { b = true; }
				break;
			case KeyManager::CONTROLLER_OTHERS::LEFTSTICK_DOWN:
				if (state.ThumbLY > 0) { b = true; }
				break;
			case KeyManager::CONTROLLER_OTHERS::LEFTSTICK_RIGHT:
				if (state.ThumbLX > 0) { b = true; }
				break;
			case KeyManager::CONTROLLER_OTHERS::LEFTSTICK_LEFT:
				if (state.ThumbLX < 0) { b = true; }
				break;
			case KeyManager::CONTROLLER_OTHERS::RIGHTSTICK_UP:
				if (state.ThumbRY < 0) { b = true; }
				break;
			case KeyManager::CONTROLLER_OTHERS::RIGHTSTICK_DOWN:
				if (state.ThumbRY > 0) { b = true; }
				break;
			case KeyManager::CONTROLLER_OTHERS::RIGHTSTICK_RIGHT:
				if (state.ThumbRX > 0) { b = true; }
				break;
			case KeyManager::CONTROLLER_OTHERS::RIGHTSTICK_LEFT:
				if (state.ThumbRX < 0) { b = true; }
				break;
			case KeyManager::CONTROLLER_OTHERS::LEFT_TRIGGER:
				if (state.LeftTrigger > 0) { b = true; }
				break;
			case KeyManager::CONTROLLER_OTHERS::RIGHT_TRIGGER:
				if (state.RightTrigger > 0) { b = true; }
				break;
			default:
				break;
			}
		}

		keyInfo[i].now = b;

		keyInfo[i].up = (keyInfo[i].prev && !keyInfo[i].now);
		keyInfo[i].down = (!keyInfo[i].prev && keyInfo[i].now);
	}
}

void KeyManager::Release(void)
{
	for (auto& input : keyboardFormat) { input.clear(); }
	for (auto& input : controllerButtonFormat) { input.clear(); }
	for (auto& input : controllerOthersFormat) { input.clear(); }
}