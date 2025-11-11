#pragma once

#include"../SceneBase.h"

class OverScene : public SceneBase
{
public:
	OverScene();
	~OverScene();

	void Load(void)override;
	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;
	void Release(void)override;

private:
	int img_;
};