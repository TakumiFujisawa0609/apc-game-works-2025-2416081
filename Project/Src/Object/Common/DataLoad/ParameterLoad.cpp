#include"ParameterLoad.h"

#include<sstream>
#include<fstream>
#include<iostream>

#include"../../../Utility/Utility.h"

ParameterLoad::ParameterLoad(std::string filePath) :
	parameterMap()
{
	// 指定のファイルを開く
	std::ifstream ifs = std::ifstream(filePath.c_str());

	// 例外処理 : ファイルが開けなかった場合
	if (!ifs) { std::cerr << "ParameterLoad::LoadParameter : ファイルが開けませんでした。" << std::endl; return; }

	// 1行の代入先
	std::string line;

	// 1行ずつカンマ区切りにされた状態で読み込む
	while (getline(ifs, line)) {
		// カンマごとに分けて文字列として配列に格納
		std::vector<std::string> prameterStringArray = Split(line, ',');

		// 一旦一時変数にデータを取りまとめる
		std::pair<std::string, std::vector<float>> parameterPair;
		for (size_t i = 0; i < prameterStringArray.size(); i++) {
			// 名前は添字
			if (i == PARAMETER_NAME_INDEX) { parameterPair.first = prameterStringArray[i]; }
			// それ以外は数値としてベクターに格納
			else { parameterPair.second.emplace_back(std::stof(prameterStringArray[i])); }
		}

		// 1行分のデータをマップに格納
		parameterMap.insert(parameterPair);
	}
}

const float ParameterLoad::GetParameter(std::string parameterName, int index) const { return parameterMap.at(parameterName).at(index); }

const std::vector<float>& ParameterLoad::GetParameterArray(std::string parameterName) const { return parameterMap.at(parameterName); }

void ParameterLoad::Release(void) { parameterMap.clear(); }