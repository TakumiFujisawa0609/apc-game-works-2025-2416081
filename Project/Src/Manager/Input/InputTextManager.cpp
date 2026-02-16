#include"InputTextManager.h"

#include"KeyManager.h"

InputTextManager::InputTextManager() :
	inputText(),

	prevKey(),nowKey(),
	downKey(),

	INPUT_TEXT_MAX(255),

	isConversionJapanese(true)
{
}

void InputTextManager::Update(void)
{
	// 入力検出
	Input(inputText);

	// 日本語変換
	if (isConversionJapanese) { ConversionJapanese(inputText); }

	// 日本語変換切り替え
	if (Key::GetIns().GetInfo(KEY_TYPE::TEXT_INPUT_LANGUAGE_SWITCH).down) { IsConversionJapaneseSwitch(); }
}

void InputTextManager::Input(std::wstring& inputText)
{
	std::copy(std::begin(nowKey), std::end(nowKey), std::begin(prevKey));
	char key[256];
	GetHitKeyStateAll(key);
	for (short i = 0; i < 256; i++) {
		if (key[i]) {
			if (nowKey[i] >= 150) { nowKey[i] = (char)150; }
			else { nowKey[i]++; }
		}
		else { nowKey[i] = 0; }
	}
	for (unsigned short i = 0; i < 256; i++) { downKey[i] = (prevKey[i] <= 0 && nowKey[i] > 0) || (nowKey[i] >= 40); }


	if (inputText.size() < INPUT_TEXT_MAX) {

		// A〜Z
		for (int k = 0; k < (int)Alphabet::MAX; k++) {
			if (downKey[ALPHABET_INPUT_SIGNAL[k]]) { inputText.push_back(ALPHABET_CHARACTER_CODE[k]); }
		}

		// 数字
		for (int k = 0; k < (int)Numbers::MAX; k++) {
			if (downKey[NUMBERS_INPUT_SIGNAL[k]]) { inputText.push_back(NUMBERS_CHARACTER_CODE[k]); }
		}

	}

	// 消去
	if (downKey[KEY_INPUT_BACK] && !inputText.empty()) { inputText.pop_back(); }
}


void InputTextManager::ConversionJapanese(std::wstring& inputText)
{
	if (inputText.empty()) { return; }

	std::wstring newChar = {};
	newChar.push_back(inputText.back()); inputText.pop_back();
	if (
		newChar.back() == L'A' ||
		newChar.back() == L'I' ||
		newChar.back() == L'U' ||
		newChar.back() == L'E' ||
		newChar.back() == L'O'
		) {
		while (ConsonantSorting(inputText, newChar));
	}

	// 2回連続で'N'が続いていたら'ん'に変換
	if (!newChar.empty() && !inputText.empty()) {
		if (newChar.back() == L'N' && inputText.back() == L'N') {
			newChar = L"ン";
			inputText.pop_back();
		}
	}

	for (wchar_t& c : newChar) { inputText.push_back(c); }
	newChar.clear();
}

bool InputTextManager::ConsonantSorting(std::wstring& inputText, std::wstring& newChar)
{
	if (inputText.empty()) { return ConsonantToDefault(newChar); }

	switch (inputText.back())
	{
	case 'B': { return ConsonantToB(inputText, newChar); }
	case 'C': { return ConsonantToC(inputText, newChar); }
	case 'D': { return ConsonantToD(inputText, newChar); }
	case 'F': { return ConsonantToF(inputText, newChar); }
	case 'G': { return ConsonantToG(inputText, newChar); }
	case 'H': { return ConsonantToH(inputText, newChar); }
	case 'J': { return ConsonantToJ(inputText, newChar); }
	case 'K': { return ConsonantToK(inputText, newChar); }
	case 'M': { return ConsonantToM(inputText, newChar); }
	case 'N': { return ConsonantToN(inputText, newChar); }
	case 'P': { return ConsonantToP(inputText, newChar); }
	case 'Q': { return ConsonantToQ(inputText, newChar); }
	case 'R': { return ConsonantToR(inputText, newChar); }
	case 'S': { return ConsonantToS(inputText, newChar); }
	case 'T': { return ConsonantToT(inputText, newChar); }
	case 'V': { return ConsonantToV(inputText, newChar); }
	case 'W': { return ConsonantToW(inputText, newChar); }
	case 'Y': { return ConsonantToY(inputText, newChar); }
	case 'Z': { return ConsonantToZ(inputText, newChar); }

	case 'X': case 'L': { return ConsonantToXL(inputText, newChar); }

	default: { return ConsonantToDefault(newChar); }
	}
}

bool InputTextManager::ConsonantToB(std::wstring& inputText, std::wstring& newChar)
{
	bool ret = false;
	bool assimiDetection = true;

	switch (newChar.back())
	{
	case L'A': { newChar = L"バ"; break; }
	case L'I': { newChar = L"ビ"; break; }
	case L'U': { newChar = L"ブ"; break; }
	case L'E': { newChar = L"ベ"; break; }
	case L'O': { newChar = L"ボ"; break; }

	case L'ヤ': { newChar = L"ビャ"; break; }
	case L'ユ': { newChar = L"ビュ"; break; }
	case L'ヨ': { newChar = L"ビョ"; break; }

	default: { assimiDetection = false; break; }
	}

	if (assimiDetection) {
		inputText.pop_back();
		if (!inputText.empty() && inputText.back() == L'B') {
			newChar = L"ッ" + newChar;
			inputText.pop_back();
			ret = false;
		}
	}
	return ret;
}

bool InputTextManager::ConsonantToC(std::wstring& inputText, std::wstring& newChar)
{
	bool ret = false;
	bool assimiDetection = true;

	switch (newChar.back())
	{
	case L'A': { newChar = L"カ"; break; }
	case L'I': { newChar = L"シ"; break; }
	case L'U': { newChar = L"ク"; break; }
	case L'E': { newChar = L"セ"; break; }
	case L'O': { newChar = L"コ"; break; }

	case L'ヤ': { newChar = L"チャ"; break; }
	case L'ユ': { newChar = L"チュ"; break; }
	case L'ヨ': { newChar = L"チョ"; break; }

	case L'ハ': { newChar = L"チャ"; break; }
	case L'ヒ': { newChar = L"チ"; break; }
	case L'フ': { newChar = L"チュ"; break; }
	case L'ヘ': { newChar = L"チェ"; break; }
	case L'ホ': { newChar = L"チョ"; break; }

	default: { assimiDetection = false; break; }
	}

	if (assimiDetection) {
		inputText.pop_back();
		if (!inputText.empty() && inputText.back() == L'C') {
			newChar = L"ッ" + newChar;
			inputText.pop_back();
			ret = false;
		}
	}
	return ret;
}

bool InputTextManager::ConsonantToD(std::wstring& inputText, std::wstring& newChar)
{
	bool ret = false;
	bool assimiDetection = true;

	switch (newChar.back())
	{
	case L'A': { newChar = L"ダ"; ret = true; break; }
	case L'I': { newChar = L"ヂ"; ret = true; break; }
	case L'U': { newChar = L"ヅ"; ret = true; break; }
	case L'E': { newChar = L"デ"; ret = true; break; }
	case L'O': { newChar = L"ド"; ret = true; break; }

	case L'ヤ': { newChar = L"ヂャ"; break; }
	case L'ユ': { newChar = L"ヂュ"; break; }
	case L'ヨ': { newChar = L"ヂョ"; break; }

	case L'ハ': { newChar = L"デャ"; break; }
	case L'ヒ': { newChar = L"ディ"; break; }
	case L'フ': { newChar = L"デュ"; break; }
	case L'ヘ': { newChar = L"デェ"; break; }
	case L'ホ': { newChar = L"デョ"; break; }

	default: { assimiDetection = false; break; }
	}

	if (assimiDetection) {
		inputText.pop_back();
		if (!inputText.empty() && inputText.back() == L'D') {
			newChar = L"ッ" + newChar;
			inputText.pop_back();
			ret = false;
		}
	}
	return ret;
}

bool InputTextManager::ConsonantToF(std::wstring& inputText, std::wstring& newChar)
{
	bool ret = false;
	bool assimiDetection = true;

	switch (newChar.back())
	{
	case L'A': { newChar = L"ファ"; break; }
	case L'I': { newChar = L"フィ"; break; }
	case L'U': { newChar = L"フ";	break; }
	case L'E': { newChar = L"フェ"; break; }
	case L'O': { newChar = L"フォ"; break; }

	case L'ヤ': { newChar = L"フャ"; break; }
	case L'ユ': { newChar = L"フュ"; break; }
	case L'ヨ': { newChar = L"フョ"; break; }

	default: { assimiDetection = false; break; }
	}

	if (assimiDetection) {
		inputText.pop_back();
		if (!inputText.empty() && inputText.back() == L'F') {
			newChar = L"ッ" + newChar;
			inputText.pop_back();
			ret = false;
		}
	}
	return ret;
}

bool InputTextManager::ConsonantToG(std::wstring& inputText, std::wstring& newChar)
{
	bool ret = false;
	bool assimiDetection = true;

	switch (newChar.back())
	{
	case L'A': { newChar = L"ガ"; break; }
	case L'I': { newChar = L"ギ"; break; }
	case L'U': { newChar = L"グ"; break; }
	case L'E': { newChar = L"ゲ"; break; }
	case L'O': { newChar = L"ゴ"; break; }

	case L'ヤ': { newChar = L"ギャ"; break; }
	case L'ユ': { newChar = L"ギュ"; break; }
	case L'ヨ': { newChar = L"ギョ"; break; }

	default: { assimiDetection = false; break; }
	}

	if (assimiDetection) {
		inputText.pop_back();
		if (!inputText.empty() && inputText.back() == L'G') {
			newChar = L"ッ" + newChar;
			inputText.pop_back();
			ret = false;
		}
	}
	return ret;
}

bool InputTextManager::ConsonantToH(std::wstring& inputText, std::wstring& newChar)
{
	bool ret = false;
	bool assimiDetection = true;

	switch (newChar.back())
	{
	case L'A': { newChar = L"ハ"; ret = true; break; }
	case L'I': { newChar = L"ヒ"; ret = true; break; }
	case L'U': { newChar = L"フ"; ret = true; break; }
	case L'E': { newChar = L"ヘ"; ret = true; break; }
	case L'O': { newChar = L"ホ"; ret = true; break; }

	case L'ヤ': { newChar = L"ヒャ"; break; }
	case L'ユ': { newChar = L"ヒュ"; break; }
	case L'ヨ': { newChar = L"ヒョ"; break; }

	default: { assimiDetection = false; break; }
	}

	if (assimiDetection) {
		inputText.pop_back();
		if (!inputText.empty() && inputText.back() == L'H') {
			newChar = L"ッ" + newChar;
			inputText.pop_back();
			ret = false;
		}
	}
	return ret;
}

bool InputTextManager::ConsonantToJ(std::wstring& inputText, std::wstring& newChar)
{
	bool ret = false;
	bool assimiDetection = true;

	switch (newChar.back())
	{
	case L'A': { newChar = L"ジャ"; break; }
	case L'I': { newChar = L"ジ"; break; }
	case L'U': { newChar = L"ジュ"; break; }
	case L'E': { newChar = L"ジェ"; break; }
	case L'O': { newChar = L"ジョ"; break; }

	case L'ヤ': { newChar = L"ジャ"; break; }
	case L'ユ': { newChar = L"ジュ"; break; }
	case L'ヨ': { newChar = L"ジョ"; break; }

	default: { assimiDetection = false; break; }
	}

	if (assimiDetection) {
		inputText.pop_back();
		if (!inputText.empty() && inputText.back() == L'J') {
			newChar = L"ッ" + newChar;
			inputText.pop_back();
			ret = false;
		}
	}
	return ret;
}

bool InputTextManager::ConsonantToK(std::wstring& inputText, std::wstring& newChar)
{
	bool ret = false;
	bool assimiDetection = true;

	switch (newChar.back())
	{
	case L'A': { newChar = L"カ"; break; }
	case L'I': { newChar = L"キ"; break; }
	case L'U': { newChar = L"ク"; break; }
	case L'E': { newChar = L"ケ"; break; }
	case L'O': { newChar = L"コ"; break; }

	case L'ヤ': { newChar = L"キャ"; break; }
	case L'ユ': { newChar = L"キュ"; break; }
	case L'ヨ': { newChar = L"キョ"; break; }

	default: { assimiDetection = false; break; }
	}

	if (assimiDetection) {
		inputText.pop_back();
		if (!inputText.empty() && inputText.back() == L'K') {
			newChar = L"ッ" + newChar;
			inputText.pop_back();
		}
	}

	return ret;
}

bool InputTextManager::ConsonantToM(std::wstring& inputText, std::wstring& newChar)
{
	bool ret = false;
	bool assimiDetection = true;

	switch (newChar.back())
	{
	case L'A': { newChar = L"マ"; break; }
	case L'I': { newChar = L"ミ"; break; }
	case L'U': { newChar = L"ム"; break; }
	case L'E': { newChar = L"メ"; break; }
	case L'O': { newChar = L"モ"; break; }

	case L'ヤ': { newChar = L"ミャ"; break; }
	case L'ユ': { newChar = L"ミュ"; break; }
	case L'ヨ': { newChar = L"ミョ"; break; }

	default: { assimiDetection = false; break; }
	}

	if (assimiDetection) {
		inputText.pop_back();
		if (!inputText.empty() && inputText.back() == L'M') {
			newChar = L"ッ" + newChar;
			inputText.pop_back();
			ret = false;
		}
	}
	return ret;
}

bool InputTextManager::ConsonantToN(std::wstring& inputText, std::wstring& newChar)
{
	bool ret = false;
	bool assimiDetection = true;

	switch (newChar.back())
	{
	case L'A': { newChar = L"ナ"; break; }
	case L'I': { newChar = L"ニ"; break; }
	case L'U': { newChar = L"ヌ"; break; }
	case L'E': { newChar = L"ネ"; break; }
	case L'O': { newChar = L"ノ"; break; }

	case L'ヤ': { newChar = L"ニャ"; break; }
	case L'ユ': { newChar = L"ニュ"; break; }
	case L'ヨ': { newChar = L"ニョ"; break; }

	default: { assimiDetection = false; break; }
	}

	if (assimiDetection) {
		inputText.pop_back();
		if (!inputText.empty() && inputText.back() == L'N') {
			newChar = L"ッ" + newChar;
			inputText.pop_back();
			ret = false;
		}
	}
	return ret;
}

bool InputTextManager::ConsonantToP(std::wstring& inputText, std::wstring& newChar)
{
	bool ret = false;
	bool assimiDetection = true;

	switch (newChar.back())
	{
	case L'A': { newChar = L"パ"; break; }
	case L'I': { newChar = L"ピ"; break; }
	case L'U': { newChar = L"プ"; break; }
	case L'E': { newChar = L"ペ"; break; }
	case L'O': { newChar = L"ポ"; break; }

	case L'ヤ': { newChar = L"ピャ"; break; }
	case L'ユ': { newChar = L"ピュ"; break; }
	case L'ヨ': { newChar = L"ピョ"; break; }

	default: { assimiDetection = false; break; }
	}

	if (assimiDetection) {
		inputText.pop_back();
		if (!inputText.empty() && inputText.back() == L'P') {
			newChar = L"ッ" + newChar;
			inputText.pop_back();
			ret = false;
		}
	}
	return ret;
}

bool InputTextManager::ConsonantToQ(std::wstring& inputText, std::wstring& newChar)
{
	bool ret = false;
	bool assimiDetection = true;

	switch (newChar.back())
	{
	case L'A': { newChar = L"クァ"; break; }
	case L'I': { newChar = L"クィ"; break; }
	case L'U': { newChar = L"ク"; break; }
	case L'E': { newChar = L"クェ"; break; }
	case L'O': { newChar = L"クォ"; break; }

	case L'ヤ': { newChar = L"クャ"; break; }
	case L'ユ': { newChar = L"クュ"; break; }
	case L'ヨ': { newChar = L"クョ"; break; }

	default: { assimiDetection = false; break; }
	}

	if (assimiDetection) {
		inputText.pop_back();
		if (!inputText.empty() && inputText.back() == L'Q') {
			newChar = L"ッ" + newChar;
			inputText.pop_back();
			ret = false;
		}
	}
	return ret;
}

bool InputTextManager::ConsonantToR(std::wstring& inputText, std::wstring& newChar)
{
	bool ret = false;
	bool assimiDetection = true;

	switch (newChar.back())
	{
	case L'A': { newChar = L"ラ"; break; }
	case L'I': { newChar = L"リ"; break; }
	case L'U': { newChar = L"ル"; break; }
	case L'E': { newChar = L"レ"; break; }
	case L'O': { newChar = L"ロ"; break; }

	case L'ヤ': { newChar = L"リャ"; break; }
	case L'ユ': { newChar = L"リュ"; break; }
	case L'ヨ': { newChar = L"リョ"; break; }

	default: { assimiDetection = false; break; }
	}

	if (assimiDetection) {
		inputText.pop_back();
		if (!inputText.empty() && inputText.back() == L'R') {
			newChar = L"ッ" + newChar;
			inputText.pop_back();
			ret = false;
		}
	}
	return ret;
}

bool InputTextManager::ConsonantToS(std::wstring& inputText, std::wstring& newChar)
{
	bool ret = false;
	bool assimiDetection = true;

	switch (newChar.back())
	{
	case L'A': { newChar = L"サ"; break; }
	case L'I': { newChar = L"シ"; break; }
	case L'U': { newChar = L"ス"; break; }
	case L'E': { newChar = L"セ"; break; }
	case L'O': { newChar = L"ソ"; break; }

	case L'ヤ': { newChar = L"シャ"; break; }
	case L'ユ': { newChar = L"シュ"; break; }
	case L'ヨ': { newChar = L"ショ"; break; }

	case L'ハ': { newChar = L"シャ"; break; }
	case L'ヒ': { newChar = L"シ"; break; }
	case L'フ': { newChar = L"シュ"; break; }
	case L'ヘ': { newChar = L"シェ"; break; }
	case L'ホ': { newChar = L"ショ"; break; }

	default: { assimiDetection = false; break; }
	}

	if (assimiDetection) {
		inputText.pop_back();
		if (!inputText.empty() && inputText.back() == L'S') {
			newChar = L"ッ" + newChar;
			inputText.pop_back();
			ret = false;
		}
	}
	return ret;
}

bool InputTextManager::ConsonantToT(std::wstring& inputText, std::wstring& newChar)
{
	bool ret = false;
	bool assimiDetection = true;

	switch (newChar.back())
	{
	case L'A': { newChar = L"タ"; break; }
	case L'I': { newChar = L"チ"; break; }
	case L'U': { newChar = L"ツ"; ret = true; break; }
	case L'E': { newChar = L"テ"; break; }
	case L'O': { newChar = L"ト"; break; }

	case L'ヤ': { newChar = L"チャ"; break; }
	case L'ユ': { newChar = L"チュ"; break; }
	case L'ヨ': { newChar = L"チョ"; break; }

	case L'ハ': { newChar = L"テャ"; break; }
	case L'ヒ': { newChar = L"ティ"; break; }
	case L'フ': { newChar = L"テュ"; break; }
	case L'ヘ': { newChar = L"テェ"; break; }
	case L'ホ': { newChar = L"テョ"; break; }

	default: { assimiDetection = false; break; }
	}

	if (assimiDetection) {
		inputText.pop_back();
		if (!inputText.empty() && inputText.back() == L'T') {
			newChar = L"ッ" + newChar;
			inputText.pop_back();
			ret = false;
		}
	}
	return ret;
}

bool InputTextManager::ConsonantToV(std::wstring& inputText, std::wstring& newChar)
{
	bool ret = false;
	bool assimiDetection = true;

	switch (newChar.back())
	{
	case L'A': { newChar = L"ヴァ"; break; }
	case L'I': { newChar = L"ヴィ"; break; }
	case L'U': { newChar = L"ヴ"; break; }
	case L'E': { newChar = L"ヴェ"; break; }
	case L'O': { newChar = L"ヴォ"; break; }

	case L'ヤ': { newChar = L"ヴャ"; break; }
	case L'ユ': { newChar = L"ヴュ"; break; }
	case L'ヨ': { newChar = L"ヴョ"; break; }

	default: { assimiDetection = false; break; }
	}

	if (assimiDetection) {
		inputText.pop_back();
		if (!inputText.empty() && inputText.back() == L'V') {
			newChar = L"ッ" + newChar;
			inputText.pop_back();
			ret = false;
		}
	}
	return ret;
}

bool InputTextManager::ConsonantToW(std::wstring& inputText, std::wstring& newChar)
{
	bool ret = false;
	bool assimiDetection = true;

	switch (newChar.back())
	{
	case L'A': { newChar = L"ワ"; break; }
	case L'I': { newChar = L"ウィ"; break; }
	case L'U': { newChar = L"ウ"; break; }
	case L'E': { newChar = L"ウェ"; break; }
	case L'O': { newChar = L"ウォ"; break; }

	case L'ハ': { newChar = L"ウァ"; break; }
	case L'ヒ': { newChar = L"ウィ"; break; }
	case L'フ': { newChar = L"ウ"; break; }
	case L'ヘ': { newChar = L"ウェ"; break; }
	case L'ホ': { newChar = L"ウォ"; break; }

	default: { assimiDetection = false; break; }
	}

	if (assimiDetection) {
		inputText.pop_back();
		if (!inputText.empty() && inputText.back() == L'W') {
			newChar = L"ッ" + newChar;
			inputText.pop_back();
			ret = false;
		}
	}
	return ret;
}

bool InputTextManager::ConsonantToY(std::wstring& inputText, std::wstring& newChar)
{
	bool ret = false;
	bool assimiDetection = true;

	switch (newChar.back())
	{
	case L'A': { newChar = L"ヤ"; ret = true; break; }
	case L'I': { newChar = L"イ"; break; }
	case L'U': { newChar = L"ユ"; ret = true; break; }
	case L'E': { newChar = L"イェ"; break; }
	case L'O': { newChar = L"ヨ"; ret = true; break; }

	default: { assimiDetection = false; break; }
	}

	if (assimiDetection) {
		inputText.pop_back();
		if (!inputText.empty() && inputText.back() == L'Y') {
			newChar = L"ッ" + newChar;
			inputText.pop_back();
			ret = false;
		}
	}
	return ret;
}

bool InputTextManager::ConsonantToZ(std::wstring& inputText, std::wstring& newChar)
{
	bool ret = false;
	bool assimiDetection = true;

	switch (newChar.back())
	{
	case L'A': { newChar = L"ザ"; break; }
	case L'I': { newChar = L"ジ"; break; }
	case L'U': { newChar = L"ズ"; break; }
	case L'E': { newChar = L"ゼ"; break; }
	case L'O': { newChar = L"ゾ"; break; }

	case L'ヤ': { newChar = L"ジャ"; break; }
	case L'ユ': { newChar = L"ジュ"; break; }
	case L'ヨ': { newChar = L"ジョ"; break; }

	default: { assimiDetection = false; break; }
	}

	if (assimiDetection) {
		inputText.pop_back();
		if (!inputText.empty() && inputText.back() == L'Z') {
			newChar = L"ッ" + newChar;
			inputText.pop_back();
			ret = false;
		}
	}
	return ret;
}


bool InputTextManager::ConsonantToXL(std::wstring& inputText, std::wstring& newChar)
{
	bool ret = false;
	bool assimiDetection = true;

	switch (newChar.back())
	{
	case L'A': { newChar = L"ァ"; break; }
	case L'I': { newChar = L"ィ"; break; }
	case L'U': { newChar = L"ゥ"; break; }
	case L'E': { newChar = L"ェ"; break; }
	case L'O': { newChar = L"ォ"; break; }

	case L'ヤ': { newChar = L"ャ"; break; }
	case L'ユ': { newChar = L"ュ"; break; }
	case L'ヨ': { newChar = L"ョ"; break; }

	case L'ツ': { newChar = L"ッ"; break; }

	default: { assimiDetection = false; break; }
	}

	if (assimiDetection) {
		inputText.pop_back();
		if (!inputText.empty() && inputText.back() == L'Z') {
			newChar = L"ッ" + newChar;
			inputText.pop_back();
			ret = false;
		}
	}
	return ret;
}

bool InputTextManager::ConsonantToDefault(std::wstring& newChar)
{
	switch (newChar.back())
	{
	case L'A': { newChar = L"ア"; break; }
	case L'I': { newChar = L"イ"; break; }
	case L'U': { newChar = L"ウ"; break; }
	case L'E': { newChar = L"エ"; break; }
	case L'O': { newChar = L"オ"; break; }
	default: { break; }
	}
	return false;
}
