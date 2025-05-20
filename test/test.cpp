
#include <iostream>
#include <windows.h>
using namespace std;

#pragma comment(lib, "winmm.lib")

DWORD WINAPI MusicThread(void* param)
{
	PlaySoundA("C:\\Users\\sdk26\\Downloads\\city-bgm-336601.WAV", NULL, SND_NODEFAULT | SND_LOOP);
	return 0;
}

void Action()
{
	while (true)
	{
		cout << ".";
		Sleep(1000);
	}
}

int main()
{
	system("title PlaySound Demo");
	CreateThread(0, 0, MusicThread, 0, 0, 0);
	Action();
}