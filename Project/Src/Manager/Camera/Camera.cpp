#include"Camera.h"

#include"../../Application/Application.h"


Camera::Camera():
    mode_(Mode::FPS),

    pos_(0.0f, 2.0f, -10.0f),
    yawDeg_(0.0f),
    pitchDeg_(0.0f),

    target_(0.0f, 1.0f, 6.0f),
    orbitYawDeg_(0.0f),
    orbitPitchDeg_(-10.0f),
    radius_(10.0f),

    fovYDeg_(60.0f),

    near_(0.1f),
    far_(1000.0f),
    worldUp_(0.0f, 1.0f, 0.0f)
{
}

Camera::~Camera()
{
}

void Camera::MoveLocal(float dx, float dy, float dz)
{
    Basis b = BasisFromYawPitch();
    pos_ = VAdd(pos_, VAdd(VScale(b.right, dx), VAdd(VScale(b.up, dy), VScale(b.forward, dz))));
}

void Camera::AddYawPitchDeg(float dYaw, float dPitch)
{
    yawDeg_ += dYaw;
    pitchDeg_ += dPitch; ClampPitch();
}

void Camera::SetOrbit(float yawDeg, float pitchDeg, float radius)
{
    orbitYawDeg_ = yawDeg;
    orbitPitchDeg_ = pitchDeg;
    radius_ = radius;
    ClampOrbitPitch();
}

void Camera::AddOrbit(float dYawDeg, float dPitchDeg, float dRadius)
{
    orbitYawDeg_ += dYawDeg;
    orbitPitchDeg_ += dPitchDeg;
    radius_ += dRadius;
    if (radius_ < 0.1f) { radius_ = 0.1f; }
    ClampOrbitPitch();
}

void Camera::Apply()
{
    SetupCamera_Perspective(Utility::Deg2RadF(fovYDeg_));
    SetCameraNearFar(near_, far_);

    switch (mode_)
    {

    case Camera::Mode::FPS: {
        Basis b = BasisFromYawPitch();
        VECTOR target = VAdd(pos_, b.forward);
        SetCameraPositionAndTargetAndUpVec(pos_, target, b.up);
        break;
    }

    case Camera::Mode::Orbit: {
        Basis b = BasisFromYawPitch(orbitYawDeg_, orbitPitchDeg_);
        VECTOR eye = VAdd(target_, VScale(b.forward, -radius_)); // ターゲットを中心に見る
        SetCameraPositionAndTargetAndUpVec(eye, target_, b.up);
        break;
    }

    }
}

VECTOR Camera::GetPosition() const
{
    switch (mode_)
    {
    case Camera::Mode::FPS: { return pos_; }
    case Camera::Mode::Orbit: {
        Basis b = BasisFromYawPitch(orbitYawDeg_, orbitPitchDeg_);
        return VAdd(target_, VScale(b.forward, -radius_));
    }
    }
    return pos_;
}

bool Camera::VisibleApprox(const MeshBatch& b, const VECTOR& worldOffset, const SimpleCam& cam)
{
    VECTOR c = VScale(VAdd(b.bmin, b.bmax), 0.5f);
    VECTOR e = VScale(VSub(b.bmax, b.bmin), 0.5f);
    float radius = std::sqrt(e.x * e.x + e.y * e.y + e.z * e.z);

    VECTOR cw = VAdd(c, worldOffset);           // ワールド中心
    VECTOR v = VSub(cw, cam.pos);
    float dForward = VDot(v, cam.fwd);

    if (dForward + radius < cam.nearZ)  return false;
    if (dForward - radius > cam.farZ)   return false;

    float dRight = std::fabs(VDot(v, cam.right));
    float dUp = std::fabs(VDot(v, cam.up));

    // 視野コーン（tanθ * 前方距離 + 半径）で外側なら不可視
    if (dRight > (cam.tanHalfFovX * dForward + radius)) return false;
    if (dUp > (cam.tanHalfFovY * dForward + radius)) return false;

    return true;
}

Camera::SimpleCam Camera::GetCullingParams(float aspect) const
{
    Basis b = BasisFromYawPitch();
    float tanY = std::tan(DX_PI_F * (fovYDeg_ * 0.5f) / 180.0f);
    return { pos_, b.forward, b.right, b.up, tanY, tanY * aspect, near_, far_ };
}

void Camera::OrbitDebugDraw(void)
{
    Vector2I screen = { Application::SCREEN_SIZE_X,Application::SCREEN_SIZE_Y };

    int fontSize = 32;
    SetFontSize(fontSize);
    DrawFormatString(0, screen.y - (fontSize * 3), 0xffffff, "orbitYawDeg_:%.2f", orbitYawDeg_);
    DrawFormatString(0, screen.y - (fontSize * 2), 0xffffff, "orbitPitchDeg_:%.2f", orbitPitchDeg_);
    DrawFormatString(0, screen.y - (fontSize * 1), 0xffffff, "radius_:%.2f", radius_);
    SetFontSize(16);
}

Camera::Basis Camera::BasisFromYawPitch(float yawDeg, float pitchDeg) const
{
    float yaw = Utility::Deg2RadF(yawDeg);
    float pit = Utility::Deg2RadF(pitchDeg);
    VECTOR fwd = { std::cos(yaw) * std::cos(pit), std::sin(pit), std::sin(yaw) * std::cos(pit) };
    VECTOR upw = worldUp_;
    VECTOR right = Utility::Normalize(Cross(upw, fwd));
    VECTOR up = Utility::Normalize(Cross(fwd, right));
    return { Utility::Normalize(fwd), right, up };
}
