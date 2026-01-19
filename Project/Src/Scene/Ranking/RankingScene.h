#pragma once

#include<vector>

#include"../../Manager/Score/Ranking.h"

#include"../SceneBase.h"

class RankingScene : public SceneBase
{
public:
	RankingScene();
	~RankingScene()override = default;

	void Load(void)override;
	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;
	void Release(void)override;

private:
	const std::vector<Ranking::RankingData>& rankingData;

	int scrollHeight;

	int font;
	const unsigned char FONT_SIZE = 80;
};