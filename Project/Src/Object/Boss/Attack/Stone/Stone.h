#pragma once

#include"../../../UnitBase.h"

class Stone : public UnitBase
{
public:
	Stone();
	~Stone();

	void ModelLoad(int model) { unit_.model_ = MV1DuplicateModel(model); }
	void Load(void)override;
	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;
	void Release(void)override;

	void On(const VECTOR& pos, const VECTOR& moveVec);

	void OnCollision(UnitBase* other)override;

private:

	VECTOR moveVec_;

};
