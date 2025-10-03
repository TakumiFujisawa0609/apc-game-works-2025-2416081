#pragma once

#include"Rock/ThrowingRock.h"


enum class THROW_TYPE { NON = -1, ROCK, MAX };

class Throwing
{
public:
	static constexpr int MAX_OBJ_NUM = 20;

	Throwing(const VECTOR& playerPos, const VECTOR& playerAngle);
	~Throwing();

	void Init(void);
	void Update(void);
	void Draw(void);
	void Release(void);

	void Throw(THROW_TYPE type);

private:
	struct THROW_OBJ_INFO {
		ThrowingObjBase* ins = nullptr;
		THROW_TYPE type = THROW_TYPE::NON;
	};
	std::vector<THROW_OBJ_INFO> throwObj_;

	const VECTOR& playerPos_;
	const VECTOR LOCAL_THROW_POS = { 0.0f,50.0f,100.0f };
	const VECTOR& playerAngle_;
	const VECTOR THROW_VEC = { 0.0f,0.0f,1.0f };
};