#pragma once
#include <DxLib.h>
#include "Camera.h"

class CameraController {
public:
    void Attach(Camera* cam) { cam_ = cam; }
    void SetSpeed(float move, float rot) { moveSpeed_ = move; rotSpeedDeg_ = rot; }

    // 簡易: I/K=上下回転, J/L=左右回転, WASD移動, R/F上下, LSHIFT=倍速
    void Update() {
        if (!cam_) return;
        float r = rotSpeedDeg_;
        if (CheckHitKey(KEY_INPUT_I)) cam_->AddYawPitchDeg(0, -r);
        if (CheckHitKey(KEY_INPUT_K)) cam_->AddYawPitchDeg(0, +r);
        if (CheckHitKey(KEY_INPUT_J)) cam_->AddYawPitchDeg(-r, 0);
        if (CheckHitKey(KEY_INPUT_L)) cam_->AddYawPitchDeg(+r, 0);

        float m = moveSpeed_ * (CheckHitKey(KEY_INPUT_LSHIFT) ? 4.0f : 1.0f);
        float dx = 0, dy = 0, dz = 0;
        if (CheckHitKey(KEY_INPUT_W)) dz += +m;
        if (CheckHitKey(KEY_INPUT_S)) dz += -m;
        if (CheckHitKey(KEY_INPUT_D)) dx += +m;
        if (CheckHitKey(KEY_INPUT_A)) dx += -m;
        if (CheckHitKey(KEY_INPUT_R)) dy += +m;
        if (CheckHitKey(KEY_INPUT_F)) dy += -m;
        cam_->MoveLocal(dx, dy, dz);

        if (CheckHitKey(KEY_INPUT_LEFT))cam_->AddOrbit(-r, 0, 0);
        if (CheckHitKey(KEY_INPUT_RIGHT))cam_->AddOrbit(r, 0, 0);
        if (CheckHitKey(KEY_INPUT_RSHIFT)) {
            r *= 2.0f;
            if (CheckHitKey(KEY_INPUT_UP))cam_->AddOrbit(0, 0, -r);
            if (CheckHitKey(KEY_INPUT_DOWN))cam_->AddOrbit(0, 0, r);
        } else {
            if (CheckHitKey(KEY_INPUT_UP))cam_->AddOrbit(0, -r, 0);
            if (CheckHitKey(KEY_INPUT_DOWN))cam_->AddOrbit(0, r, 0);
        }
    }
private:
    Camera* cam_ = nullptr;
    float moveSpeed_ = 0.15f;     // 1フレームあたりの移動量（目安）
    float rotSpeedDeg_ = 1.5f;    // 1フレームあたりの回転量（度）
};