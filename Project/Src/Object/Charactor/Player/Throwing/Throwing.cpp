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
#pragma region 各種モデルを読み込む
	models_[(int)THROW_TYPE::ROCK] = MV1LoadModel("Data/Model/Player/ThrowingObj/Rock/Rock.mv1");
#pragma endregion

	unsigned short num = 0;

	for (unsigned char type = 0; type < (unsigned char)THROW_TYPE::MAX; type++) {
		for (unsigned short i = 0; i < BY_TYPE_NUM[type]; i++) {

			// タイプを保持
			throwObj_[num].type = (THROW_TYPE)type;

			// タイプ別にインスタンスを生成
			switch ((THROW_TYPE)type)
			{
			case THROW_TYPE::NON: { continue; }
			case THROW_TYPE::ROCK: { throwObj_[num].ins = new ThrowRock(playerPos_, playerAngle_); break; }
			}

			num++;
		}
	}

	for (THROW_OBJ_INFO& obj : throwObj_) { 
		obj.ins->ModelDuplicate(models_[(int)obj.type]);
		obj.ins->Load(); 
	}
}

void Throwing::Init(void)
{
	for (THROW_OBJ_INFO& obj : throwObj_) { obj.ins->Init(); }
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
}

void Throwing::Carry(THROW_TYPE type)
{
	for (THROW_OBJ_INFO& obj : throwObj_) {
		if (obj.type == type &&
			obj.ins->GetState() == ThrowObjBase::STATE::NON) {

			obj.ins->Carry();

			return;
		}
	}
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