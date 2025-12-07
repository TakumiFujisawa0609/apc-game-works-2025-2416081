#include "RockWall.h"

#include <algorithm>
#include <queue>
#include <cmath>

#include"../../../../../Manager/Sound/SoundManager.h"

#include"../../../../../Scene/Game/GameScene.h"

#include"../../../Boss/Boss.h"

RockWall::RockWall(int modelId, int textureId) :

    VoxelBase(),

    state_(STATE::NON),
    stateFuncPtr(),

    stageCollisionFlg_(0)
{
    SetDynamicFlg(false);

    SetPushFlg(true);
    SetPushWeight(100);

    ModelDuplicate(modelId);

    // ボクセルメッシュ生成に必要な情報を設定する
    VoxelInfoInit(
        SIZE,
        textureId,
        30.0f,
        CENTER_DIFF,
        0.1f
    );

    SetJudge(false);
    SetIsDraw(false);
}

void RockWall::On(const Vector3& pos)
{
    ReVival();

    state_ = STATE::MOVE;

    trans_.pos = pos;
    trans_.pos.y = 0.0f;
}

void RockWall::SubLoad(void)
{
#pragma region 関数ポインタ配列へ各関数を格納
#define SET_STATE(state, func) stateFuncPtr[(int)(state)] = static_cast<STATEFUNC>(func)
    SET_STATE(STATE::NON, &RockWall::Non);
    SET_STATE(STATE::MOVE, &RockWall::Move);
    SET_STATE(STATE::BE, &RockWall::Be);
#pragma endregion
}

void RockWall::SubInit(void)
{
    SetJudge(true);
    SetIsDraw(true);

    stageCollisionFlg_ = 0;
}

void RockWall::SubUpdate(void)
{
    // 現在のステートに対応する関数を実行
    (this->*stateFuncPtr[(int)state_])();
}

void RockWall::Move(void)
{
    if (stageCollisionFlg_ == 0) {
        state_ = STATE::BE;
        return;
    }

    stageCollisionFlg_ = 0;

    // 上に動かす（ステージとの接触がなくなるまで）
    trans_.pos.y += MOVE_SPEED;
}

void RockWall::Be(void)
{
    // 何もしない
}

void RockWall::OnCollision(const ColliderBase& collider)
{
    if (state_ == STATE::MOVE) {
        if (stageCollisionFlg_ == 1) { return; }
        if (collider.GetTag() == TAG::STAGE) { stageCollisionFlg_ = 1; }
        return;
    }

    switch (collider.GetTag())
    {
    case TAG::PLAYER_PUNCH:
    case TAG::PLAYER_GOUGE:
    case TAG::PLAYER_THROWING:
    case TAG::GOLEM_ATTACK_STONE:
    case TAG::GOLEM_ATTACK_FALL:
    case TAG::GOLEM_ATTACK_PSYCHOROCK: {
        ApplyBrush(255);
        GameScene::Shake();
        Smng::GetIns().Play(SOUND::OBJECT_BREAK, true, 150);
        return;
    }
    default: { return; }
    }
}