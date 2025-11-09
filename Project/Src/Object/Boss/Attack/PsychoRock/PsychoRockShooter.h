#pragma once

#include<vector>
#include"PsychoRock.h"

class PsychoRockShooter
{
public:
	PsychoRockShooter();
	~PsychoRockShooter();

	void Load(void);
	void Init(void);
	void Update(void);
	void Draw(void);
	void Release(void);

private:
	std::vector<PsychoRock*>rocks_;

};