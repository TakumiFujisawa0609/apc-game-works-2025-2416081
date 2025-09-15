#include<DxLib.h>
#include"Application/Application.h"

//----------------
// WinMain関数
//----------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//アプリケーションクラスの実体化したインスタンスの作成
	Application::CreateInstance();
	if (Application::GetInstance().IsInitFail())return -1;

	//ゲーム起動
	Application::GetInstance().Run();

	//解放処理
	Application::GetInstance().Release();
	if (Application::GetInstance().IsReleaseFail())return -1;
	Application::DeleteInstance();

	return 0;		//ゲーム終了
}
