#include <iostream> // подключение библиотеки для работы с консолью
#include <windows.h> // подключение библиотеки для работы с WinAPI
#include <conio.h> // подключение библиотеки для работы с клавиатурой
#pragma comment(lib, "winmm.lib")
using namespace std;

enum Color : short { // цвета текста
	BLACK, DARKBLUE, DARKGREEN, TURQUOISE, DARKRED,
	PURPLE, DARKYELLOW, GREY, DARKGREY, BLUE, GREEN,
	CYAN, RED, PINK, YELLOW, WHITE
};

enum Key : short { // коды клавиш
	LEFT = 75, RIGHT = 77, UP = 72, DOWN = 80,
	ENTER = 13, SPACE = 32, ESCAPE = 27, BACKSPACE = 8
};

struct SNAKE_CHARS { // структура для хранения символов "змейки" и "яблока"
	char snake;
	char apple;
	char head;
};

struct GAME_FIELD { // структура для хранения параметров игрового поля
	int width;
	int height;
};


void MUSIC(); // запуск музыки
void MUSIC_END(); // завершение музыки
void APPLE_SETUP(const GAME_FIELD& field, int* X_apple, int* Y_apple, int* array_X, int* array_Y, int* length); // установка координат "яблока"
void DRAWING_FRAME(const GAME_FIELD& field, HANDLE h); // отрисовка рамки поля
void INFO_PANEL(HANDLE h, int max_length, int length, const GAME_FIELD& field); // отрисовка информационной панели
void UPDATE_LENGTH_APPLE(HANDLE h, int length, const GAME_FIELD& field); // обновление длины "змейки" в информационной панели
void DRAWING_APPLE_SNAKE(HANDLE h, int X_apple, int Y_apple, int array_X[], int array_Y[], const SNAKE_CHARS& chars); // отрисовка "яблока" и "змейки"
void MAIN_CYCLE(const int max_length, const SNAKE_CHARS& chars, int sleep_time, int dx, int dy, int* array_X, int* array_Y, int* length, const GAME_FIELD& field, HANDLE h, int* X_apple, int* Y_apple); // основной цикл игры
void GAME_OVER(HANDLE h); // движение "змейки"


void MUSIC() // запуск музыки
{
	auto MusicThread = [](LPVOID param) -> DWORD {
		PlaySoundA("C:\\Users\\sdk26\\Downloads\\city-bgm-336601.WAV", NULL, SND_ASYNC | SND_LOOP);
		return 0;
		};

	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MusicThread, 0, 0, 0); // создание потока для музыки
}

void MUSIC_END()
{
	auto MusicThread = [](LPVOID param) -> DWORD {
		PlaySoundA("C:\\Users\\sdk26\\Downloads\\igra-ne-proidena.WAV", NULL, SND_NODEFAULT | SND_LOOP);
		return 0;
		};

	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MusicThread, 0, 0, 0);
}

void APPLE_SETUP(const GAME_FIELD& field, int* X_apple, int* Y_apple, int* array_X, int* array_Y, int* length) { // установка координат "яблока"
	do // цикл ставит координаты яблока случанйм образом - но чтобы они не совпадали со "змейкой"
	{
		*X_apple = rand() % (field.width - 2) + 1; // установка координат "яблока" по оси абсцисс
		*Y_apple = rand() % (field.height - 2) + 1; // установка координат "яблока" по оси ординат
	} while (*X_apple != array_X[*length - 1] && *Y_apple != array_Y[*length - 1]); // проверка на совпадение с "змейкой"
}

void DRAWING_FRAME(const GAME_FIELD& field, HANDLE h) { // отрисовка рамки поля
	SetConsoleTextAttribute(h, 4); // установка цвета, которым рисуется рамка поля
	for (int y = 0; y < field.height; y++) // стандартный двойной цикл на отрисовку рамки
	{
		for (int x = 0; x < field.width; x++)
		{
			char s;
			if (x == 0 && y == 0) // в верхнем левом углу поля - символ соответствующего угла
				s = '#'; // 218
			else if (x == 0 && y == field.height - 1) // нижний левый угол
				s = '#'; // 192
			else if (y == 0 && x == field.width - 1) // верхний правый угол
				s = '#'; // 191
			else if (y == field.height - 1 && x == field.width - 1) // нижний правый угол
				s = '#'; // 217
			else if (y == 0 || y == field.height - 1) // верхняя и нижняя граница поля
				s = '#'; // 196
			else if (x == 0 || x == field.width - 1) // левая и правая граница поля
				s = '#'; // 179
			else s = ' '; // во всех остальных случаях должен быть просто пробел (означает пустую область поля)
			cout << s; // выводим символ
		}
		cout << "\n";
	}
}

void INFO_PANEL(HANDLE h, int max_length, int length, const GAME_FIELD& field) { // отрисовка информационной панели
	COORD info_panel; // создание объекта для хранения координат информационной панели

	SetConsoleOutputCP(1251); // установка кодовой страницы для отображения кириллицы
	info_panel.X = field.width + 5;
	info_panel.Y = 0;
	SetConsoleCursorPosition(h, info_panel);
	SetConsoleTextAttribute(h, Color::RED);
	cout << "Максимальная длина змейки: ";
	SetConsoleTextAttribute(h, Color::WHITE);
	cout << max_length; // выводим максимальную длину "змейки"

	info_panel.Y = 2; // установка координат для вывода текущей длины "змейки"
	SetConsoleCursorPosition(h, info_panel);
	SetConsoleTextAttribute(h, Color::RED);
	cout << "Поточная длина змейки: ";
	SetConsoleTextAttribute(h, Color::WHITE);
	cout << length; // выводим текущую длину "змейки"

	info_panel.Y = 4; // установка координат для вывода текущей длины "змейки"
	SetConsoleCursorPosition(h, info_panel);
	SetConsoleTextAttribute(h, Color::RED);
	cout << "Количество собраных яблок: ";
	SetConsoleTextAttribute(h, Color::WHITE);
	cout << length - 1; // выводим текущую длину "змейки"
	SetConsoleOutputCP(866);
}

void UPDATE_LENGTH_APPLE(HANDLE h, int length, const GAME_FIELD& field) { // обновление длины "змейки" в информационной панели
	COORD info_panel; // создание объекта для хранения координат информационной панели

	info_panel.X = field.width + 5; // установка координат для вывода текущей длины "змейки"
	info_panel.Y = 2;
	SetConsoleCursorPosition(h, info_panel);
	SetConsoleOutputCP(1251);
	SetConsoleTextAttribute(h, Color::RED);
	cout << "Поточная длина змейки: ";
	SetConsoleTextAttribute(h, Color::WHITE);
	cout << length; // выводим текущую длину "змейки"
	SetConsoleTextAttribute(h, Color::GREEN);

	info_panel.Y = 4; // установка координат для вывода количества собранных "яблок"
	SetConsoleCursorPosition(h, info_panel);
	SetConsoleTextAttribute(h, Color::RED);
	cout << "Количество собраных яблок: ";
	SetConsoleTextAttribute(h, Color::WHITE);
	cout << length - 1; // выводим количество собранных "яблок"
	SetConsoleOutputCP(866);
}

void DRAWING_APPLE_SNAKE(HANDLE h, int X_apple, int Y_apple, int array_X[], int array_Y[], const SNAKE_CHARS& chars) {
	COORD c; // создание объекта для хранения координат
	c.X = X_apple; // связываем объект координат с позициями "яблока"
	c.Y = Y_apple;
	SetConsoleCursorPosition(h, c); // отправляем курсор на позицию "яблока"
	SetConsoleTextAttribute(h, Color::RED); // устанавливаем красный цвет для отрисовки "яблока"
	cout << chars.apple; // отображаем символ "яблока"

	c.X = array_X[0]; // связываем объект координат со стартовой позицией "змейки"
	c.Y = array_Y[0];
	SetConsoleCursorPosition(h, c); // отправляем курсор на позицию головы "змейки"
	SetConsoleTextAttribute(h, Color::GREEN); // устанавливаем зеленый цвет для отрисовки "змейки"
	cout << chars.head; // отображаем символ головы "змейки"
}

void MAIN_CYCLE(const int max_length, const SNAKE_CHARS& chars, int sleep_time, int dx, int dy, int* array_X, int* array_Y, int* length,
	const GAME_FIELD& field, HANDLE h, int* X_apple, int* Y_apple) { // основной цикл игры
	COORD c; // создание объекта для хранения координат
	COORD info_panel; // создание объекта для хранения координат информационной панели

	bool flag = true; // переменная для управления ходом цикла

	do // собственно цикл игры
	{
		Sleep(sleep_time); // задержка потока программы на заданный ранее интервал

		if (_kbhit()) // проверяем, была ли нажата какая-либо клавиша и запускаем её обработку в случае ИСТИНЫ
		{
			int k = _getch(); // считываем код клавиши из буфера
			if (k == 0 || k == 224) // если первый код - вспомогательный, считываем второй код
				k = _getch();
			switch (k) // пропускаем код нажатой клавиши внутрь оператора выбора
			{
			case Key::DOWN: // если была нажата клавиша вниз
				dy = 1; // то приращение по оси ординат будет положительным
				dx = 0; // по оси абсцисс приращение нулевое
				break;
			case Key::UP: // если вверх
				dy = -1; // аналогично согласно геометрической логике
				dx = 0;
				break;
			case Key::LEFT: // если влево
				dy = 0;
				dx = -1;
				break;
			case Key::RIGHT: // если вправо
				dy = 0;
				dx = 1;
				break;
			case Key::ESCAPE: // если была нажата клавиша ESC
				flag = false; // устанавливаем флажок на ЛОЖЬ, чтоб закончить показ движения
				break;
			}
		}

		int X = array_X[*length - 1] + dx; // определяем значение абсциссы головы "змейки" после смещения
		int Y = array_Y[*length - 1] + dy; // то же самое для ординаты
		if (X == 0 || X == field.width - 1 || Y == 0 || Y == field.height - 1) // проверка на достижение границ поля
		{
			flag = false; // пока что - просто установка управляющей переменной цикла
		}
		else if (X == *X_apple && Y == *Y_apple) // проверка на достижение "яблока"
		{
			c.X = array_X[*length - 1]; // установка в объект координат позиции головы "змейки"
			c.Y = array_Y[*length - 1];
			SetConsoleCursorPosition(h, c); // установка курсора в эту позицию
			cout << chars.snake; // отображение символа тела "змейки"

			(*length)++; // увеличение длины "змейки" (яблоко проглочено)

			UPDATE_LENGTH_APPLE(h, *length, field);

			c.X = array_X[*length - 1] = X; // установка в массивы позиции нового звена "змейки"
			c.Y = array_Y[*length - 1] = Y;
			SetConsoleCursorPosition(h, c); // установка туда курсора
			cout << chars.head; // и отображение там символа головы "змейки"

			if (*length == max_length) // проверка, достигла ли длина "змейки" своего максимального значения
			{
				break; // пока что - просто прерываем цикл 
			}

			int i; // переменная для подсчета количества звеньев "змейки", не совпадающих с позицией "яблока"
			do {
				*X_apple = rand() % (field.width - 2) + 1; // установка новых координат "яблока"
				*Y_apple = rand() % (field.height - 2) + 1;
				i = 0; // обнуление числа несовпадающих координат
				for (; i < *length; i++) // запуск цикла на сверку совпадений
					if (*X_apple == array_X[i] && *Y_apple == array_Y[i]) // если совпадение найдено
						break; // то прерываем цикл for
			} while (i < *length); // поиск новых координат продолжается, пока число несовпадающих координат меньше длины "змейки"

			c.X = *X_apple; // установка в объект координат новой корректной позиции "яблока"
			c.Y = *Y_apple;
			SetConsoleCursorPosition(h, c); // отправка туда курсора
			SetConsoleTextAttribute(h, Color::RED); // установка цвета в красный
			cout << chars.apple; // отображение символа "яблока"
			SetConsoleTextAttribute(h, Color::GREEN); // обратная установка цвета в зеленый - для дальнейшего отображения "змейки"
		}
		else // случай, когда голова "змейки" оказалась на новой пустой позиции
		{
			int i = 1; // переменная на количество звеньев, не совпадающих с новой позицией - кроме хвоста "змейки"
			for (; i < *length; i++)
				if (X == array_X[i] && Y == array_Y[i]) // если совпадение найдено в цикле - прерываемся
					break;
			if (i < *length) // если число несовпадающих звеньев меньше длины "змейки" - то прерываем основной цикл игры
				break;
			else // а иначе запускаем обработку сдвига "змейки"
			{
				c.X = array_X[0]; // устанавливаем в объект координат позицию хвоста "змейки"
				c.Y = array_Y[0];
				SetConsoleCursorPosition(h, c); // двигаем туда курсор
				cout << ' '; // и отображаем пробел (затирка хвоста)

				if (*length > 1) // если длина змейки больше 
				{
					c.X = array_X[*length - 1]; // устанавливаем в объект координат предыдущую позицию головы "змейки"
					c.Y = array_Y[*length - 1];
					SetConsoleCursorPosition(h, c);  // двигаем туда курсор
					putchar(chars.snake); // выводим символ тела "змейки"
				}

				for (int i = 0; i < *length - 1; i++) // запускаем цикл свдига координат звеньев "змейки"
				{
					array_X[i] = array_X[i + 1]; // обрабатываем все звенья - кроме последнего
					array_Y[i] = array_Y[i + 1];
				}

				c.X = array_X[*length - 1] = X; // устанавливаем новую позицию головы "змейки"
				c.Y = array_Y[*length - 1] = Y;
				SetConsoleCursorPosition(h, c); // двигаем туда курсора
				cout << chars.head; // отображаем символ головы "змейки"
			}
		}
	} while (flag); // выходим из цикла, если сброшена управляющая переменная
}

void GAME_OVER(HANDLE h) { // завершение игры
	// очистка консоли и вывод надписи
	PlaySound(NULL, 0, 0);
	SetConsoleOutputCP(1251); // установка кодовой страницы для корректного отображения кириллицы
	system("cls"); // очистка консоли 
	COORD END; // создание объекта для хранения координат завершения игры
	END.X = 52;
	END.Y = 15;

	SetConsoleCursorPosition(h, END); // установка курсора в заданные координаты
	SetConsoleTextAttribute(h, Color::RED); // установка цвета текста
	MUSIC_END(); // запуск музыки завершения игры
	cout << "ИГРА ОКОНЧЕНА!\n";
	_getch(); // ожидание нажатия клавиши
}