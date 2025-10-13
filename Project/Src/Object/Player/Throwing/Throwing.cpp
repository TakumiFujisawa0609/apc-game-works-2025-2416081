#include"Throwing.h"

#include"../../../Manager/Collision/Collision.h"

Throwing::Throwing(const VECTOR& playerPos, const VECTOR& playerAngle) :
	models_(),

	playerPos_(playerPos),
	playerAngle_(playerAngle)
{
}

Throwing::~Throwing()
{
}

void Throwing::Load(void)
{
	models_[(int)THROW_TYPE::ROCK] = MV1LoadModel("Data/Model/Player/ThrowingObj/Rock/Rock.mv1");

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
	for (auto& id : models_) { MV1DeleteModel(id); }
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
	case THROW_TYPE::NON: { break; }
	case THROW_TYPE::ROCK:
		throwObj_.emplace_back(new ThrowRock(), type);
		throwObj_.back().ins->ModelLoad(models_[(int)THROW_TYPE::ROCK]);
		throwObj_.back().ins->Load();
		throwObj_.back().ins->Init();
		throwObj_.back().ins->Throw(throwPos, throwVec);
		Collision::AddPlayer(throwObj_.back().ins);
		break;
	}
}