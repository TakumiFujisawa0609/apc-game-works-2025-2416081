#include"Throwing.h"


Throwing::Throwing(const int& playerModel, const Vector3& playerAngle) :
	models(),

	playerModel(playerModel),
	playerAngle(playerAngle)
{
}

void Throwing::Load(void)
{
#pragma region 各種モデルを読み込む
	models[(int)THROW_TYPE::ROCK] = MV1LoadModel("Data/Model/Player/ThrowingObj/Rock/Rock.mv1");
#pragma endregion

	unsigned short num = 0;

	for (unsigned char type = 0; type < (unsigned char)THROW_TYPE::MAX; type++) {
		for (unsigned short i = 0; i < BY_TYPE_NUM[type]; i++) {

			// タイプを保持
			throwObj[num].type = (THROW_TYPE)type;

			// タイプ別にインスタンスを生成
			switch ((THROW_TYPE)type)
			{
			case THROW_TYPE::NON: { continue; }
			case THROW_TYPE::ROCK: { throwObj[num].ins = new ThrowRock(playerModel, playerAngle); break; }
			}

			num++;
		}
	}

	for (THROW_OBJ_INFO& obj : throwObj) { 
		obj.ins->ModelDuplicate(models[(int)obj.type]);
		obj.ins->Load(); 
	}
}

void Throwing::Init(void)
{
	for (THROW_OBJ_INFO& obj : throwObj) { obj.ins->Init(); }
}

void Throwing::Update(void)
{
	for (THROW_OBJ_INFO& obj : throwObj) { obj.ins->Update(); }
}

void Throwing::Draw(void)
{
	for (THROW_OBJ_INFO& obj : throwObj) { obj.ins->Draw(); }
}

void Throwing::AlphaDraw(void)
{
	for (THROW_OBJ_INFO& obj : throwObj) { obj.ins->AlphaDraw(); }
}

void Throwing::Release(void)
{
	for (int& id : models) { MV1DeleteModel(id); }
	for (THROW_OBJ_INFO& obj : throwObj) {
		if (!obj.ins) { continue; }
		obj.ins->Release();
		delete obj.ins;
	}
}

void Throwing::Carry(THROW_TYPE type)
{
	for (THROW_OBJ_INFO& obj : throwObj) {
		if (obj.type == type &&
			obj.ins->GetState() == ThrowObjBase::STATE::NON) {

			obj.ins->Carry();

			return;
		}
	}
}

void Throwing::Drop()
{
	for (auto& obj : throwObj) {
		if (obj.ins->GetState() == ThrowObjBase::STATE::CARRY) {
			obj.ins->Drop();
		}
	}
}
void Throwing::Throw()
{
	for (auto& obj : throwObj) {
		if (obj.ins->GetState() == ThrowObjBase::STATE::CARRY) {
			obj.ins->Throw();
		}
	}
}