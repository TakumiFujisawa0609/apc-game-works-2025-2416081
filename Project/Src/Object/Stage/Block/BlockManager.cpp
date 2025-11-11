#include <string>
#include<fstream>
#include "../../../Application/Application.h"
#include"../../../Utility/Utility.h"
#include "Block.h"
#include "BlockManager.h"

BlockManager::BlockManager(void):
	textureId_(-1)
{
}
BlockManager::~BlockManager(void)
{
}

void BlockManager::Load(void)
{
	Utility::LoadImg(textureId_, "Data/Model/Rock/Rock.png");

	// 各種ブロックモデルのロード
	std::string PATH = "Data/Model/StageBlocks/";

	//models_[0] = MV1LoadModel((PATH + "Block_Grass.mv1").c_str());
	models_[0] = MV1LoadModel((PATH + "Block.mv1").c_str());
	//models_[1] = MV1LoadModel((PATH + "Block_Metal.mv1").c_str());
	models_[1] = MV1LoadModel((PATH + "Block.mv1").c_str());

	LoadMapCsvData();
}
void BlockManager::Update(void)
{
	for (auto& b : blocks_) { b->Update(); }

}
void BlockManager::Draw(void)
{
	// マップの描画
	for (auto& b : blocks_) { b->Draw(); }
}

void BlockManager::Release(void)
{
	// モデルのメモリ解放
	for (auto& b : blocks_) {
		if (!b) { continue; }
		b->Release(); 
		delete b;
	}
	blocks_.clear();

	for (auto& b : models_) {
		if (b.second == -1)continue;
		MV1DeleteModel(b.second);
	}

	DeleteGraph(textureId_);
}

const std::vector<UnitBase*> BlockManager::GetBlocks(void) const
{
	std::vector<UnitBase*> ret = {};

	ret.reserve(blocks_.size());

	for (auto& b : blocks_) { ret.emplace_back(b); }

	return ret;
}



void BlockManager::LoadMapCsvData(void)
{
	// ファイルの読込
	std::ifstream ifs = std::ifstream("Data/MapData/MapData1.ply");
	if (!ifs) { return; }

	// ファイルを１行ずつ読み込む

	std::string line; // 1行の文字情報

	std::vector<std::string> strSplit; // 1行を1文字の動的配列に分割

	int chipNo = 0;

	int lineCount = 0;

	while (getline(ifs, line))
	{
		lineCount++;
		if (lineCount <= 11) { continue; }

		// １行をカンマ区切りで分割
		strSplit = Utility::Split(line, ' ');

		int x = std::stoi(strSplit[0]);
		int z = std::stoi(strSplit[1]);
		int y = std::stoi(strSplit[2]);

		Block* block = new Block();
		block->Create((Block::TYPE)0, models_[0], textureId_, x, y, z);
		block->Load();
		block->Init();

		blocks_.emplace_back(block);
	}
}

void BlockManager::SetCamera(Camera* c)
{
	for (auto& b : blocks_) { b->SetCamera(c); }
}

void BlockManager::StageRevival(void)
{
	blocks_.at(382)->ReVival();
	blocks_.at(383)->ReVival();
	blocks_.at(384)->ReVival();
	blocks_.at(385)->ReVival();
	blocks_.at(399)->ReVival();
	blocks_.at(400)->ReVival();
	blocks_.at(401)->ReVival();
	blocks_.at(402)->ReVival();
	blocks_.at(418)->ReVival();
	blocks_.at(419)->ReVival();
	blocks_.at(420)->ReVival();
	blocks_.at(421)->ReVival();
	blocks_.at(438)->ReVival();
	blocks_.at(439)->ReVival();
	blocks_.at(440)->ReVival();
	blocks_.at(441)->ReVival();
}
