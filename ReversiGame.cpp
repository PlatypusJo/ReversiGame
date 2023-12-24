#include <stdio.h>
#include <Windows.h>
#include <conio.h>
#include <string.h>
#include <ctime>

enum ConsoleColor {
	Blue = 1,
	Green = 2,
	Yellow = 14,
	LightGreen = 11
}; // цвета

struct PosData // данные о позиции
{
	bool corner;//позиция в углу
	bool square2x2;//C и X клетки рядом с углом
	bool edge;//края
	int internal;//внутренние фишки
	int stable;//стабильные фишки
	int discs;//общее кол-во дисков
};

struct Moves // ходы на уровнях дерева
{
	int cur_value, calc_value, summ_value = 0; // текущий вес вершины, посчитанные очки на данной вершине, очки с разных вершин в сумме
	int** brd;
};

struct TheBest
{
	int tb_value;
	int** brd;
}TheBestMove; // лучший ход

bool FIRSTCIRCLE;//первый проход по дереву
int kind; //игра с игроком или с компьютером
int** brd2, ** brd3, ** GBoard;// brd2 вспомогательная доска для функции проверки хода,brd3 вспомогательная доска для функции расчёта позиции, GBoard текущая игровая доска
unsigned char but;//код клавиши

bool CheckMove(int cc, int cy, int cx, int buf_w, int buf_h, int** brd) // проверяет можно ли сделать ход
{
	bool test, passed, result = FALSE; // test - надйдена фишка противника, passed - есть возможный ход
	int op;//oponent
	int comp = cc;
	if (comp == 2)
		op = 1;
	else
		op = 2;
	// копирование доски
	for (int i = 0; i < buf_h; i++)
		for (int j = 0; j < buf_w; j++)
			brd2[i][j] = brd[i][j];

	if (brd2[cy][cx] != 0) // если клетка занята
	{
		return result; // возвращение результата функции
	}
	test = FALSE;
	passed = FALSE;

	for (int i = cx - 1; i >= 0; i--) //проверяем есть ли возможность хода слева
	{
		if (brd2[cy][i] == op)
			test = TRUE;
		else
			if (brd2[cy][i] == 0 || brd2[cy][i] == comp && test == FALSE) // если следующая клетка со своей фишкой или пустая и между нет фишек противника тогда выход из цикла
				break;
			else
				if (brd2[cy][i] == comp && test)
				{
					passed = TRUE;
					for (int j = cx; j > i; j--) //переворачиваем фишки
						brd2[cy][j] = comp;
					break;
				}
	}
	test = FALSE;

	for (int i = cx + 1; i < buf_w; i++) //проверяем есть ли возможность хода справа
	{
		if (brd2[cy][i] == op)
			test = TRUE;
		else
			if (brd2[cy][i] == 0 || brd2[cy][i] == comp && test == FALSE)
				break;
			else
				if (brd2[cy][i] == comp && test)
				{
					passed = TRUE;
					for (int j = cx; j < i; j++)
						brd2[cy][j] = comp;
					break;
				}
	}
	test = FALSE;

	for (int i = cy - 1; i >= 0; i--) //проверяем есть ли возможность хода вверх 
	{
		if (brd2[i][cx] == op)
			test = TRUE;
		else
			if (brd2[i][cx] == 0 || brd2[i][cx] == comp && test == FALSE)
				break;
			else
				if (brd2[i][cx] == comp && test)
				{
					passed = TRUE;
					for (int j = cy; j > i; j--)
						brd2[j][cx] = comp;
					break;
				}
	}
	test = FALSE;

	for (int i = cy + 1; i < buf_h; i++) //проверяем есть ли возможность хода вниз
	{
		if (brd2[i][cx] == op)
			test = TRUE;
		else
			if (brd2[i][cx] == 0 || brd2[i][cx] == comp && test == FALSE)
				break;
			else
				if (brd2[i][cx] == comp && test)
				{
					passed = TRUE;
					for (int j = cy; j < i; j++)
						brd2[j][cx] = comp;
					break;
				}
	}
	test = FALSE;

	for (int i = 1; i < buf_h; i++) //проверяем есть ли возможность хода влево вверх по диагонали
	{
		if (cy - i >= 0 && cx - i >= 0)
		{
			if (brd2[cy - i][cx - i] == op)
				test = TRUE;
			else
				if (brd2[cy - i][cx - i] == 0 || brd2[cy - i][cx - i] == comp && test == FALSE)
					break;
				else
					if (brd2[cy - i][cx - i] == comp && test)
					{
						passed = TRUE;
						for (int j = 0; j < i; j++)
							brd2[cy - j][cx - j] = comp;
						break;
					}
		}
		else
			break;
	}
	test = FALSE;

	for (int i = 1; i < buf_h; i++) //проверяем есть ли возможность хода влево вниз по диагонали
	{
		if (cy + i < buf_h && cx - i >= 0)
		{
			if (brd2[cy + i][cx - i] == op)
				test = TRUE;
			else
				if (brd2[cy + i][cx - i] == 0 || brd2[cy + i][cx - i] == comp && test == FALSE)
					break;
				else
					if (brd2[cy + i][cx - i] == comp && test)
					{
						passed = TRUE;
						for (int j = 0; j < i; j++)
							brd2[cy + j][cx - j] = comp;
						break;
					}
		}
		else
			break;
	}
	test = FALSE;

	for (int i = 1; i < buf_h; i++) //проверяем есть ли возможность хода вправо вниз по диагонали
	{
		if (cy + i < buf_h && cx + i < buf_w)
		{
			if (brd2[cy + i][cx + i] == op)
				test = TRUE;
			else
				if (brd2[cy + i][cx + i] == 0 || brd2[cy + i][cx + i] == comp && test == FALSE)
					break;
				else
					if (brd2[cy + i][cx + i] == comp && test)
					{
						passed = TRUE;
						for (int j = 0; j < i; j++)
							brd2[cy + j][cx + j] = comp;
						break;
					}
		}
		else
			break;
	}
	test = FALSE;

	for (int i = 1; i < buf_h; i++) //проверяем есть ли возможность хода вправо вверх по диагонали
	{
		if (cy - i >= 0 && cx + i < buf_w)
		{
			if (brd2[cy - i][cx + i] == op)
				test = TRUE;
			else
				if (brd2[cy - i][cx + i] == 0 || brd2[cy - i][cx + i] == comp && test == FALSE)
					break;
				else
					if (brd2[cy - i][cx + i] == comp && test)
					{
						passed = TRUE;
						for (int j = 0; j < i; j++)
							brd2[cy - j][cx + j] = comp;
						break;
					}
		}
		else
			break;
	}
	test = FALSE;

	if (passed)
		result = TRUE;
	return result;
}

PosData EvalPosition(int Comp, int cy, int cx, int buf_w, int buf_h, int** brd) // оценка позиции
{
	// делаем копию доски
	for (int i = 0; i < buf_w; i++)
		for (int j = 0; j < buf_h; j++)
			brd3[i][j] = brd[i][j];

	bool intern;

	PosData data;
	data.corner = FALSE;
	data.edge = FALSE;
	data.square2x2 = FALSE;
	data.discs = 0;
	data.internal = 0;
	data.stable = 0;

	for (int i = 0; i < buf_h; i++)
		for (int j = 0; j < buf_w; j++)
			if (brd3[i][j] == Comp)
				data.discs++;

	// проверяет заняты ли углы
	if (cy == 0 && cx == 0 || cy == buf_h - 1 && cx == buf_w - 1 || cy == 0 && cx == buf_w - 1 || cy == buf_h - 1 && cx == 0)
		data.corner = TRUE;
	//проверяет опасные C- и X-клетки
	if (cy <= 1 && cx <= 1 || cy <= 1 && cx >= buf_w - 1 || cy >= buf_h - 1 && cx <= 1 || cy >= buf_h - 1 && cx >= buf_w - 1)
		data.square2x2 = TRUE;

	// проверка края
	if (cy == 0 || cx == 0 || cx == buf_w - 1 || cy == buf_h - 1)
		data.edge = TRUE;

	//Стабильные диски
	for (int i = 0; i < buf_h; i++) //левый верхний угол 
	{
		if (brd3[i][0] != Comp)
			break;
		for (int j = 0; j < buf_w; j++)
		{
			if (brd3[i][j] != Comp)
				break;
			data.stable++;
			brd3[i][j] = 0;
		}
	}

	for (int i = buf_h - 1; i >= 0; i--) // левый нижний угол 
	{
		if (brd3[i][0] != Comp)
			break;
		for (int j = 0; j < buf_w; j++)
		{
			if (brd3[i][j] != Comp)
				break;
			data.stable++;
			brd3[i][j] = 0;
		}
	}

	for (int i = buf_h - 1; i >= 0; i--) //правый нижний угол
	{
		if (brd3[i][buf_w - 1] != Comp)
			break;
		for (int j = buf_w - 1; j >= 0; j--)
		{
			if (brd3[i][j] != Comp)
				break;
			data.stable++;
			brd3[i][j] = 0;
		}
	}

	for (int i = 0; i < buf_h; i++) //правый верхний угол
	{
		if (brd3[i][buf_w - 1] != Comp)
			break;
		for (int j = buf_w - 1; j >= 0; j--)
		{
			if (brd3[i][j] != Comp)
				break;
			data.stable++;
			brd3[i][j] = 0;
		}
	}

	// считает внутренние фишки
	for (int i = 0; i < buf_h; i++)
		for (int j = 0; j < buf_w; j++)
			if (brd[i][j] == Comp)
			{
				intern = TRUE;
				if (i > 0 && j > 0 && brd[i - 1][j - 1] == 0)
					intern = FALSE;
				if (i > 0 && brd[i - 1][j] == 0)
					intern = FALSE;
				if (i > 0 && j < buf_w - 1 && brd[i - 1][j + 1] == 0)
					intern = FALSE;
				if (i < buf_h - 1 && j>0 && brd[i + 1][j - 1] == 0)
					intern = FALSE;
				if (i < buf_h - 1 && brd[i + 1][j] == 0)
					intern = FALSE;
				if (i < buf_h - 1 && j < buf_w - 1 && brd[i + 1][j + 1] == 0)
					intern = FALSE;
				if (j > 0 && brd[i][j - 1] == 0)
					intern = FALSE;
				if (j < buf_w - 1 && brd[i][j + 1] == 0)
					intern = FALSE;
				if (intern)
					data.internal++;
			}
	return data; // возвращение результата функции
}

int CalculDataPosition(int CompColour, int weidth, int hight, int Oy, int Ox, int** curbrd) // подсчёт очков за текущую позицию
{
	PosData CURPosData = EvalPosition(CompColour, Oy, Ox, weidth, hight, curbrd);
	int value1;
	if (CURPosData.corner)
		value1 = 500;
	else
		value1 = 0;
	value1 = value1 + CURPosData.stable * 25;
	value1 = value1 + CURPosData.internal * 15;
	value1 = value1 + CURPosData.discs * 20;
	if (CURPosData.edge)
		value1 += 10;
	if (CURPosData.square2x2)
		value1 = value1 - 250;

	return value1; // возвращение результата функции
}

bool DoStep(int CUR_Y, int CUR_X, int weidth, int hight, int CURPLAYER, int huPlayer, int aiPlayer, int depth, Moves NewMove[], int LEVELS, int* x, int* y)// функция хода
{
	bool FirstMove = TRUE, FINDMOVE = FALSE;// FirstMove - первый ход из текущей вершины, FINDMOVE - ход из вершины найден
	int NEXTPLAYER; //следующий игрок
	if (depth == 0)
	{
		FIRSTCIRCLE = TRUE;// первый проход из 0 вершины
		NEXTPLAYER = CURPLAYER;
	}
	if (depth >= 1)
	{
		if (CURPLAYER == 2)
			NEXTPLAYER = 1;
		else
			NEXTPLAYER = 2;
		for (int i = 0; i < weidth; i++)
			for (int j = 0; j < hight; j++)
				NewMove[depth].brd[i][j] = brd2[i][j];// сохранение состояния доски на текущей глубине поиска
		NewMove[depth].calc_value = CalculDataPosition(CURPLAYER, weidth, hight, CUR_Y, CUR_X, NewMove[depth].brd); // оценка хода на текущей глубине
		if (CURPLAYER == aiPlayer)// вычитаем или прибавляем очки
			NewMove[depth].summ_value = NewMove[depth - 1].summ_value + NewMove[depth].calc_value;
		else
			NewMove[depth].summ_value = NewMove[depth - 1].summ_value - NewMove[depth].calc_value;
	}
	if (depth == 5) // если глубина 5 
	{
		NewMove[depth].cur_value = NewMove[depth].summ_value; // присваиваем вес вершине
		return TRUE;
	}
	for (int i = 0; i < weidth; i++)
		for (int j = 0; j < hight; j++)
			if (CheckMove(NEXTPLAYER, i, j, weidth, hight, NewMove[depth].brd)) // проверяет можно ли сделать ход
			{
				if (depth != 6)// если не достигнута нужная глубина
					FINDMOVE = DoStep(i, j, weidth, hight, NEXTPLAYER, huPlayer, aiPlayer, depth + 1, NewMove, LEVELS, NULL, NULL); // переход на следующую глубину поиска
				if (depth < 5 && FINDMOVE)// если глубина 5 не достигнута и найден ход на глубине depth+1
				{
					if (CURPLAYER == aiPlayer)// если текущий ход компютера
					{
						if (FirstMove)
						{
							NewMove[depth].cur_value = NewMove[depth + 1].cur_value;
							FirstMove = FALSE;
						}
						else
							if (NewMove[depth + 1].cur_value > NewMove[depth].cur_value)// сранение значений на следующей и текущей глубине
							{
								NewMove[depth].cur_value = NewMove[depth + 1].cur_value;
							}
					}
					if (CURPLAYER == huPlayer)// если текущий ход противника
					{
						if (FirstMove)
						{
							NewMove[depth].cur_value = NewMove[depth + 1].cur_value;
							FirstMove = FALSE;
						}
						else
							if (NewMove[depth + 1].cur_value < NewMove[depth].cur_value)// сранение значений на следующей и текущей глубине
								NewMove[depth].cur_value = NewMove[depth + 1].cur_value;
					}
					if (depth == 0)
					{
						if (FIRSTCIRCLE)
						{
							TheBestMove.tb_value = NewMove[1].cur_value; // запоминаем лучшую оценку
							*x = i; // запоминаем координаты хода
							*y = j;
							for (int i = 0; i < weidth; i++)
								for (int j = 0; j < hight; j++)
									TheBestMove.brd[i][j] = NewMove[1].brd[i][j];
							FIRSTCIRCLE = FALSE;
						}
						else
							if (NewMove[1].cur_value > TheBestMove.tb_value)
							{
								TheBestMove.tb_value = NewMove[1].cur_value;
								*x = i;
								*y = j;
								for (int i = 0; i < weidth; i++)
									for (int j = 0; j < hight; j++)
										TheBestMove.brd[i][j] = NewMove[1].brd[i][j];
							}

					}
				}
			}
	if (depth != 0 && FINDMOVE == FALSE)// если глубина не 0 и следующий ход не найден
	{
		NewMove[depth].cur_value = NewMove[depth].summ_value;
		FINDMOVE = TRUE;
	}
	return FINDMOVE; // возвращение результата функции
}

void ComputeDisks(int weidth, int hight, int AI, int Chel, int* countpl1, int* countpl2)//считает фишки игрогов
{
	*countpl1 = 0, * countpl2 = 0;
	for (int i = 0; i < hight; i++)
	{
		for (int j = 0; j < weidth; j++)
		{
			if (GBoard[i][j] == AI)
				*countpl2 += 1;
			else
				if (GBoard[i][j] == Chel)
					*countpl1 += 1;
		}
	}
}

void DrawEmptyChosenCell(HDC hd, int x, int y)//рисует выбранную пустую клетку
{
	RECT r = { 10,10,110,110 };//координаты для рисования
	HBRUSH hb = CreateSolidBrush(RGB(255, 255, 0));
	r = { (50 + 3) * y + 20, (50 + 3) * x + 20, 50 * (y + 1) + y * 3 + 20, 50 * (x + 1) + x * 3 + 20 };
	SelectObject(hd, hb);
	FillRect(hd, &r, hb);
	DeleteObject(hb);
}

void DrawBlackDiskChosenCell(HDC hd, int x, int y)//рисует выбранную клетку с чёрной фишкой
{
	RECT r = { 10,10,110,110 };//координаты для рисования
	int R = 20;
	HBRUSH hb = CreateSolidBrush(RGB(255, 255, 0));
	r = { (50 + 3) * y + 20, (50 + 3) * x + 20, 50 * (y + 1) + y * 3 + 20, 50 * (x + 1) + x * 3 + 20 };
	SelectObject(hd, hb);
	FillRect(hd, &r, hb);
	DeleteObject(hb);
	HPEN Pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	HBRUSH black = CreateSolidBrush(RGB(0, 0, 0));
	SelectObject(hd, black);
	SelectObject(hd, Pen);
	Ellipse(hd, (50 + 3) * y + 20, (50 + 3) * x + 20, 50 * (y + 1) + y * 3 + 20, 50 * (x + 1) + x * 3 + 20);
	DeleteObject(black);
	DeleteObject(Pen);
}

void DrawWhiteDiskChosenCell(HDC hd, int x, int y)//рисует выбранную клетку с белой фишкой
{
	RECT r = { 10,10,110,110 };//координаты для рисования
	int R = 20;
	HBRUSH hb = CreateSolidBrush(RGB(255, 255, 0));
	r = { (50 + 3) * y + 20, (50 + 3) * x + 20, 50 * (y + 1) + y * 3 + 20, 50 * (x + 1) + x * 3 + 20 };
	SelectObject(hd, hb);
	FillRect(hd, &r, hb);
	DeleteObject(hb);
	HPEN whitePen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	HBRUSH white = CreateSolidBrush(RGB(255, 255, 255));
	SelectObject(hd, white);
	SelectObject(hd, whitePen);
	Ellipse(hd, (50 + 3) * y + 20, (50 + 3) * x + 20, 50 * (y + 1) + y * 3 + 20, 50 * (x + 1) + x * 3 + 20);
	DeleteObject(white);
	DeleteObject(whitePen);
}

void DrawEmptyCell(HDC hd, int x, int y)//рисует пустую клетку
{
	RECT r = { 10,10,110,110 };//координаты для рисования
	HBRUSH hb = CreateSolidBrush(RGB(0, 200, 0));
	r = { (50 + 3) * y + 20, (50 + 3) * x + 20, 50 * (y + 1) + y * 3 + 20, 50 * (x + 1) + x * 3 + 20 };
	SelectObject(hd, hb);
	FillRect(hd, &r, hb);
	DeleteObject(hb);
}

void DrawBlackDiskCell(HDC hd, int x, int y)//рисует клетку с чёрной фишкой
{
	RECT r = { 10,10,110,110 };//координаты для рисования
	int R = 20;
	HBRUSH hb = CreateSolidBrush(RGB(0, 200, 0));
	r = { (50 + 3) * y + 20, (50 + 3) * x + 20, 50 * (y + 1) + y * 3 + 20, 50 * (x + 1) + x * 3 + 20 };
	SelectObject(hd, hb);
	FillRect(hd, &r, hb);
	DeleteObject(hb);
	HPEN Pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	HBRUSH black = CreateSolidBrush(RGB(0, 0, 0));
	SelectObject(hd, black);
	SelectObject(hd, Pen);
	Ellipse(hd, (50 + 3) * y + 20, (50 + 3) * x + 20, 50 * (y + 1) + y * 3 + 20, 50 * (x + 1) + x * 3 + 20);
	DeleteObject(black);
	DeleteObject(Pen);
}

void DrawWhiteDiskCell(HDC hd, int x, int y)//рисует клетку с белой фишкой фишкой
{
	RECT r = { 10,10,110,110 };//координаты для рисования
	int R = 20;
	HBRUSH hb = CreateSolidBrush(RGB(0, 200, 0));
	r = { (50 + 3) * y + 20, (50 + 3) * x + 20, 50 * (y + 1) + y * 3 + 20, 50 * (x + 1) + x * 3 + 20 };
	SelectObject(hd, hb);
	FillRect(hd, &r, hb);
	DeleteObject(hb);
	HPEN whitePen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	HBRUSH white = CreateSolidBrush(RGB(255, 255, 255));
	SelectObject(hd, white);
	SelectObject(hd, whitePen);
	Ellipse(hd, (50 + 3) * y + 20, (50 + 3) * x + 20, 50 * (y + 1) + y * 3 + 20, 50 * (x + 1) + x * 3 + 20);
	DeleteObject(white);
	DeleteObject(whitePen);
}

void DrawDesk(int weidth, int hight, HDC hd, int new_x, int new_y)//рисует доску
{
	Sleep(10);
	for (int i = 0; i < hight; i++)
		for (int j = 0; j < weidth; j++)
		{
			if (GBoard[i][j] == 0 && new_x == i && new_y == j)
				DrawEmptyChosenCell(hd, i, j);
			else
				if (GBoard[i][j] == 0)
					DrawEmptyCell(hd, i, j);
				else
					if (GBoard[i][j] == 2 && new_x == i && new_y == j)
						DrawBlackDiskChosenCell(hd, i, j);
					else
						if (GBoard[i][j] == 2)
							DrawBlackDiskCell(hd, i, j);
						else
							if (GBoard[i][j] == 1 && new_x == i && new_y == j)
								DrawWhiteDiskChosenCell(hd, i, j);
							else
								if (GBoard[i][j] == 1)
									DrawWhiteDiskCell(hd, i, j);
		}
}

int DefineFinish(int countpl1, int countpl2)//определяет конец игры
{
	int result;
	if (countpl1 > countpl2)
		result = 1; // победа игрока 1
	else
		if (countpl2 > countpl1)
			result = 2; // победа игрока 2
		else
			result = 3; // ничья
	return result; // возвращение результата работы функции
}

bool PlayerMove(int weidth, int hight, int compute, int human, HDC hdc, int countpl1, int countpl2) // ход игрока
{
	int next_x, next_y; // объявление переменных - координат клетки на доске
	bool Move = FALSE, FalseMove = FALSE, result = FALSE; // Move - найден ход, FalseMove - неверный ход, result - результат работы функции
	for (int i = 0; i < hight && Move == FALSE; i++)	// поиск возможного хода
	{
		for (int j = 0; j < weidth && Move == FALSE; j++)
			if (CheckMove(human, i, j, weidth, hight, GBoard))
			{
				Move = TRUE;
				next_x = i; // сохранение координат возможного хода
				next_y = j;
			}
	}

	if (Move) // если ход найден
	{
		if (kind == 1) // если игра с компьютером
		{
			printf("\n\t\t\t\t\t\t\t\tВаш ход: ");
			if (human == 1)
				printf("Белые\n");
			else
				printf("Чёрные\n");
		}
		else
			if (kind == 2) //если игра с человеком
			{
				printf("\n\t\t\t\t\t\t\t\tВаш ход: ");
				if (human == 1)
					printf("Белые\n");
				else
					printf("Чёрные\n");
			}

		//отключение курсора
		CONSOLE_CURSOR_INFO curs = { 0 };
		curs.dwSize = sizeof(curs);
		curs.bVisible = FALSE;
		::SetConsoleCursorInfo(::GetStdHandle(STD_OUTPUT_HANDLE), &curs);

		// вывод информации о текущей игре
		if (kind == 1)
			printf("\n\t\t\t\t\t\t\t    Вы: %d\tКомпьютер: %d\n", countpl1, countpl2);
		else
			if (kind == 2)
				printf("\n\t\t\t\t\t\t   Игрок 1 (Чёрные): %d\tИгрок 2 (Белые): %d\n", countpl1, countpl2);
		printf("\t\t\t\t\t\t\t\"Esc\" - Вернуться в главное меню");
		Sleep(10);
		DrawDesk(weidth, hight, hdc, next_x, next_y); // отрисовка доски
		do
		{
			if (FalseMove) // был сделан недопустимый ход
			{
				if (kind == 1)
				{
					printf("\n\t\t\t\t\t\t\t\tВаш ход: ");
					if (human == 1)
						printf("Белые\n");
					else
						printf("Чёрные\n");
				}
				else
					if (kind == 2)
					{
						printf("\n\t\t\t\t\t\t\t\tВаш ход: ");
						if (human == 1)
							printf("Белые\n");
						else
							printf("Чёрные\n");
					}
				if (kind == 1)
					printf("\n\t\t\t\t\t\tВы: %d\tКомпьютер: %d\n", countpl1, countpl2);
				else
					if (kind == 2)
						printf("\n\t\t\t\t\t\t   Игрок 1 (Чёрные): %d\tИгрок 2 (Белые): %d\n", countpl1, countpl2);
				printf("\t\t\t\t\t\t\t\"Esc\" - Вернуться в главное меню");
				Sleep(10);
				DrawDesk(weidth, hight, hdc, next_x, next_y);
				FalseMove = FALSE;
			}
			but = 0; // очищение клавиши
			// обработка перемещения игрока по доске с помощью клавиш
			if (_kbhit())
				but = _getch();
			switch (but)
			{
			case 72:
				next_x--;
				if (next_x < 0) next_x = weidth - 1;
				DrawDesk(weidth, hight, hdc, next_x, next_y);
				break;
			case 80:
				next_x++;
				if (next_x > weidth - 1) next_x = 0;
				DrawDesk(weidth, hight, hdc, next_x, next_y);
				break;
			case 77:
				next_y++;
				if (next_y > hight - 1) next_y = 0;
				DrawDesk(weidth, hight, hdc, next_x, next_y);
				break;
			case 75:
				next_y--;
				if (next_y < 0) next_y = hight - 1;
				DrawDesk(weidth, hight, hdc, next_x, next_y);
				break;
			}
			if (but != 27 && but == 13) // если не нажата клавиша esc и нажата клавиша enter
			{
				if (CheckMove(human, next_x, next_y, weidth, hight, GBoard)) // проверка возможности хода
				{
					for (int i = 0; i < hight; i++)
						for (int j = 0; j < weidth; j++)
							GBoard[i][j] = brd2[i][j]; // ход записан
					result = TRUE;
				}
				else
				{
					system("cls");
					printf("\n\t\t\t\t\t\t\tНеверный ход.Попробуйте ещё раз\n\t\t\t\t\t\t\t");
					Sleep(10);
					DrawDesk(weidth, hight, hdc, -1, -1);
					Sleep(2000);
					FalseMove = TRUE;
					system("cls");
				}
			}
		} while (but != 27 && result == FALSE); // пока не нажата ecs и не сделан ход
	}
	else
	{
		printf("\t\t\t\t\t\t\tНет возможных ходов\n");
		Sleep(10);
		DrawDesk(weidth, hight, hdc, -1, -1);
		Sleep(2000);
	}
	system("cls");

	if (result)
	{
		printf("\t\t\t\t\t\t\tВы сделали ход\n\t\t\t\t\t\t\t");
		but = 0;
		while (_kbhit())
			_getch();
		Sleep(10);
		DrawDesk(weidth, hight, hdc, -1, -1);
		Sleep(2000);
	}
	system("cls");
	return result; // возвращение результата работы функции
}

void PlayGameVSComp(int weidth, int hight, int aiPL, int human, Moves move_n[], int Razmer, HANDLE hConsole, HDC hdc, int countpl1, int countpl2)
{
	SetConsoleTextAttribute(hConsole, (WORD)(Blue << 4) | Yellow); // установка цвета текста
	but = 0; //очищение клавиши
	system("cls");
	ComputeDisks(weidth, hight, aiPL, human, &countpl1, &countpl2); // подсчёт дисков
	bool compmove = FALSE, playermove = FALSE, huNotFound = TRUE, aiNotFound = TRUE, Finish; // объявление переменных
	if (aiPL == 2) // если компьютер играет чёрными,то он ходит первым
	{
		int check_x = 0, check_y = 0; // инициализация координат хода компьютера
		for (int i = 0; i < hight; i++)
			for (int j = 0; j < weidth; j++)
				move_n[0].brd[i][j] = GBoard[i][j]; // инциализация текущего состояния доски
		compmove = DoStep(-1, -1, weidth, hight, aiPL, human, aiPL, 0, move_n, 6, &check_x, &check_y); // просчёт хода компьютером
		if (compmove)
		{
			for (int i = 0; i < hight; i++)
				for (int j = 0; j < weidth; j++)
					GBoard[i][j] = TheBestMove.brd[i][j]; // в доску записывается лучший выбранный ход компьютера
			ComputeDisks(weidth, hight, aiPL, human, &countpl1, &countpl2);	// подсчёт дисков
			printf("\t\t\t\t\t\t\tКомпьютер сделал ход\n\t\t\t\t\t\t\t");
			Sleep(10);
			DrawDesk(weidth, hight, hdc, check_x, check_y);// отрисовка доски,на которой показан ход компьютера
			Sleep(2000);
			system("cls");
		}
	}
	else
		playermove = PlayerMove(weidth, hight, aiPL, human, hdc, countpl1, countpl2); // ход игрока
	if (playermove || compmove)
		ComputeDisks(weidth, hight, aiPL, human, &countpl1, &countpl2);
	if (but != 27)
	{
		do
		{
			if (compmove || playermove == FALSE) // если сходил компьютер или не сходил игрок
			{
				playermove = PlayerMove(weidth, hight, aiPL, human, hdc, countpl1, countpl2);
				if (playermove == FALSE)
					huNotFound = FALSE;
				else
					huNotFound = TRUE;
				compmove = FALSE;
				ComputeDisks(weidth, hight, aiPL, human, &countpl1, &countpl2);
				system("cls");
			}
			if (but != 27)
				if (playermove || compmove == FALSE) // если сходил игрок или не сходил компьютер
				{
					int check_x = 0, check_y = 0;
					for (int i = 0; i < hight; i++)
						for (int j = 0; j < weidth; j++)
							move_n[0].brd[i][j] = GBoard[i][j];
					compmove = DoStep(-1, -1, weidth, hight, aiPL, human, aiPL, 0, move_n, 6, &check_x, &check_y);
					if (compmove == FALSE)
						aiNotFound = FALSE;
					else
						aiNotFound = TRUE;
					playermove = FALSE;
					if (compmove)
					{
						for (int i = 0; i < hight; i++)
							for (int j = 0; j < weidth; j++)
								GBoard[i][j] = TheBestMove.brd[i][j];
						ComputeDisks(weidth, hight, aiPL, human, &countpl1, &countpl2);
						printf("\t\t\t\t\t\t\tКомпьютер сделал ход\n\t\t\t\t\t\t\t");
						Sleep(10);
						DrawDesk(weidth, hight, hdc, check_x, check_y); // отрисовка доски,на которой показан ход компьютера
						Sleep(2000);
						system("cls");
					}
				}
			if (huNotFound || aiNotFound) // пока у игроков есть ходы игра продолжается
				Finish = TRUE;
			else
				Finish = FALSE;
		} while (but != 27 && Finish);
	}
	if (but != 27) // если не нажата клавиша esc
	{
		int GameOver = DefineFinish(countpl1, countpl2); // определение итога игры
		switch (GameOver)
		{
		case 1:printf("\t\t\t\t\t\t\t\tВы победили\n");
			printf("\n\t\t\t\t\t\tВы: %d\tКомпьютер: %d\n", countpl1, countpl2);
			Sleep(10);
			DrawDesk(weidth, hight, hdc, -1, -1);
			Sleep(2000);
			break;
		case 2:printf("\t\t\t\t\t\t\t\tВы проиграли\n");
			printf("\n\t\t\t\t\t\tВы: %d\tКомпьютер: %d\n", countpl1, countpl2);
			Sleep(10);
			DrawDesk(weidth, hight, hdc, -1, -1);
			Sleep(2000);
			break;
		case 3:printf("\t\t\t\t\t\t\t\tНичья\n");
			printf("\n\t\t\t\t\t\tВы: %d\tКомпьютер: %d\n", countpl1, countpl2);
			Sleep(10);
			DrawDesk(weidth, hight, hdc, -1, -1);
			Sleep(2000);
			break;
		}
		system("cls");
	}
}

void PlayGame2players(int weidth, int hight, HANDLE hConsole, HDC hdc, int countpl1, int countpl2)
{
	SetConsoleTextAttribute(hConsole, (WORD)(Blue << 4) | Yellow); // установка цвета текста
	but = 0; // очищение клавиши
	system("cls");
	int  player1hum = 2, player2hum = 1; // установка цвета фишек игроков
	ComputeDisks(weidth, hight, player2hum, player1hum, &countpl1, &countpl2); // подсчёт фишек игроков
	bool Finish, player2move = FALSE, player1move = FALSE, NotFound1 = TRUE, NotFound2 = TRUE; // объявление переменных
	do
	{
		if (player2move || player1move == FALSE) // игрок 2 сходил или игрок 1 не сходил
		{
			player1move = PlayerMove(weidth, hight, player2hum, player1hum, hdc, countpl1, countpl2);
			player2move = FALSE;
			if (player1move == FALSE)
				NotFound1 = FALSE;
			else
				NotFound1 = TRUE;
			if (player1move)
				ComputeDisks(weidth, hight, player2hum, player1hum, &countpl1, &countpl2);
			system("cls");
		}
		if (but != 27)
			if (player1move || player2move == FALSE) // игрок 1 сходил или игрок 2 не сходил
			{
				player2move = PlayerMove(weidth, hight, player1hum, player2hum, hdc, countpl1, countpl2);
				player1move = FALSE;
				if (player2move == FALSE)
					NotFound2 = FALSE;
				else
					NotFound2 = TRUE;
				if (player2move)
					ComputeDisks(weidth, hight, player2hum, player1hum, &countpl1, &countpl2);
				system("cls");
			}
		if (NotFound1 || NotFound2) //пока у игроков есть ходы игра продолжается
			Finish = TRUE;
		else
			Finish = FALSE;
	} while (but != 27 && Finish);
	if (but != 27) // если не нажата клавиша esc
	{
		int GameOver = DefineFinish(countpl1, countpl2); // определение итога игры
		switch (GameOver)
		{
		case 1:printf("\t\t\t\t\t\t\t\tпобедил игрок 1\n");
			printf("\n\t\t\t\t\t\t   Игрок 1 (Чёрные): %d\tИгрок 2 (Белые): %d\n", countpl1, countpl2);
			Sleep(10);
			DrawDesk(weidth, hight, hdc, -1, -1);
			Sleep(2000);
			break;
		case 2:printf("\t\t\t\t\t\t\t\tпобедил игрок 2\n");
			printf("\n\t\t\t\t\t\t   Игрок 1 (Чёрные): %d\tИгрок 2 (Белые): %d\n", countpl1, countpl2);
			Sleep(10);
			DrawDesk(weidth, hight, hdc, -1, -1);
			Sleep(2000);
			break;
		case 3:printf("\t\t\t\t\t\t\t\tНичья\n");
			printf("\n\t\t\t\t\t\t   Игрок 1 (Чёрные): %d\tИгрок 2 (Белые): %d\n", countpl1, countpl2);
			Sleep(10);
			DrawDesk(weidth, hight, hdc, -1, -1);
			Sleep(2000);
			break;
		}
		system("cls");
	}
}

void ReadMe(HANDLE hConsole, HWND hcon)
{
	SetConsoleTextAttribute(hConsole, (WORD)(Blue << 4) | Yellow);
	system("cls");

	// программная установка консольного шрифта для отображения русского текста
	CONSOLE_FONT_INFO cfi;
	GetCurrentConsoleFont(hConsole, false, &cfi);
	COORD fs = GetConsoleFontSize(hConsole, cfi.nFont);
	PCONSOLE_FONT_INFOEX ccf = new CONSOLE_FONT_INFOEX;
	(*ccf).dwFontSize.X = 12;
	(*ccf).dwFontSize.Y = 20;
	(*ccf).nFont = 11;
	(*ccf).cbSize = sizeof(CONSOLE_FONT_INFOEX);
	ccf->FontWeight = 400;
	lstrcpyW((*ccf).FaceName, L"Lucida Console");
	(*ccf).FontFamily = FF_DONTCARE;
	bool b = SetCurrentConsoleFontEx(hConsole, false, ccf);
	fs = GetConsoleFontSize(hConsole, cfi.nFont);

	int k = 9; // кол-во пунктов
	COORD position[9];
	char names[9][200] = {
		"Программа \"Реверси\" предназначена для игры в реверси по стандартным правилам. Для ",
		"работы с программой используются клавиши со стрелками, а также клавиша \"Enter\"",
		"для подтверждения действия. Пользователю предоставляется возможность игры с компьютером",
		"или вдвоём при выборе соответствующего пункта главного меню. Игра проходит на поле ",
		"размером 4x4, 6x6 или 8x8 в зависимости от выбора пользователя. Перемещение по полю ",
		"осуществляется с помощью клавиш со стрелками,выбранная клетка окрашивается в жёлтый, ",
		"для подтверждения хода в выбранной клетке нужно нажать \"Enter\", возвращение в ",
		"главное меню осуществляется по нажатию клавиши \"Esc\". Программа разработана в ",
		"качестве курсовой работы по дисциплине \"Программирование и основы алгоритмизации\"."
	};
	int xmax, ymax;

	// получение параметров окна 
	PCONSOLE_SCREEN_BUFFER_INFO pwi = new CONSOLE_SCREEN_BUFFER_INFO;
	PWINDOWINFO pgwi = new WINDOWINFO;
	GetConsoleScreenBufferInfo(hConsole, pwi);
	GetWindowInfo(hcon, pgwi);
	xmax = pwi->dwSize.X;
	ymax = pwi->dwSize.Y;

	int y0 = 2;
	for (int i = 0; i < k; i++)
	{
		position[i].X = (xmax - strlen(names[i])) / 2;
		position[i].Y = y0 + i;
	}
	SetConsoleTextAttribute(hConsole, (WORD)(Blue << 4));
	system("cls");
	SetConsoleTextAttribute(hConsole, (WORD)(Blue << 4) | Yellow);
	char title[] = "СПРАВКА";
	COORD pos;
	pos.X = (xmax - strlen(title)) / 2;
	pos.Y = 0;
	SetConsoleCursorPosition(hConsole, pos);
	puts(title);
	for (int i = 0; i < 9; i++)
	{
		SetConsoleCursorPosition(hConsole, position[i]);
		puts(names[i]);
	}
	printf("\n");
	system("pause");
	system("cls");
}

void AboutMe(HANDLE hConsole, HWND hcon)
{
	SetConsoleTextAttribute(hConsole, (WORD)(Blue << 4) | Yellow);
	system("cls");

	// программная установка консольного шрифта для отображения русского текста
	CONSOLE_FONT_INFO cfi;
	GetCurrentConsoleFont(hConsole, false, &cfi);
	COORD fs = GetConsoleFontSize(hConsole, cfi.nFont);
	PCONSOLE_FONT_INFOEX ccf = new CONSOLE_FONT_INFOEX;
	(*ccf).dwFontSize.X = 12;
	(*ccf).dwFontSize.Y = 20;
	(*ccf).nFont = 11;
	(*ccf).cbSize = sizeof(CONSOLE_FONT_INFOEX);
	ccf->FontWeight = 400;
	lstrcpyW((*ccf).FaceName, L"Lucida Console");
	(*ccf).FontFamily = FF_DONTCARE;
	bool b = SetCurrentConsoleFontEx(hConsole, false, ccf);
	fs = GetConsoleFontSize(hConsole, cfi.nFont);

	int k = 6; // кол-во пунктов
	COORD position[6];
	char names[6][200] = {
		"Реверси",
		"Курсовая работа по предмету:",
		"\"Программирование и основы алгоритмизации\"",
		"ИГЭУ 2021",
		"Разработал: Муравьёв М.П.",
		"Руководитель: Алыкова А.Л."
	};
	int xmax, ymax;

	// получение параметров окна 
	PCONSOLE_SCREEN_BUFFER_INFO pwi = new CONSOLE_SCREEN_BUFFER_INFO;
	PWINDOWINFO pgwi = new WINDOWINFO;
	GetConsoleScreenBufferInfo(hConsole, pwi);
	GetWindowInfo(hcon, pgwi);
	xmax = pwi->dwSize.X;
	ymax = pwi->dwSize.Y;

	int y0 = 2;
	for (int i = 0; i < k; i++)
	{
		position[i].X = (xmax - strlen(names[i])) / 2;
		position[i].Y = y0 + i;
	}
	SetConsoleTextAttribute(hConsole, (WORD)(Blue << 4));
	system("cls");
	SetConsoleTextAttribute(hConsole, (WORD)(Blue << 4) | Yellow);
	char title[] = "О ПРОГРАММЕ";
	COORD pos;
	pos.X = (xmax - strlen(title)) / 2;
	pos.Y = 0;
	SetConsoleCursorPosition(hConsole, pos);
	puts(title);
	for (int i = 0; i < 6; i++)
	{
		SetConsoleCursorPosition(hConsole, position[i]);
		puts(names[i]);
	}
	printf("\n");
	system("pause");
	system("cls");
}

int main()
{
	// получение информации о консольном окне
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	HWND hcon = GetConsoleWindow();
	HDC hdc = GetDC(hcon);

	int cc, countpl1, countpl2;// цвет фишек компьютера, счётчик игрока 1, счётчик игрока 2

	// программная установка консольного шрифта для отображения русского текста
	CONSOLE_FONT_INFO cfi;
	GetCurrentConsoleFont(hConsole, false, &cfi);
	COORD fs = GetConsoleFontSize(hConsole, cfi.nFont);
	PCONSOLE_FONT_INFOEX ccf = new CONSOLE_FONT_INFOEX;
	(*ccf).dwFontSize.X = 12;
	(*ccf).dwFontSize.Y = 20;
	(*ccf).nFont = 11;
	(*ccf).cbSize = sizeof(CONSOLE_FONT_INFOEX);
	ccf->FontWeight = 400;
	lstrcpyW((*ccf).FaceName, L"Lucida Console");
	(*ccf).FontFamily = FF_DONTCARE;
	bool b = SetCurrentConsoleFontEx(hConsole, false, ccf);
	fs = GetConsoleFontSize(hConsole, cfi.nFont);

	// установка кодировки 1251 для отображения русского текста
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	int k = 5;  //кол-во пунктов меню
	COORD position[5];
	int punkt = 0;
	char names[5][25] = { "Игра с компьютером", "Игра вдвоём", "Справка", "О программе", "Выход" };
	unsigned char ch;
	do
	{
		int xmax, ymax;

		// получение параметров окна 
		PCONSOLE_SCREEN_BUFFER_INFO pwi = new CONSOLE_SCREEN_BUFFER_INFO;
		PWINDOWINFO pgwi = new WINDOWINFO;
		GetConsoleScreenBufferInfo(hConsole, pwi);
		GetWindowInfo(hcon, pgwi);
		xmax = pwi->dwSize.X;
		ymax = pwi->dwSize.Y;
		
		int y0 = 7;
		for (int i = 0; i < k; i++)
		{
			position[i].X = (xmax - strlen(names[i])) / 2;
			position[i].Y = y0 + i;
		}
		SetConsoleTextAttribute(hConsole, (WORD)(Blue << 4)); // цвет фона
		system("cls");				// очистка окна
		SetConsoleTextAttribute(hConsole, (WORD)(Blue << 4) | Yellow); // цвет символов и цвет фона
		char title[] = "РЕВЕРСИ";
		COORD pos;
		pos.X = (xmax - strlen(title)) / 2;
		pos.Y = 5;
		SetConsoleCursorPosition(hConsole, pos);
		puts(title);
		for (int i = 0; i < 5; i++)
		{
			SetConsoleCursorPosition(hConsole, position[i]);
			puts(names[i]);
		}
		SetConsoleTextAttribute(hConsole, (WORD)(Blue << 4) | LightGreen);
		SetConsoleCursorPosition(hConsole, position[punkt]);		// выделение текущего пункта ярким цветом
		puts(names[punkt]);

		do
		{
			// обработка перемещения по меню клавишами со стрелками
			ch = _getch();
			if (ch == 224)
			{
				ch = _getch();
				switch (ch)
				{
				case 72:
					SetConsoleCursorPosition(hConsole, position[punkt]);
					SetConsoleTextAttribute(hConsole, (WORD)(Blue << 4) | Yellow);
					puts(names[punkt]);
					punkt--;
					if (punkt < 0) punkt = 4;
					SetConsoleTextAttribute(hConsole, (WORD)(Blue << 4) | LightGreen);
					SetConsoleCursorPosition(hConsole, position[punkt]);
					puts(names[punkt]); break;
				case 80:
					SetConsoleCursorPosition(hConsole, position[punkt]);
					SetConsoleTextAttribute(hConsole, (WORD)(Blue << 4) | Yellow);
					puts(names[punkt]);
					punkt++;
					if (punkt > 4) punkt = 0;
					SetConsoleTextAttribute(hConsole, (WORD)(Blue << 4) | LightGreen);
					SetConsoleCursorPosition(hConsole, position[punkt]);
					puts(names[punkt]); break;
				}
			}
		} while (ch != 13);		// enter - выбор пункта меню
		switch (punkt)
		{
		case 0:
			kind = 1;
			break;
		case 1:
			kind = 2;
			break;
		case 2:
			ReadMe(hConsole, hcon);		// вызов функции
			break;
		case 3:
			AboutMe(hConsole, hcon);	// вызов функции
			break;
		}
		if (punkt != 4 && punkt == 0 || punkt == 1) // если выбран пункт Игра с...
		{
			int k = 4, n; //k - кол-во пунктов, n - размер поля
			COORD pos[4];
			int pun = 0;
			char words[4][25] = { "4x4", "6x6", "8x8", "Назад" };
			do
			{
				int y0 = 7;
				for (int i = 0; i < k; i++)
				{
					pos[i].X = (xmax - strlen(words[i])) / 2;
					pos[i].Y = y0 + i;
				}
				SetConsoleTextAttribute(hConsole, (WORD)(Blue << 4));
				system("cls");
				SetConsoleTextAttribute(hConsole, (WORD)(Blue << 4) | Yellow);
				char tit[] = "Выберите размер игрового поля";
				COORD p;
				p.X = (xmax - strlen(tit)) / 2;
				p.Y = 5;
				SetConsoleCursorPosition(hConsole, p);
				puts(tit);
				for (int i = 0; i < 4; i++)
				{
					SetConsoleCursorPosition(hConsole, pos[i]);
					puts(words[i]);
				}
				SetConsoleTextAttribute(hConsole, (WORD)(Blue << 4) | LightGreen);
				SetConsoleCursorPosition(hConsole, pos[pun]);
				puts(words[pun]);
				do
				{
					// обработка перемещения по меню клавишами со стрелками
					ch = _getch();
					if (ch == 224)
					{
						ch = _getch();
						switch (ch)
						{
						case 72:
							SetConsoleCursorPosition(hConsole, pos[pun]);
							SetConsoleTextAttribute(hConsole, (WORD)(Blue << 4) | Yellow);
							puts(words[pun]);
							pun--;
							if (pun < 0) pun = 3;
							SetConsoleTextAttribute(hConsole, (WORD)(Blue << 4) | LightGreen);
							SetConsoleCursorPosition(hConsole, pos[pun]);
							puts(words[pun]); break;
						case 80:
							SetConsoleCursorPosition(hConsole, pos[pun]);
							SetConsoleTextAttribute(hConsole, (WORD)(Blue << 4) | Yellow);
							puts(words[pun]);
							pun++;
							if (pun > 3) pun = 0;
							SetConsoleTextAttribute(hConsole, (WORD)(Blue << 4) | LightGreen);
							SetConsoleCursorPosition(hConsole, pos[pun]);
							puts(words[pun]); break;
						}
					}
				} while (ch != 13); // enter - выбор пункта меню
				switch (pun) // установка размера поля
				{
				case 0:
					n = 4;
					break;
				case 1:
					n = 6;
					break;
				case 2:
					n = 8;
					break;
				}
				if (pun != 3) // если не выбран пункт "Назад"
				{
					GBoard = new int* [n]; //выделение памяти для игровой доски
					for (int i = 0; i < n; i++)
						GBoard[i] = new int[n];
					for (int i = 0; i < n; i++)
						for (int j = 0; j < n; j++)
							GBoard[i][j] = 0;
					GBoard[(n - 1) / 2][(n - 1) / 2] = 2; // установка фишек
					GBoard[(n - 1) / 2 + 1][(n - 1) / 2 + 1] = 2;
					GBoard[(n - 1) / 2][(n - 1) / 2 + 1] = 1;
					GBoard[(n - 1) / 2 + 1][(n - 1) / 2] = 1;
					brd2 = new int* [n]; // выделение памяти для вспомогательной доски
					for (int i = 0; i < n; i++)
						brd2[i] = new int[n];

					if (kind == 1) // игра с компьютером
					{
						int player1; // цвет фишек игрока
						srand(time(NULL));
						player1 = rand() % 2 + 1; // установка цвета фишек игрока
						if (player1 == 1) // определение цвета фишек противника
							cc = 2;
						else
							cc = 1;
						countpl1 = 0, countpl2 = 0;
						brd3 = new int* [n]; // выделение памяти для вспомогательных массивов
						for (int i = 0; i < n; i++)
							brd3[i] = new int[n];

						TheBestMove.brd = new int* [n];
						for (int i = 0; i < n; i++)
							TheBestMove.brd[i] = new int[n];

						Moves move_n[6];
						for (int l = 0; l < 6; l++)
						{
							move_n[l].brd = new int* [n];
							for (int i = 0; i < n; i++)
								move_n[l].brd[i] = new int[n];
						}

						PlayGameVSComp(n, n, cc, player1, move_n, 6, hConsole, hdc, countpl1, countpl2); //функция игры с компьютером

						for (int i = 0; i < n; i++) // очищение памяти
							delete[] GBoard[i];
						delete[] GBoard;

						for (int i = 0; i < n; i++)
							delete[] brd2[i];
						delete[] brd2;

						for (int i = 0; i < n; i++)
							delete[] brd3[i];
						delete[] brd3;

						for (int i = 0; i < n; i++)
							delete[] TheBestMove.brd[i];
						delete[] TheBestMove.brd;

						for (int i = 0; i < 6; i++)
						{
							for (int j = 0; j < n; j++)
								delete[] move_n[i].brd[j];
							delete[] move_n[i].brd;
						}

					}
					else
						if (kind == 2) // игра с человеком
						{
							countpl1 = 0, countpl2 = 0;
							PlayGame2players(n, n, hConsole, hdc, countpl1, countpl2); //функция игры с человеком

							for (int i = 0; i < n; i++) // очищение памяти
								delete[] GBoard[i];
							delete[] GBoard;

							for (int i = 0; i < n; i++)
								delete[] brd2[i];
							delete[] brd2;
						}
				}
			} while (pun != 0 && pun != 1 && pun != 2 && pun != 3);
		}
	} while (punkt != 4);
	return 0;
}
