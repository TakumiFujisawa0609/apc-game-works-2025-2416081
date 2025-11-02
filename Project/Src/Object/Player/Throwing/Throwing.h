#pragma once

#include"Rock/ThrowRock.h"


enum class THROW_TYPE { NON = -1, ROCK, MAX };

class Throwing
{
public:
	static constexpr int MAX_OBJ_NUM = 20;

	Throwing(const VECTOR& playerPos, const VECTOR& playerAngle);
	~Throwing();

	void Load(void);
	void Init(void);
	void Update(void);
	void Draw(void);
	void Release(void);

	void Carry(THROW_TYPE type);
	void Drop(void);
	void Throw(void);

private:
	struct THROW_OBJ_INFO {
		ThrowObjBase* ins = nullptr;
		THROW_TYPE type = THROW_TYPE::NON;
	};
	std::vector<THROW_OBJ_INFO> throwObj_;

	int models_[(int)THROW_TYPE::MAX];

	const VECTOR& playerPos_;
	const VECTOR& playerAngle_;
};