#pragma once

#include"../../UnitBase.h"

class ThrowObjBase : public UnitBase
{
public:
	ThrowObjBase();
	virtual ~ThrowObjBase() = 0;

	void ModelLoad(int model);
	virtual void Load(void)override;
	virtual void Init(void)override;
	virtual void Update(void)override;
	virtual void Draw(void)override;
	virtual void Release(void)override;

	virtual void OnCollision(UnitBase* other)override;

	virtual void Throw(const VECTOR& pos, const VECTOR& vec);

protected:
	VECTOR moveVec_;
	int aliveTime;
	int aliveCounter_;

	int ALIVE_HIT_NUM;
	int aliveHitCou_;
};