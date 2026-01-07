#pragma once

#include"../../SceneBase.h"

#include"../../../Common/Vector2.h"

class Explanat : public SceneBase
{
public:
	Explanat();
	~Explanat()override;

	void Load(void)override;
	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;
	void Release(void)override;

private:
	int img_;
	int bottonImg_[2];
	float s_;

	const Vector2 BUTTON_POS = Vector2(1140.0f, 680.0f);

	float GetButtonExrate(void) { return abs(sinf(s_)) * 0.1f + 0.75f; }
};

