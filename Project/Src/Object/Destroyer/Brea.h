#pragma once
#include"../UnitBase.h"

class Brea : public UnitBase
{
public:
	Brea();
	~Brea()override;

	void Load(void)override;
	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;
	void Release(void)override;

	void OnCollision(UnitBase* other)override {};

private:

};

