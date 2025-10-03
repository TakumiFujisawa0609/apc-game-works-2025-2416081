#pragma once

#include"../../UnitBase.h"

class ThrowingObjBase : public UnitBase
{
public:
	ThrowingObjBase();
	virtual ~ThrowingObjBase() = 0;

	virtual void Load(void)override;
	virtual void Init(void)override;
	virtual void Update(void)override;
	virtual void Draw(void)override;
	virtual void Release(void)override;

	virtual void OnCollision(UnitBase* other)override;

	virtual void Throw(const VECTOR& pos, const VECTOR& vec);

private:
	VECTOR moveVec_;
	int aliveCounter_;
};