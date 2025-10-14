#pragma once
#include"../../SceneBase.h"
class EndScene : public SceneBase
{
public:
	EndScene();
	~EndScene();

	void Load(void)override;
	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

private:

	enum class SELECT { YES, NO, MAX };
	SELECT nowSelect_;

	int img_[(int)SELECT::MAX];
};

