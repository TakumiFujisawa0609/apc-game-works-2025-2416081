#include"Throwing.h"


Throwing::Throwing(const Vector3& playerPos, const Vector3& playerAngle) :
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
	for (THROW_OBJ_INFO& obj : throwObj_) { obj.ins->Update(); }
}

void Throwing::Draw(void)
{
	for (THROW_OBJ_INFO& obj : throwObj_) { obj.ins->Draw(); }
}

void Throwing::AlphaDraw(void)
{
	for (THROW_OBJ_INFO& obj : throwObj_) { obj.ins->AlphaDraw(); }
}

void Throwing::Release(void)
{
	for (int& id : models_) { MV1DeleteModel(id); }
	for (THROW_OBJ_INFO& obj : throwObj_) {
		if (!obj.ins) { continue; }
		obj.ins->Release();
		delete obj.ins;
	}
	throwObj_.clear();
}

void Throwing::Carry(THROW_TYPE type)
{
	for (THROW_OBJ_INFO& obj : throwObj_) {
		if (obj.ins->GetState() == ThrowObjBase::STATE::NON) {
			obj.ins->Carry();
			return;
		}
	}

	switch (type)
	{
	case THROW_TYPE::NON: { break; }
	case THROW_TYPE::ROCK:
		throwObj_.emplace_back(new ThrowRock(playerPos_, playerAngle_), type);
		break;
	}

	throwObj_.back().ins->ModelDuplicate(models_[(int)type]);
	throwObj_.back().ins->Load();
	throwObj_.back().ins->Init();
	throwObj_.back().ins->Carry();
	//Collision::AddPlayer(throwObj_.back().ins);
}

void Throwing::Drop()
{
	for (auto& obj : throwObj_) {
		if (obj.ins->GetState() == ThrowObjBase::STATE::CARRY) {
			obj.ins->Drop();
		}
	}
}
void Throwing::Throw()
{
	for (auto& obj : throwObj_) {
		if (obj.ins->GetState() == ThrowObjBase::STATE::CARRY) {
			obj.ins->Throw();
		}
	}
}