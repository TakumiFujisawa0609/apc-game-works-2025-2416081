#pragma once

#include"CarryObjBaseh.h"
#include<vector>

class CarryObjManager
{
public:
	CarryObjManager(const VECTOR& playerPos_, const VECTOR& playerAngle_);
	~CarryObjManager();

	void Load(void);
	void Update(void);
	void Draw(void);
	void Release(void);

	void On(void);
	void Off(void);
	void DropObj(void);

private:
	int model_;

	std::vector<CarryObjBase*> carryObj_;
	const VECTOR& playerPos_;
	const VECTOR& playerAngle_;
};
