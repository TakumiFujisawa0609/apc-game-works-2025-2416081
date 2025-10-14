#pragma once

#include"../SceneBase.h"

class ClearScene : public SceneBase
{
public:
	ClearScene();
	~ClearScene()override;

	void Load(void)override;
	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;
	void Release(void)override;

private:
	int img_;
};
