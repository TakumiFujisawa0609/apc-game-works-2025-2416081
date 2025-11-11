#pragma once

#include"../../SceneBase.h"

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
};

