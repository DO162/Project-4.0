#include <iostream> // ����������� ���������� ��� ������ � ��������
#include <windows.h> // ����������� ���������� ��� ������ � WinAPI
#include <conio.h> // ����������� ���������� ��� ������ � �����������
#pragma comment(lib, "winmm.lib")
using namespace std;

enum Color : short { // ����� ������
	BLACK, DARKBLUE, DARKGREEN, TURQUOISE, DARKRED,
	PURPLE, DARKYELLOW, GREY, DARKGREY, BLUE, GREEN,
	CYAN, RED, PINK, YELLOW, WHITE
};

enum Key : short { // ���� ������
	LEFT = 75, RIGHT = 77, UP = 72, DOWN = 80,
	ENTER = 13, SPACE = 32, ESCAPE = 27, BACKSPACE = 8
};

struct SNAKE_CHARS { // ��������� ��� �������� �������� "������" � "������"
	char snake;
	char apple;
	char head;
};

struct GAME_FIELD { // ��������� ��� �������� ���������� �������� ����
	int width;
	int height;
};


void MUSIC(); // ������ ������
void MUSIC_END(); // ���������� ������
void APPLE_SETUP(const GAME_FIELD& field, int* X_apple, int* Y_apple, int* array_X, int* array_Y, int* length); // ��������� ��������� "������"
void DRAWING_FRAME(const GAME_FIELD& field, HANDLE h); // ��������� ����� ����
void INFO_PANEL(HANDLE h, int max_length, int length, const GAME_FIELD& field); // ��������� �������������� ������
void UPDATE_LENGTH_APPLE(HANDLE h, int length, const GAME_FIELD& field); // ���������� ����� "������" � �������������� ������
void DRAWING_APPLE_SNAKE(HANDLE h, int X_apple, int Y_apple, int array_X[], int array_Y[], const SNAKE_CHARS& chars); // ��������� "������" � "������"
void MAIN_CYCLE(const int max_length, const SNAKE_CHARS& chars, int sleep_time, int dx, int dy, int* array_X, int* array_Y, int* length, const GAME_FIELD& field, HANDLE h, int* X_apple, int* Y_apple); // �������� ���� ����
void GAME_OVER(HANDLE h); // �������� "������"


void MUSIC() // ������ ������
{
	auto MusicThread = [](LPVOID param) -> DWORD {
		PlaySoundA("C:\\Users\\sdk26\\Downloads\\city-bgm-336601.WAV", NULL, SND_ASYNC | SND_LOOP);
		return 0;
		};

	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MusicThread, 0, 0, 0); // �������� ������ ��� ������
}

void MUSIC_END()
{
	auto MusicThread = [](LPVOID param) -> DWORD {
		PlaySoundA("C:\\Users\\sdk26\\Downloads\\igra-ne-proidena.WAV", NULL, SND_NODEFAULT | SND_LOOP);
		return 0;
		};

	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MusicThread, 0, 0, 0);
}

void APPLE_SETUP(const GAME_FIELD& field, int* X_apple, int* Y_apple, int* array_X, int* array_Y, int* length) { // ��������� ��������� "������"
	do // ���� ������ ���������� ������ �������� ������� - �� ����� ��� �� ��������� �� "�������"
	{
		*X_apple = rand() % (field.width - 2) + 1; // ��������� ��������� "������" �� ��� �������
		*Y_apple = rand() % (field.height - 2) + 1; // ��������� ��������� "������" �� ��� �������
	} while (*X_apple != array_X[*length - 1] && *Y_apple != array_Y[*length - 1]); // �������� �� ���������� � "�������"
}

void DRAWING_FRAME(const GAME_FIELD& field, HANDLE h) { // ��������� ����� ����
	SetConsoleTextAttribute(h, 4); // ��������� �����, ������� �������� ����� ����
	for (int y = 0; y < field.height; y++) // ����������� ������� ���� �� ��������� �����
	{
		for (int x = 0; x < field.width; x++)
		{
			char s;
			if (x == 0 && y == 0) // � ������� ����� ���� ���� - ������ ���������������� ����
				s = '#'; // 218
			else if (x == 0 && y == field.height - 1) // ������ ����� ����
				s = '#'; // 192
			else if (y == 0 && x == field.width - 1) // ������� ������ ����
				s = '#'; // 191
			else if (y == field.height - 1 && x == field.width - 1) // ������ ������ ����
				s = '#'; // 217
			else if (y == 0 || y == field.height - 1) // ������� � ������ ������� ����
				s = '#'; // 196
			else if (x == 0 || x == field.width - 1) // ����� � ������ ������� ����
				s = '#'; // 179
			else s = ' '; // �� ���� ��������� ������� ������ ���� ������ ������ (�������� ������ ������� ����)
			cout << s; // ������� ������
		}
		cout << "\n";
	}
}

void INFO_PANEL(HANDLE h, int max_length, int length, const GAME_FIELD& field) { // ��������� �������������� ������
	COORD info_panel; // �������� ������� ��� �������� ��������� �������������� ������

	SetConsoleOutputCP(1251); // ��������� ������� �������� ��� ����������� ���������
	info_panel.X = field.width + 5;
	info_panel.Y = 0;
	SetConsoleCursorPosition(h, info_panel);
	SetConsoleTextAttribute(h, Color::RED);
	cout << "������������ ����� ������: ";
	SetConsoleTextAttribute(h, Color::WHITE);
	cout << max_length; // ������� ������������ ����� "������"

	info_panel.Y = 2; // ��������� ��������� ��� ������ ������� ����� "������"
	SetConsoleCursorPosition(h, info_panel);
	SetConsoleTextAttribute(h, Color::RED);
	cout << "�������� ����� ������: ";
	SetConsoleTextAttribute(h, Color::WHITE);
	cout << length; // ������� ������� ����� "������"

	info_panel.Y = 4; // ��������� ��������� ��� ������ ������� ����� "������"
	SetConsoleCursorPosition(h, info_panel);
	SetConsoleTextAttribute(h, Color::RED);
	cout << "���������� �������� �����: ";
	SetConsoleTextAttribute(h, Color::WHITE);
	cout << length - 1; // ������� ������� ����� "������"
	SetConsoleOutputCP(866);
}

void UPDATE_LENGTH_APPLE(HANDLE h, int length, const GAME_FIELD& field) { // ���������� ����� "������" � �������������� ������
	COORD info_panel; // �������� ������� ��� �������� ��������� �������������� ������

	info_panel.X = field.width + 5; // ��������� ��������� ��� ������ ������� ����� "������"
	info_panel.Y = 2;
	SetConsoleCursorPosition(h, info_panel);
	SetConsoleOutputCP(1251);
	SetConsoleTextAttribute(h, Color::RED);
	cout << "�������� ����� ������: ";
	SetConsoleTextAttribute(h, Color::WHITE);
	cout << length; // ������� ������� ����� "������"
	SetConsoleTextAttribute(h, Color::GREEN);

	info_panel.Y = 4; // ��������� ��������� ��� ������ ���������� ��������� "�����"
	SetConsoleCursorPosition(h, info_panel);
	SetConsoleTextAttribute(h, Color::RED);
	cout << "���������� �������� �����: ";
	SetConsoleTextAttribute(h, Color::WHITE);
	cout << length - 1; // ������� ���������� ��������� "�����"
	SetConsoleOutputCP(866);
}

void DRAWING_APPLE_SNAKE(HANDLE h, int X_apple, int Y_apple, int array_X[], int array_Y[], const SNAKE_CHARS& chars) {
	COORD c; // �������� ������� ��� �������� ���������
	c.X = X_apple; // ��������� ������ ��������� � ��������� "������"
	c.Y = Y_apple;
	SetConsoleCursorPosition(h, c); // ���������� ������ �� ������� "������"
	SetConsoleTextAttribute(h, Color::RED); // ������������� ������� ���� ��� ��������� "������"
	cout << chars.apple; // ���������� ������ "������"

	c.X = array_X[0]; // ��������� ������ ��������� �� ��������� �������� "������"
	c.Y = array_Y[0];
	SetConsoleCursorPosition(h, c); // ���������� ������ �� ������� ������ "������"
	SetConsoleTextAttribute(h, Color::GREEN); // ������������� ������� ���� ��� ��������� "������"
	cout << chars.head; // ���������� ������ ������ "������"
}

void MAIN_CYCLE(const int max_length, const SNAKE_CHARS& chars, int sleep_time, int dx, int dy, int* array_X, int* array_Y, int* length,
	const GAME_FIELD& field, HANDLE h, int* X_apple, int* Y_apple) { // �������� ���� ����
	COORD c; // �������� ������� ��� �������� ���������
	COORD info_panel; // �������� ������� ��� �������� ��������� �������������� ������

	bool flag = true; // ���������� ��� ���������� ����� �����

	do // ���������� ���� ����
	{
		Sleep(sleep_time); // �������� ������ ��������� �� �������� ����� ��������

		if (_kbhit()) // ���������, ���� �� ������ �����-���� ������� � ��������� � ��������� � ������ ������
		{
			int k = _getch(); // ��������� ��� ������� �� ������
			if (k == 0 || k == 224) // ���� ������ ��� - ���������������, ��������� ������ ���
				k = _getch();
			switch (k) // ���������� ��� ������� ������� ������ ��������� ������
			{
			case Key::DOWN: // ���� ���� ������ ������� ����
				dy = 1; // �� ���������� �� ��� ������� ����� �������������
				dx = 0; // �� ��� ������� ���������� �������
				break;
			case Key::UP: // ���� �����
				dy = -1; // ���������� �������� �������������� ������
				dx = 0;
				break;
			case Key::LEFT: // ���� �����
				dy = 0;
				dx = -1;
				break;
			case Key::RIGHT: // ���� ������
				dy = 0;
				dx = 1;
				break;
			case Key::ESCAPE: // ���� ���� ������ ������� ESC
				flag = false; // ������������� ������ �� ����, ���� ��������� ����� ��������
				break;
			}
		}

		int X = array_X[*length - 1] + dx; // ���������� �������� �������� ������ "������" ����� ��������
		int Y = array_Y[*length - 1] + dy; // �� �� ����� ��� ��������
		if (X == 0 || X == field.width - 1 || Y == 0 || Y == field.height - 1) // �������� �� ���������� ������ ����
		{
			flag = false; // ���� ��� - ������ ��������� ����������� ���������� �����
		}
		else if (X == *X_apple && Y == *Y_apple) // �������� �� ���������� "������"
		{
			c.X = array_X[*length - 1]; // ��������� � ������ ��������� ������� ������ "������"
			c.Y = array_Y[*length - 1];
			SetConsoleCursorPosition(h, c); // ��������� ������� � ��� �������
			cout << chars.snake; // ����������� ������� ���� "������"

			(*length)++; // ���������� ����� "������" (������ ����������)

			UPDATE_LENGTH_APPLE(h, *length, field);

			c.X = array_X[*length - 1] = X; // ��������� � ������� ������� ������ ����� "������"
			c.Y = array_Y[*length - 1] = Y;
			SetConsoleCursorPosition(h, c); // ��������� ���� �������
			cout << chars.head; // � ����������� ��� ������� ������ "������"

			if (*length == max_length) // ��������, �������� �� ����� "������" ������ ������������� ��������
			{
				break; // ���� ��� - ������ ��������� ���� 
			}

			int i; // ���������� ��� �������� ���������� ������� "������", �� ����������� � �������� "������"
			do {
				*X_apple = rand() % (field.width - 2) + 1; // ��������� ����� ��������� "������"
				*Y_apple = rand() % (field.height - 2) + 1;
				i = 0; // ��������� ����� ������������� ���������
				for (; i < *length; i++) // ������ ����� �� ������ ����������
					if (*X_apple == array_X[i] && *Y_apple == array_Y[i]) // ���� ���������� �������
						break; // �� ��������� ���� for
			} while (i < *length); // ����� ����� ��������� ������������, ���� ����� ������������� ��������� ������ ����� "������"

			c.X = *X_apple; // ��������� � ������ ��������� ����� ���������� ������� "������"
			c.Y = *Y_apple;
			SetConsoleCursorPosition(h, c); // �������� ���� �������
			SetConsoleTextAttribute(h, Color::RED); // ��������� ����� � �������
			cout << chars.apple; // ����������� ������� "������"
			SetConsoleTextAttribute(h, Color::GREEN); // �������� ��������� ����� � ������� - ��� ����������� ����������� "������"
		}
		else // ������, ����� ������ "������" ��������� �� ����� ������ �������
		{
			int i = 1; // ���������� �� ���������� �������, �� ����������� � ����� �������� - ����� ������ "������"
			for (; i < *length; i++)
				if (X == array_X[i] && Y == array_Y[i]) // ���� ���������� ������� � ����� - �����������
					break;
			if (i < *length) // ���� ����� ������������� ������� ������ ����� "������" - �� ��������� �������� ���� ����
				break;
			else // � ����� ��������� ��������� ������ "������"
			{
				c.X = array_X[0]; // ������������� � ������ ��������� ������� ������ "������"
				c.Y = array_Y[0];
				SetConsoleCursorPosition(h, c); // ������� ���� ������
				cout << ' '; // � ���������� ������ (������� ������)

				if (*length > 1) // ���� ����� ������ ������ 
				{
					c.X = array_X[*length - 1]; // ������������� � ������ ��������� ���������� ������� ������ "������"
					c.Y = array_Y[*length - 1];
					SetConsoleCursorPosition(h, c);  // ������� ���� ������
					putchar(chars.snake); // ������� ������ ���� "������"
				}

				for (int i = 0; i < *length - 1; i++) // ��������� ���� ������ ��������� ������� "������"
				{
					array_X[i] = array_X[i + 1]; // ������������ ��� ������ - ����� ����������
					array_Y[i] = array_Y[i + 1];
				}

				c.X = array_X[*length - 1] = X; // ������������� ����� ������� ������ "������"
				c.Y = array_Y[*length - 1] = Y;
				SetConsoleCursorPosition(h, c); // ������� ���� �������
				cout << chars.head; // ���������� ������ ������ "������"
			}
		}
	} while (flag); // ������� �� �����, ���� �������� ����������� ����������
}

void GAME_OVER(HANDLE h) { // ���������� ����
	// ������� ������� � ����� �������
	PlaySound(NULL, 0, 0);
	SetConsoleOutputCP(1251); // ��������� ������� �������� ��� ����������� ����������� ���������
	system("cls"); // ������� ������� 
	COORD END; // �������� ������� ��� �������� ��������� ���������� ����
	END.X = 52;
	END.Y = 15;

	SetConsoleCursorPosition(h, END); // ��������� ������� � �������� ����������
	SetConsoleTextAttribute(h, Color::RED); // ��������� ����� ������
	MUSIC_END(); // ������ ������ ���������� ����
	cout << "���� ��������!\n";
	_getch(); // �������� ������� �������
}