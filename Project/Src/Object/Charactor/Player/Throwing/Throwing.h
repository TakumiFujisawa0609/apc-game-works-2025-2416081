#pragma once

#include"Rock/ThrowRock.h"


enum class THROW_TYPE { NON = -1, ROCK, MAX };

class Throwing
{
public:

	Throwing(const Vector3& playerPos, const Vector3& playerAngle);
	~Throwing();

	void Load(void);
	void Init(void);
	void Update(void);
	void Draw(void);
	void AlphaDraw(void);
	void Release(void);

	void Carry(THROW_TYPE type);
	void Drop(void);
	void Throw(void);

	std::vector<ColliderBase*> GetCollider(void)const {
		std::vector<ColliderBase*>ret = {};

		for (const THROW_OBJ_INFO& obj : throwObj_) {
			for (ColliderBase*& collider : obj.ins->GetCollider()) { ret.emplace_back(collider); }
		}

		return ret;
	}

private:
	struct THROW_OBJ_INFO {
		ThrowObjBase* ins = nullptr;
		THROW_TYPE type = THROW_TYPE::NON;
	};

	static constexpr unsigned char BY_TYPE_NUM[(int)THROW_TYPE::MAX] = 
	{
		10, // ROCK
	};

	static constexpr short MAX_OBJ_NUM = 10;

	THROW_OBJ_INFO throwObj_[MAX_OBJ_NUM];

	int models_[(int)THROW_TYPE::MAX];

	const Vector3& playerPos_;
	const Vector3& playerAngle_;
};