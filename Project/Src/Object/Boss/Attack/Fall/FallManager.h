#pragma once

#include<vector>
#include"Fall.h"

class FallManager
{
public:
	FallManager(const VECTOR& playerPos);
	~FallManager();

	void Load(void);
	void Init(void);
	void Update(void);
	void Draw(void);
	void Release(void);

	void On(void);

private:
	int model;
	std::vector<Fall*>falls_;

	const VECTOR LOCAL_POS = { 0.0f,500.0f,0.0f };

	const VECTOR& playerPos;
};