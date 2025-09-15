#pragma once
#include <DxLib.h>
#include <cmath>

#include"../../Object/VoxelBase.h"

#include"../../Utility/Utility.h"

class Camera {
public:
    enum class Mode { FPS, Orbit };

    struct SimpleCam {
        VECTOR pos;     // 位置
        VECTOR fwd;     // 正面(正規化)
        VECTOR right;   // 右(正規化)
        VECTOR up;      // 上(正規化)
        float tanHalfFovY;
        float tanHalfFovX;
        float nearZ, farZ;
    };

    Camera();
    ~Camera();


    // ---- 基本設定 ----
    void SetPerspective(float fovDeg, float nearZ, float farZ) { fovYDeg_ = fovDeg; near_ = nearZ; far_ = farZ; }
    void SetPosition(const VECTOR& p) { pos_ = p; }
    void SetYawPitchDeg(float yawDeg, float pitchDeg) { yawDeg_ = yawDeg; pitchDeg_ = pitchDeg; ClampPitch(); }
    void SetUpVector(const VECTOR& up) { worldUp_ = up; }
    void SetMode(Mode m) { mode_ = m; }

    // ---- FPSモード専用 ----
    // ローカル移動（右手: +Z=前, +X=右, +Y=上）
    void MoveLocal(float dx, float dy, float dz);

    // 角度を増分で回す（度）
    void AddYawPitchDeg(float dYaw, float dPitch);

    // ---- オービットモード専用 ----
    void SetOrbitTarget(const VECTOR& t) { target_ = t; }
    void SetOrbit(float yawDeg, float pitchDeg, float radius);
    void AddOrbit(float dYawDeg, float dPitchDeg, float dRadius);

    // ---- 適用（DxLibへ反映）----
    void Apply();

    // 取得系
    VECTOR GetPosition() const;
    float  GetYawDeg()   const { return yawDeg_; }
    float  GetPitchDeg() const { return pitchDeg_; }


    // AABB中心と半径で近似（球カリング＋FOVコーン）
    static bool VisibleApprox(const MeshBatch& b, const VECTOR& worldOffset, const SimpleCam& cam);

    // 画面アスペクト(例: 1280/720)を渡すとカリング用パラメータを返す
    SimpleCam GetCullingParams(float aspect) const;

    void OrbitDebugDraw(void);
private:
    // ---- 内部データ ----
    Mode mode_;

    // FPS 用
    VECTOR pos_;
    float  yawDeg_;   // +Y軸周り
    float  pitchDeg_;   // +X軸周り（上下）, 制限あり

    // Orbit 用
    VECTOR target_;
    float  orbitYawDeg_;
    float  orbitPitchDeg_;
    float  radius_;

    // 共通
    float  fovYDeg_;
    float  near_, far_;
    VECTOR worldUp_;

    struct Basis { VECTOR forward, right, up; };

    // yaw,pitch から forward/right/up を作る（右手系）
    Basis BasisFromYawPitch(float yawDeg, float pitchDeg) const;
    Basis BasisFromYawPitch() const { return BasisFromYawPitch(yawDeg_, pitchDeg_); }

    static VECTOR VAdd(const VECTOR& a, const VECTOR& b) { return { a.x + b.x,a.y + b.y,a.z + b.z }; }
    static VECTOR VSub(const VECTOR& a, const VECTOR& b) { return { a.x - b.x,a.y - b.y,a.z - b.z }; }
    static VECTOR VScale(const VECTOR& a, float s) { return { a.x * s,a.y * s,a.z * s }; }

    static VECTOR Cross(const VECTOR& a, const VECTOR& b) { return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x }; }

    void ClampPitch() { pitchDeg_ = Utility::Clamp(pitchDeg_, -89.0f, 89.0f); }
    void ClampOrbitPitch() { orbitPitchDeg_ = Utility::Clamp(orbitPitchDeg_, -89.0f, 89.0f); }
};
