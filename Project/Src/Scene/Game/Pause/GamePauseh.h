#pragma once

#include"../../SceneBase.h"

class GamePause : public SceneBase
{
public:
	GamePause();
	~GamePause();

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
