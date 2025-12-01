#pragma once

#include"../ActorBase.h"

class SkyDome : public ActorBase
{
public:
	SkyDome() {}
	~SkyDome() {}

	void Load(void)override;
	void Init(void)override {}
	void Update(void)override;
	void Draw(void)override;

	void SetPos(const Vector3& pos) { trans_.pos = pos; }
private:
	// •\Ž¦‚·‚é‘å‚«‚³
	const float SCALE = 50.0f;

	void SubInit(void)override{}
	void SubUpdate(void)override{}
	void SubDraw(void)override{}
	void SubRelease(void)override {}
};