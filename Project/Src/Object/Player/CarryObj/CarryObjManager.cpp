#include"CarryObjManagerh.h"
#include"../../../Manager/Collision/Collision.h"

CarryObjManager::CarryObjManager(const VECTOR& playerPos_, const VECTOR& playerAngle_) :
	playerPos_(playerPos_),
	playerAngle_(playerAngle_), 
	model_(-1),
	carryObj_()
{
}

CarryObjManager::~CarryObjManager()
{
}

void CarryObjManager::Load(void)
{
	model_ = MV1LoadModel("Data/Model/Player/ThrowingObj/Rock/Rock.mv1");
	carryObj_.reserve(3);
}

void CarryObjManager::Update(void)
{
	for (auto& obj : carryObj_) { obj->Update(); }
}

void CarryObjManager::Draw(void)
{
	for (auto& obj : carryObj_) { obj->Draw(); }
}

void CarryObjManager::Release(void)
{
	for (auto& obj : carryObj_) {
		if (!obj) { continue; }
		obj->Release();
		delete obj;
		obj = nullptr;
	}
	carryObj_.clear();
}

void CarryObjManager::On(void)
{
	for (auto& obj : carryObj_) {
		if (obj->GetState() == CarryObjBase::STATE::NON) {
			obj->On();
			return;
		}
	}

	carryObj_.emplace_back(new CarryObjBase(model_, playerPos_, playerAngle_));
	carryObj_.back()->Init();
	carryObj_.back()->On();
}
void CarryObjManager::Off(void)
{
	for (auto& obj : carryObj_) {
		if (obj->GetState() == CarryObjBase::STATE::CARRY) { obj->Off(); }
	}
}
void CarryObjManager::DropObj(void)
{
	for (auto& obj : carryObj_) {
		if (obj->GetState() == CarryObjBase::STATE::CARRY) { obj->DropObj(); }
	}
}
