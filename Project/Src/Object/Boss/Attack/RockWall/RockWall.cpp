#include "RockWall.h"

#include <algorithm>
#include <queue>
#include <cmath>

#include"../../../../Manager/Sound/SoundManager.h"
#include"../../../../Manager/Collision/CollisionUtility.h"

#include"../../../../Scene/Game/GameScene.h"

#include"../../../Player/Player.h"
#include"../../../Stage/Block/Block.h"

RockWall::RockWall():
    state_(STATE::NON),
    stateFuncPtr(),

    nowFrameMove_(true)
{
}

void RockWall::On(const VECTOR& pos)
{
    ReVival();

    state_ = STATE::MOVE;

    unit_.pos_ = pos;
    unit_.pos_.y = 0.0f;

    unit_.isAlive_ = true;
}

void RockWall::SubLoad(void)
{
    Utility::LoadImg(textureId_, "Data/Model/Rock/Rock.png");

    unit_.para_.colliShape = CollisionShape::AABB;

    float scale = 1.0f;
    unit_.para_.size = VScale(SIZE, scale);
    gridCenter_ = VScale(CENTER_DIFF, scale);
    unit_.pos_ = { 0.0f,SIZE.y/2,0.0f };

    unit_.angle_ = {};

    unit_.model_ = MV1LoadModel("Data/Model/Rock/Rock.mv1");
    MV1SetPosition(unit_.model_, unit_.pos_);
    MV1SetRotationXYZ(unit_.model_, { 0.0f,0.0f,0.0f });
    MV1SetScale(unit_.model_, { scale,scale,scale });
    
    aliveNeedRatio_ = 0.02f;

#pragma region 関数ポインタ配列へ各関数を格納
#define SET_STATE(state, func) stateFuncPtr[(int)(state)] = static_cast<STATEFUNC>(func)
    SET_STATE(STATE::NON, &RockWall::Non);
    SET_STATE(STATE::MOVE, &RockWall::Move);
    SET_STATE(STATE::BE, &RockWall::Be);
#pragma endregion

}

void RockWall::SubInit(void)
{
    nowFrameMove_ = true;

    unit_.isAlive_ = true;
}

void RockWall::SubUpdate(void)
{
    // 現在のステートに対応する関数を実行
    (this->*stateFuncPtr[(int)state_])();
}

void RockWall::SubDraw(void)
{
}

void RockWall::SubRelease(void)
{
}

void RockWall::Move(void)
{
    if (nowFrameMove_) { nowFrameMove_ = false; }
    else { state_ = STATE::BE; }

    unit_.pos_.y += MOVE_SPEED;

    if (unit_.pos_.y > 360.0f) { state_ = STATE::BE; }
}

void RockWall::Be(void)
{
}

void RockWall::OnCollision(UnitBase* other)
{
    if (nowFrameMove_ == false) {
        if (VoxelBase* voxel = dynamic_cast<Block*>(other)) {
            for (const auto& v : voxel->GetCellCenterPoss()) {
                if (Cfunc::Obb(unit_.WorldPos(), unit_.para_.size, unit_.angle_, v, voxel->GetCellSizeVECTOR(), voxel->GetUnit().angle_)) {
                    state_ = STATE::MOVE;
                    nowFrameMove_ = true;
                    break;
                }
            }
        }
    }

    if (dynamic_cast<PlayerPunch*>(other)) {
        if (ApplyBrush(other->GetUnit(), 200)) {
            GameScene::Shake();
            Smng::GetIns().Play(SOUND::OBJECT_BREAK, true, 150);
        }
        return;
    }

    if (dynamic_cast<PlayerGouge*>(other)) {
        if (ApplyBrush(other->GetUnit(), 255)) {
            Smng::GetIns().Play(SOUND::OBJECT_BREAK, true, 150);
        }
        return;
    }

    if (dynamic_cast<ThrowObjBase*>(other)) {
        if (ApplyBrush(other->GetUnit(), 200)) {
            GameScene::Shake();
            Smng::GetIns().Play(SOUND::OBJECT_BREAK, false, 150);
        }
        return;
    }
}
