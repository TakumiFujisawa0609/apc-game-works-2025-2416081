#include"PlayerOperationUI.h"

#include"../../../../../Utility/Utility.h"

#include"../../../../../Manager/Input/KeyManager.h"

#include"../../Player.h"

PlayerOperationUI::PlayerOperationUI(const int& playerState) :

	playerState(playerState),

	actionImg(),
	operationKeyboardImg(),
	operationControllerImg(),

	pos(),

	actionPossibleFlg(true, true, true, true, true)
{
}

void PlayerOperationUI::Load(void)
{
	// äeâÊëúÇÃÉçÅ[Éh
	for (unsigned char i = 0; i < (unsigned char)ACTION::MAX; i++) {
		// ÉAÉNÉVÉáÉìÇï\åªÇµÇΩÉCÉâÉXÉg
		Utility::LoadImg(actionImg[i], ACTION_IMG_PATH + ACTION_IMG_NAME[i] + COMMON_IMG_FILE_NAME);
	}
	for (unsigned char i = 0; i < (unsigned char)SORT::MAX; i++) {
		// ëÄçÏÉ{É^ÉìÉCÉâÉXÉg(ÉLÅ[É{Å[Éh)
		Utility::LoadImg(operationKeyboardImg[i], OPERATION_KEYBOARD_IMG_PATH + OPERATION_KEYBOARD_IMG_NAME[i] + COMMON_IMG_FILE_NAME);

		// ëÄçÏÉ{É^ÉìÉCÉâÉXÉg(ÉRÉìÉgÉçÅ[ÉâÅ[)
		Utility::LoadImg(operationControllerImg[i], OPERATION_CONTROLLER_IMG_PATH + OPERATION_CONTROLLER_IMG_NAME[i] + COMMON_IMG_FILE_NAME);
	}
}

void PlayerOperationUI::Init(const Vector2& pos)
{
	// ç¿ïWÇÉZÉbÉg
	this->pos = pos;
}

void PlayerOperationUI::Update(void)
{
	switch (playerState)
	{
	case (int)Player::STATE::NON: { break; }

	case (int)Player::STATE::MOVE: {
		actionPossibleFlg[(int)SORT::MOVE] = true;
		actionPossibleFlg[(int)SORT::JUMP] = true;
		actionPossibleFlg[(int)SORT::EVASION] = true;
		actionPossibleFlg[(int)SORT::ATTACK] = true;
		actionPossibleFlg[(int)SORT::GOUGE] = true;
		break;
	}
	case (int)Player::STATE::ATTACK: {
		actionPossibleFlg[(int)SORT::MOVE] = false;
		actionPossibleFlg[(int)SORT::JUMP] = false;
		actionPossibleFlg[(int)SORT::EVASION] = true;
		actionPossibleFlg[(int)SORT::ATTACK] = true;
		actionPossibleFlg[(int)SORT::GOUGE] = false;
		break;
	}
	case (int)Player::STATE::GOUGE: {
		actionPossibleFlg[(int)SORT::MOVE] = false;
		actionPossibleFlg[(int)SORT::JUMP] = false;
		actionPossibleFlg[(int)SORT::EVASION] = false;
		actionPossibleFlg[(int)SORT::ATTACK] = false;
		actionPossibleFlg[(int)SORT::GOUGE] = true;
		break;
	}
	case (int)Player::STATE::CARRY_OBJ: {
		actionPossibleFlg[(int)SORT::MOVE] = true;
		actionPossibleFlg[(int)SORT::JUMP] = false;
		actionPossibleFlg[(int)SORT::EVASION] = false;
		actionPossibleFlg[(int)SORT::ATTACK] = true;
		actionPossibleFlg[(int)SORT::GOUGE] = false;
		break;
	}
	case (int)Player::STATE::THROWING_OBJ: {
		actionPossibleFlg[(int)SORT::MOVE] = false;
		actionPossibleFlg[(int)SORT::JUMP] = false;
		actionPossibleFlg[(int)SORT::EVASION] = false;
		actionPossibleFlg[(int)SORT::ATTACK] = true;
		actionPossibleFlg[(int)SORT::GOUGE] = false;
		break;
	}
	case (int)Player::STATE::EVASION: {
		actionPossibleFlg[(int)SORT::MOVE] = false;
		actionPossibleFlg[(int)SORT::JUMP] = false;
		actionPossibleFlg[(int)SORT::EVASION] = true;
		actionPossibleFlg[(int)SORT::ATTACK] = false;
		actionPossibleFlg[(int)SORT::GOUGE] = false;
		break;
	}
	case (int)Player::STATE::DAMAGE: {
		actionPossibleFlg[(int)SORT::MOVE] = false;
		actionPossibleFlg[(int)SORT::JUMP] = false;
		actionPossibleFlg[(int)SORT::EVASION] = false;
		actionPossibleFlg[(int)SORT::ATTACK] = false;
		actionPossibleFlg[(int)SORT::GOUGE] = false;
		break;
	}
	case (int)Player::STATE::DEATH: {
		actionPossibleFlg[(int)SORT::MOVE] = false;
		actionPossibleFlg[(int)SORT::JUMP] = false;
		actionPossibleFlg[(int)SORT::EVASION] = false;
		actionPossibleFlg[(int)SORT::ATTACK] = false;
		actionPossibleFlg[(int)SORT::GOUGE] = false;
		break;
	}
	case (int)Player::STATE::END: {
		actionPossibleFlg[(int)SORT::MOVE] = false;
		actionPossibleFlg[(int)SORT::JUMP] = false;
		actionPossibleFlg[(int)SORT::EVASION] = false;
		actionPossibleFlg[(int)SORT::ATTACK] = false;
		actionPossibleFlg[(int)SORT::GOUGE] = false;
		break;
	}

	}
}

void PlayerOperationUI::Draw(void)
{
	bool alpha = false;

	for (unsigned char i = 0; i < (unsigned char)SORT::MAX; i++) {

		if (alpha != !actionPossibleFlg[i]) {
			alpha = !actionPossibleFlg[i];
			if (alpha) { SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150); }
			else { SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); }
		}

		DrawRotaGraph3F(pos.x + (i * ACTION_IMG_SIZE_X), pos.y, 0.0f, 0.0f, ACTION_IMG_DRAW_SCALE, ACTION_IMG_DRAW_SCALE, 0, actionImg[SortNumToActionNum(i)], true);

		if (KEY::GetIns().LastInputKinds()) {
			DrawRotaGraphF(
				pos.x + (i * ACTION_IMG_SIZE_X) + OPERATION_CONTROLLER_IMG_DRAW_OFFSET_X,
				pos.y + OPERATION_CONTROLLER_IMG_DRAW_OFFSET_Y,
				OPERATION_CONTROLLER_IMG_DRAW_SCALE, 0,
				operationControllerImg[i], true);
		}
		else {
			DrawRotaGraphF(
				pos.x + (i * ACTION_IMG_SIZE_X) + OPERATION_KEYBOARD_IMG_DRAW_OFFSET_X,
				pos.y + OPERATION_KEYBOARD_IMG_DRAW_OFFSET_Y,
				OPERATION_KEYBOARD_IMG_DRAW_SCALE, 0,
				operationKeyboardImg[i], true);
		}
	}

	if (alpha) { SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); }
}

void PlayerOperationUI::Release(void)
{
	// äeâÊëúÇÃîjä¸Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`
	for (int& id : actionImg) { DeleteGraph(id); }
	for (int& id : operationKeyboardImg) { DeleteGraph(id); }
	for (int& id : operationControllerImg) { DeleteGraph(id); }
	// Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`Å`
}

int PlayerOperationUI::SortNumToActionNum(int sort)
{
	switch (sort)
	{
	case (int)PlayerOperationUI::SORT::MOVE: { return (int)ACTION::MOVE; }
	case (int)PlayerOperationUI::SORT::JUMP: { return (int)ACTION::JUMP; }
	case (int)PlayerOperationUI::SORT::EVASION: { return (int)ACTION::EVASION; }
	case (int)PlayerOperationUI::SORT::ATTACK: { return (int)((playerState == (int)Player::STATE::CARRY_OBJ) ? ACTION::THROWING : ACTION::PUNCH); }
	case (int)PlayerOperationUI::SORT::GOUGE: { return (int)ACTION::GOUGE; }
	}

	return (int)ACTION::MOVE;
}