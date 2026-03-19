#pragma once

#include<map>
#include<vector>
#include<string>

class ParameterLoad
{
public:
	// コンストラクタ
	ParameterLoad();
	// デストラクタ
	~ParameterLoad() = default;

	// パラメーターをファイルから読み込む関数
	void LoadParameter(std::string filePath);
	// パラメーターを取得する関数
	const std::vector<float>& GetParameter(std::string parameterName) const;
	// パラメーターを解放する関数
	void Release(void);

private:

	// 1行のパラメーターにおいて名前(添字)に当たる番号
	const char PARAMETER_NAME_INDEX = 0;

	// パラメーターを保存するマップ
	std::map<std::string, std::vector<float>> parameterMap;
};