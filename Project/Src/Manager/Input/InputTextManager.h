#pragma once

#include<DxLib.h>
#include<string>

class InputTextManager
{
public:
	InputTextManager();
	~InputTextManager() = default;

	// 入力中の文字列を消去
	void Reset(void) { inputText.clear(); }

	// 入力更新
	void Update(void);

	// 入力中の文字列情報を取得
	std::wstring InputText(void)const { return inputText; }

	// 入力文字数に制限をかける
	void SetInputTextMax(unsigned char INPUT_TEXT_MAX) { this->INPUT_TEXT_MAX = INPUT_TEXT_MAX; }

	// 今日本語変換機能がオンになっているかどうか
	bool IsConversionJapanese(void) { return isConversionJapanese; }
	// 日本語変換を行うかを切り替える
	void IsConversionJapaneseSwitch(void) { isConversionJapanese = !isConversionJapanese; }

private:
	// 入力中の文字列情報
	std::wstring inputText;

	char prevKey[256], nowKey[256];
	bool downKey[256];

	unsigned char INPUT_TEXT_MAX;

	// 入力を検出し、文字列に格納していく
	void Input(std::wstring& inputText);

	// 入力受付アルファベット列挙
	enum class Alphabet { A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z, MAX };
	// 入力受付アルファベットの各入力信号
	const int ALPHABET_INPUT_SIGNAL[(int)Alphabet::MAX] =
	{
		KEY_INPUT_A, // A
		KEY_INPUT_B, // B
		KEY_INPUT_C, // C
		KEY_INPUT_D, // D
		KEY_INPUT_E, // E
		KEY_INPUT_F, // F
		KEY_INPUT_G, // G
		KEY_INPUT_H, // H
		KEY_INPUT_I, // I
		KEY_INPUT_J, // J
		KEY_INPUT_K, // K
		KEY_INPUT_L, // L
		KEY_INPUT_M, // M
		KEY_INPUT_N, // N
		KEY_INPUT_O, // O
		KEY_INPUT_P, // P
		KEY_INPUT_Q, // Q
		KEY_INPUT_R, // R
		KEY_INPUT_S, // S
		KEY_INPUT_T, // T
		KEY_INPUT_U, // U
		KEY_INPUT_V, // V
		KEY_INPUT_W, // W
		KEY_INPUT_X, // X
		KEY_INPUT_Y, // Y
		KEY_INPUT_Z, // Z
	};
	// 入力受付アルファベットの各文字コード
	const char ALPHABET_CHARACTER_CODE[(int)Alphabet::MAX] = { 'A','B','C',	'D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z' };

	// 入力受付数字列挙
	enum class Numbers { ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, MAX };
	// 入力受付数字の各入力信号
	const int NUMBERS_INPUT_SIGNAL[(int)Numbers::MAX] =
	{
		KEY_INPUT_0, // 0
		KEY_INPUT_1, // 1
		KEY_INPUT_2, // 2
		KEY_INPUT_3, // 3
		KEY_INPUT_4, // 4
		KEY_INPUT_5, // 5
		KEY_INPUT_6, // 6
		KEY_INPUT_7, // 7
		KEY_INPUT_8, // 8
		KEY_INPUT_9, // 9
	};
	// 入力受付数字の各文字コード
	const char NUMBERS_CHARACTER_CODE[(int)Numbers::MAX] = { '0','1','2','3','4','5','6','7','8','9', };


#pragma region 日本語入力に関すること
	// 入力中の文字列情報をローマ字入力基準での日本語変換を行うかどうか（true = 行い日本語とアルファベットが混在した状態、false = 行わず全てアルファベットのまま）
	bool isConversionJapanese;

	// 文字列の日本語変換を行う
	void ConversionJapanese(std::wstring& inputText);

	// 文字変換の振り分け
	bool ConsonantSorting(std::wstring& inputText, std::wstring& newChar);

#pragma region 各子音の変換
	// B行
	bool ConsonantToB(std::wstring& inputText, std::wstring& newChar);

	// C行
	bool ConsonantToC(std::wstring& inputText, std::wstring& newChar);

	// D行
	bool ConsonantToD(std::wstring& inputText, std::wstring& newChar);

	// F行
	bool ConsonantToF(std::wstring& inputText, std::wstring& newChar);

	// G行
	bool ConsonantToG(std::wstring& inputText, std::wstring& newChar);

	// H行
	bool ConsonantToH(std::wstring& inputText, std::wstring& newChar);

	// J行
	bool ConsonantToJ(std::wstring& inputText, std::wstring& newChar);

	// K行
	bool ConsonantToK(std::wstring& inputText, std::wstring& newChar);

	// M行
	bool ConsonantToM(std::wstring& inputText, std::wstring& newChar);

	// N行
	bool ConsonantToN(std::wstring& inputText, std::wstring& newChar);

	// P行
	bool ConsonantToP(std::wstring& inputText, std::wstring& newChar);

	// Q行
	bool ConsonantToQ(std::wstring& inputText, std::wstring& newChar);

	// R行
	bool ConsonantToR(std::wstring& inputText, std::wstring& newChar);

	// S行
	bool ConsonantToS(std::wstring& inputText, std::wstring& newChar);

	// T行
	bool ConsonantToT(std::wstring& inputText, std::wstring& newChar);

	// V行
	bool ConsonantToV(std::wstring& inputText, std::wstring& newChar);

	// W行
	bool ConsonantToW(std::wstring& inputText, std::wstring& newChar);

	// Y行
	bool ConsonantToY(std::wstring& inputText, std::wstring& newChar);

	// Z行
	bool ConsonantToZ(std::wstring& inputText, std::wstring& newChar);


	// XL行（小文字変換）
	bool ConsonantToXL(std::wstring& inputText, std::wstring& newChar);

	// デフォルト
	bool ConsonantToDefault(std::wstring& newChar);
#pragma endregion

#pragma endregion


};