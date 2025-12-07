#pragma once

#include"Fall.h"

class FallManager
{
public:
	FallManager(const Vector3& playerPos);
	~FallManager();

	void Load(void);
	void Init(void);
	void Update(void);
	void Draw(void);
	void AlphaDraw(void);
	void Release(void);

	void Set(void);
	void On(void);

private:
	const Vector3 LOCAL_POS = { 0.0f,500.0f,0.0f };

	static constexpr unsigned char FALL_NUM_MAX = 3;

	int model;
	Fall* falls_[FALL_NUM_MAX];


	const Vector3& playerPos;
};