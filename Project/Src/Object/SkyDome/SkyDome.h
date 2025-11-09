#pragma once

#include"../../Utility/Utility.h"

class SkyDome
{
public:
	SkyDome();
	~SkyDome();

	void Load(void);
	void Update(void);
	void Draw(void);
	void Release(void);

	void SetPos(const VECTOR& pos) { pos_ = pos; }
private:
	int model_;
	VECTOR pos_;
	VECTOR angle_;

	const float SCALE = 50.0f;
};