#include"Throwing.h"

Throwing::Throwing(const VECTOR& playerPos, const VECTOR& playerAngle) :
	playerPos_(playerPos),
	playerAngle_(playerAngle)
{
}

Throwing::~Throwing()
{
}

void Throwing::Init(void)
{
	throwObj_.reserve(MAX_OBJ_NUM);
}

void Throwing::Update(void)
{
	for (auto& obj : throwObj_) { obj.ins->Update(); }
}

void Throwing::Draw(void)
{
	for (auto& obj : throwObj_) { obj.ins->Draw(); }
}

void Throwing::Release(void)
{

	for (auto& obj : throwObj_) {
		if (!obj.ins) { continue; }
		obj.ins->Release();
		delete obj.ins;
	}
	throwObj_.clear();
}

void Throwing::Throw(THROW_TYPE type)
{
	VECTOR throwPos = VAdd(playerPos_, VTransform(LOCAL_THROW_POS, MGetRotY(playerAngle_.y)));
	VECTOR throwVec = VTransform(THROW_VEC, MGetRotY(playerAngle_.y));

	for (auto& obj : throwObj_) {
		if (obj.type == type && obj.ins->GetUnit().isAlive_ == false) {
			obj.ins->Throw(throwPos, throwVec);
			return;
		}
	}

	switch (type)
	{
	case THROW_TYPE::NON:break;
	case THROW_TYPE::ROCK:
		throwObj_.emplace_back(new ThrowingRock(), type);
		throwObj_.end()->ins->Load();
		throwObj_.end()->ins->Init();
		throwObj_.end()->ins->Throw(throwPos, throwVec);
		break;
	}
}