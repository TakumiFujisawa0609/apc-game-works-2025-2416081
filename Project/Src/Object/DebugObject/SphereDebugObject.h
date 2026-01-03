#pragma once

#include"../ActorBase.h"

#include"../../Manager/Input/KeyManager.h"

class SphereDebugObject : public ActorBase
{
public:
	SphereDebugObject(const Vector3& cameraAngle) :ActorBase(),
		cameraAngle(cameraAngle)
	{
	}
	~SphereDebugObject()override = default;

	void Load(void)override {
		ColliderCreate(new SphereCollider(TAG::SPHERE_DEBUG_OBJECT, 50.0f, 50.0f));
	}
private:

	void SubInit(void)override { SetJudge(true); };
	void SubUpdate(void)override {
		Vector3 vec = {};

		if (KEY::GetIns().GetInfo(KEY_TYPE::DEBUG_OBJECT_FRONT).now) { vec.z++; }
		if (KEY::GetIns().GetInfo(KEY_TYPE::DEBUG_OBJECT_BACK).now) { vec.z--; }
		if (KEY::GetIns().GetInfo(KEY_TYPE::DEBUG_OBJECT_RIGHT).now) { vec.x++; }
		if (KEY::GetIns().GetInfo(KEY_TYPE::DEBUG_OBJECT_LEFT).now) { vec.x--; }
		if (KEY::GetIns().GetInfo(KEY_TYPE::DEBUG_OBJECT_UP).now) { vec.y++; }
		if (KEY::GetIns().GetInfo(KEY_TYPE::DEBUG_OBJECT_DOWN).now) { vec.y--; }

		if (vec != 0.0f) { trans_.pos += vec.TransMat(MGetRotY(cameraAngle.y)).Normalized() * 10.0f; }
	};
	void SubDraw(void)override {
		DrawSphere3D(trans_.pos.ToVECTOR(), 50.0f, 12, 0xffffff, 0xffffff, true);

		if (CollisionManager::IsStageCollision(trans_.pos, 50.0f)) { DrawString(0, 400, "true", 0xffffff); }
		else { DrawString(0, 400, "false", 0xffffff); }
	};
	void SubAlphaDraw(void)override {};
	void SubRelease(void)override {};

	const Vector3& cameraAngle;
};