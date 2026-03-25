#pragma once

#include<map>
#include<vector>
#include<string>

class ParameterLoad
{
public:
	// コンストラクタ / ファイルパスを引数にして、パラメーターを読み込む
	ParameterLoad(std::string filePath);
	// デストラクタ
	~ParameterLoad() = default;

	// パラメーターを取得する関数
	const float GetParameter(std::string parameterName, int index = 0) const;
	// パラメーターを取得する関数
	const std::vector<float>& GetParameterArray(std::string parameterName) const;

	// パラメーターを解放する関数
	void Release(void);

private:

	// 1行のパラメーターにおいて名前(添字)に当たる番号
	const char PARAMETER_NAME_INDEX = 0;

	// パラメーターを保存するマップ
	std::map<std::string, std::vector<float>> parameterMap;
};