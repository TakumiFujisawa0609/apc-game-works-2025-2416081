#include<DxLib.h>
#include"Application/Application.h"

//----------------
// WinMain関数
//----------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//アプリケーションクラスの実体化したインスタンスの作成
	Application::CreateIns();
	if (Application::GetIns().IsInitFail())return -1;

	//ゲーム起動
	Application::GetIns().Run();

	//解放処理
	Application::GetIns().Release();
	if (Application::GetIns().IsReleaseFail())return -1;
	Application::DeleteIns();

	return 0;		//ゲーム終了
}
