#include "Rock.h"

#include <algorithm>
#include <queue>
#include <cmath>

#include"../../Utility/Utility.h"

#include"../../Manager/Sound/SoundManager.h"

#include"../../Scene/Game/GameScene.h"

#include"../Player/Player.h"

Rock::Rock()
{
}

void Rock::SubLoad(void)
{
    Utility::LoadImg(textureId_, "Data/Model/Rock/Rock.png");

    unit_.para_.colliType = CollisionType::ENEMY;
    unit_.para_.colliShape = CollisionShape::OBB;

    float scale = 1.0f;
    unit_.para_.size = VScale(SIZE, scale);
    gridCenter_ = VScale(CENTER_DIFF, scale);
    unit_.pos_ = { 0.0f,SIZE.y/2,0.0f };

    unit_.angle_ = {};

    unit_.isAlive_ = true;

    unit_.model_ = MV1LoadModel("Data/Model/Rock/Rock.mv1");
    MV1SetPosition(unit_.model_, unit_.pos_);
    MV1SetRotationXYZ(unit_.model_, { 0.0f,0.0f,0.0f });
    MV1SetScale(unit_.model_, { scale,scale,scale });

    aliveNeedRatio_ = 0.02f;
}

void Rock::SubInit(void)
{
}

void Rock::SubUpdate(void)
{
    VECTOR vec = {};

    if (CheckHitKey(KEY_INPUT_0)) { vec.x++; }
    if (CheckHitKey(KEY_INPUT_9)) { vec.x--; }
    if (CheckHitKey(KEY_INPUT_6)) { vec.y++; }
    if (CheckHitKey(KEY_INPUT_7)) { vec.y--; }

    vec = Utility::Normalize(vec);

    unit_.pos_ = VAdd(unit_.pos_, VScale(vec, unit_.para_.speed));
}

void Rock::SubDraw(void)
{
    //DrawSphere3D(VAdd(unit_.pos_, unit_.para_.center), 10.0f, 16, GetColor(255, 0, 0), GetColor(255, 0, 0), true);

    ////デバッグ用に当たり判定の表示
    //VECTOR debugPos[8] =
    //{
    //    VAdd(unit_.pos_, VTransform({ -unit_.para_.size.x / 2.0f, -unit_.para_.size.y / 2.0f, -unit_.para_.size.z / 2.0f },Utility::MatrixAllMultY({unit_.angle_}))),
    //    VAdd(unit_.pos_, VTransform({  unit_.para_.size.x / 2.0f, -unit_.para_.size.y / 2.0f, -unit_.para_.size.z / 2.0f },Utility::MatrixAllMultY({unit_.angle_}))),
    //    VAdd(unit_.pos_, VTransform({ -unit_.para_.size.x / 2.0f,  unit_.para_.size.y / 2.0f, -unit_.para_.size.z / 2.0f },Utility::MatrixAllMultY({unit_.angle_}))),
    //    VAdd(unit_.pos_, VTransform({  unit_.para_.size.x / 2.0f,  unit_.para_.size.y / 2.0f, -unit_.para_.size.z / 2.0f },Utility::MatrixAllMultY({unit_.angle_}))),
    //    VAdd(unit_.pos_, VTransform({ -unit_.para_.size.x / 2.0f, -unit_.para_.size.y / 2.0f,  unit_.para_.size.z / 2.0f },Utility::MatrixAllMultY({unit_.angle_}))),
    //    VAdd(unit_.pos_, VTransform({  unit_.para_.size.x / 2.0f, -unit_.para_.size.y / 2.0f,  unit_.para_.size.z / 2.0f },Utility::MatrixAllMultY({unit_.angle_}))),
    //    VAdd(unit_.pos_, VTransform({ -unit_.para_.size.x / 2.0f,  unit_.para_.size.y / 2.0f,  unit_.para_.size.z / 2.0f },Utility::MatrixAllMultY({unit_.angle_}))),
    //    VAdd(unit_.pos_, VTransform({  unit_.para_.size.x / 2.0f,  unit_.para_.size.y / 2.0f,  unit_.para_.size.z / 2.0f },Utility::MatrixAllMultY({unit_.angle_})))
    //};
    //for (int i = 0; i < 8; i++) {
    //    DrawSphere3D(debugPos[i], 3.0f, 30, GetColor(255, 0, 0), GetColor(255, 0, 0), true);
    //}

    //for (auto& vPos : GetVoxelCenters()) {
    //    DrawSphere3D(vPos, GetCellSize() / 2, 30, 0xffffff, 0xffffff, true);
    //}
}

void Rock::SubRelease(void)
{
}


void Rock::OnCollision(UnitBase* other)
{
    if (dynamic_cast<PlayerPunch*>(other)) {
        if (ApplyBrush(other->GetUnit(), 200)) {
            GameScene::Shake();
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
