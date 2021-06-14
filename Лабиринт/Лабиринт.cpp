#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <conio.h>
#include <fstream>

#define field_space 21
#define debug 0

int generator = 0;
// 0 - без рандома
// 1 - рандом
int type_game = 0;
// 0 - стандарт
// 1 - не виден игрок
int layout = 0;
// 0 - нампад (4 8 2 6)
// 1 - стрелочки

using namespace std;
int point_player_x = 0, point_player_y = 0;
int field[field_space][field_space];
int player_field[field_space][field_space];
int score = 0;
int steps = 0;
int score_out = 0;
int steps_out = 0;

bool deadend(int, int, int**, int, int); // Вспомогательная функция, определяет тупики
void visual(int**, int, int); // Изображение результата с помощью консольной графики
void mazemake(int**, int, int); // Собственно алгоритм
const int wall_l = 0, pass = 1;

int getRandomNumber(int min, int max)
{
    static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
    // Равномерно распределяем рандомное число в нашем диапазоне
    return static_cast<int>(rand() * fraction * (max - min + 1) + min);
}

class Walls
{
public:
    Walls()
    {
        x = 0;
        y = 0;
    }
    void wall_input_x(int x_t)
    {
        x = x_t;
    }
    void wall_input_y(int y_t)
    {
        y = y_t;
    }
    int wall_out_x()
    {
        return x;
    }
    int wall_out_y()
    {
        return y;
    }
private:
    int x;
    int y;
};
vector<Walls> wall;

class Games
{
public:
    void steps_change(int step_t)
    {
        steps = step_t;
    }
    int steps_out()
    {
        return steps;
    }
private:
    int steps;
};
vector<Games> games;
vector<Games> old_games;

bool deadend(int x, int y, int** maze, int height, int width) {
    int a = 0;

    if (x != 1) {
        if (maze[y][x - 2] == pass)
            a += 1;
    }
    else a += 1;

    if (y != 1) {
        if (maze[y - 2][x] == pass)
            a += 1;
    }
    else a += 1;

    if (x != width - 2) {
        if (maze[y][x + 2] == pass)
            a += 1;
    }
    else a += 1;

    if (y != height - 2) {
        if (maze[y + 2][x] == pass)
            a += 1;
    }
    else a += 1;

    if (a == 4)
        return 1;
    else
        return 0;
}

void mazemake(int** maze, int height, int width) {
    int x, y, c, a;
    bool b;

    for (int i = 0; i < height; i++) // Массив заполняется землей-ноликами
        for (int j = 0; j < width; j++)
            maze[i][j] = wall_l;

    x = 3; y = 3; a = 0; // Точка приземления крота и счетчик
    while (a < 10000) { // Да, простите, костыль, иначе есть как, но лень
        maze[y][x] = pass; a++;
        while (1) { // Бесконечный цикл, который прерывается только тупиком
            c = rand() % 4; // Напоминаю, что крот прорывает
            switch (c) {  // по две клетки в одном направлении за прыжок
            case 0: if (y != 1)
                if (maze[y - 2][x] == wall_l) { // Вверх
                    maze[y - 1][x] = pass;
                    maze[y - 2][x] = pass;
                    y -= 2;
                }
            case 1: if (y != height - 2)
                if (maze[y + 2][x] == wall_l) { // Вниз
                    maze[y + 1][x] = pass;
                    maze[y + 2][x] = pass;
                    y += 2;
                }
            case 2: if (x != 1)
                if (maze[y][x - 2] == wall_l) { // Налево
                    maze[y][x - 1] = pass;
                    maze[y][x - 2] = pass;
                    x -= 2;
                }
            case 3: if (x != width - 2)
                if (maze[y][x + 2] == wall_l) { // Направо
                    maze[y][x + 1] = pass;
                    maze[y][x + 2] = pass;
                    x += 2;
                }
            }
            if (deadend(x, y, maze, height, width))
                break;
        }

        if (deadend(x, y, maze, height, width)) // Вытаскиваем крота из тупика
            do {
                x = 2 * (rand() % ((width - 1) / 2)) + 1;
                y = 2 * (rand() % ((height - 1) / 2)) + 1;
            } while (maze[y][x] != pass);
    } // На этом и все.
}

void Lab_Generate()
{
    srand((unsigned)time(NULL));

    int height = 21, width = 21;

    int** maze = new int* [height];
    for (int i = 0; i < height; i++)
        maze[i] = new int[width];

    mazemake(maze, height, width);

    Walls* wall_temp = new Walls();
    for (int i = 0; i < 21; i++)
    {
        for (int j = 0; j < 21; j++)
        {
            if (maze[i][j] == 0)
            {
                wall_temp->wall_input_x(i);
                wall_temp->wall_input_y(j);
                wall.push_back(*wall_temp);
            }
        }
    }

    for (int i = 0; i < height; i++)
        delete[] maze[i];
    delete[] maze;
}

void Wall_Add()
{
    Walls* wall_temp = new Walls();
    for (int j = 0; j < field_space; j++)
    {
        wall_temp->wall_input_x(0);
        wall_temp->wall_input_y(j);
        wall.push_back(*wall_temp);
    }
    for (int j = 0; j < field_space; j++)
    {
        wall_temp->wall_input_x(field_space - 1);
        wall_temp->wall_input_y(j);
        wall.push_back(*wall_temp);
    }
    for (int i = 0; i < field_space; i++)
    {
        wall_temp->wall_input_x(i);
        wall_temp->wall_input_y(0);
        wall.push_back(*wall_temp);
    }
    for (int i = 0; i < field_space; i++)
    {
        wall_temp->wall_input_x(i);
        wall_temp->wall_input_y(field_space - 1);
        wall.push_back(*wall_temp);
    }
}

void Field_Fill()
{
	for (int i = 0; i < field_space; i++)
	{
		for (int j = 0; j < field_space; j++)
		{
			field[i][j] = 0;
		}
	}
    for (int i = 0; i < field_space; i++)
    {
        for (int j = 0; j < field_space; j++)
        {
            for (int k = 0; k < wall.size(); k++)
            {
                if (wall[k].wall_out_x() == j && wall[k].wall_out_y() == i)
                {
                    field[i][j] = 1;
                }
            }
        }
    }
    for (int i = 0; i < field_space; i++)
    {
        for (int j = 0; j < field_space; j++)
        {
            player_field[i][j] = 0;
        }
    }
}

void Field_Out()
{
    for (int i = 0; i < field_space; i++)
    {
        for (int j = 0; j < field_space; j++)
        {
            if (field[i][j] != 0)
            {
                if (field[i][j] == 1)
                {
                    cout << "#";
                }
                else if (field[i][j] == 5)
                {
                    cout << "%";
                }
                else
                {
                    cout << field[i][j];
                }
            }
            else
            {
                if (type_game != 1)
                {
                    if (player_field[i][j] == 2)
                    {
                        cout << "$";
                    }
                    else
                    {
                        cout << " ";
                    }
                }
                else
                {
                    cout << " ";
                }
            }
            if (j != field_space - 1)
            {
                cout << " ";
            }
            else if (j == field_space - 1)
            {
                cout << endl;
            }
        }
    }
    cout << endl;
    cout << "Score: " << score << endl;
    cout << "Steps: " << steps << endl;
    if (games.size() != 0)
    {
        cout << "Games: " << endl;
        for (int i = 0; i < games.size(); i++)
        {
            cout << "Game #" << i << " Steps: " << games[i].steps_out() << endl;
        }
    }
}

void Player_Exit_Generate()
{
    int temp_int;
    srand((unsigned)time(NULL));
    temp_int = getRandomNumber(1, 4);
    switch (temp_int)
    {
    case 1:
        player_field[1][1] = 2;
        point_player_x = 1;
        point_player_y = 1;
        field[19][19] = 5;
        break;
    case 2:
        player_field[19][19] = 2;
        point_player_x = 19;
        point_player_y = 19;
        field[1][1] = 5;
        break;
    case 3:
        player_field[1][19] = 2;
        point_player_x = 19;
        point_player_y = 1;
        field[19][1] = 5;
        break;
    case 4:
        player_field[19][1] = 2;
        point_player_x = 1;
        point_player_y = 19;
        field[1][19] = 5;
        break;
    }
}

void Player_Generate()
{
    player_field[1][1] = 2;
    point_player_x = 1;
    point_player_y = 1;
}

void Exit_Generate()
{
    field[19][19] = 5;
}

int Player_Check_Wall(int num)
{
    int ok = 1;
    int temp_x = 0, temp_y = 0;
    temp_x = point_player_x;
    temp_y = point_player_y;
    switch (num)
    {
    case 1:
        temp_y = temp_y - 1;
        for (int i = 0; i < wall.size(); i++)
        {
            if (temp_y == wall[i].wall_out_y())
            {
                if (temp_x == wall[i].wall_out_x())
                {
                    ok = 0;
                }
            }
        }
        break;
    case 2:
        temp_y = temp_y + 1;
        for (int i = 0; i < wall.size(); i++)
        {
            if (temp_y == wall[i].wall_out_y())
            {
                if (temp_x == wall[i].wall_out_x())
                {
                    ok = 0;
                }
            }
        }
        break;
    case 3:
        temp_x = temp_x + 1;
        for (int i = 0; i < wall.size(); i++)
        {
            if (temp_x == wall[i].wall_out_x())
            {
                if (temp_y == wall[i].wall_out_y())
                {
                    ok = 0;
                }
            }
        }
        break;
    case 4:
        temp_x = temp_x - 1;
        for (int i = 0; i < wall.size(); i++)
        {
            if (temp_x == wall[i].wall_out_x())
            {
                if (temp_y == wall[i].wall_out_y())
                {
                    ok = 0;
                }
            }
        }
        break;
    }
    return ok;
}

int Player_Check_Exit()
{
    int ok = 1;
    int temp_x = 0, temp_y = 0;
    temp_x = point_player_x;
    temp_y = point_player_y;
    if (temp_x == 19 && temp_y == 19)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void OutSettings()
{
    ofstream output("settings.dat");
    if (output)
    {
        output << generator << endl;
        output << type_game << endl;
        output << layout << endl;
    }
    output.close();
}

void InputSettings()
{
    int temp_int;
    ifstream input("settings.dat");
    if (input)
    {
            input >> generator;
            input >> type_game;
            input >> layout;
    }
    input.close();
}

void OutProgress()
{
    ofstream output("out.dat", std::ios_base::app);
    if (output)
    {
        output << endl;
        output << score_out << endl;
        output << steps_out;
    }
    output.close();
}

void InputProgress()
{
    int temp_int;
    int quantity = 0;
    ifstream input("out.dat");
    if (input)
    {
        while (!input.eof())
        {
            cout << "Game #" << quantity << ": ";
            input >> temp_int;
            cout << " Score: " << temp_int;
            input >> temp_int;
            cout << " Steps: " << temp_int << endl;
            quantity = quantity + 1;
        }
    }
    input.close();
}

void Regenerate_Game()
{
    system("cls");
    wall.clear();
    Games* games_temp = new Games();
    games_temp->steps_change(steps);
    games.push_back(*games_temp);
    score = score + 1;
    steps_out = steps + steps_out;
    steps = 0;
    Lab_Generate();
    Field_Fill();
    if (generator == 0)
    {
        Player_Generate();
        Exit_Generate();
    }
    else
    {
        Player_Exit_Generate();
    }
    Field_Out();
}

void Yminusone()
{
    if (Player_Check_Wall(1) == 1)
    {
        system("cls");
        player_field[point_player_y][point_player_x] = 0;
        point_player_y = point_player_y - 1;
        player_field[point_player_y][point_player_x] = 2;
        steps = steps + 1;
        Field_Out();
    }
    if (Player_Check_Exit() == 1)
    {
        system("cls");
        Regenerate_Game();
    }
}

void Yplusone()
{
    if (Player_Check_Wall(2) == 1)
    {
        system("cls");
        player_field[point_player_y][point_player_x] = 0;
        point_player_y = point_player_y + 1;
        player_field[point_player_y][point_player_x] = 2;
        steps = steps + 1;
        Field_Out();
    }
    if (Player_Check_Exit() == 1)
    {
        system("cls");
        Regenerate_Game();
    }
}

void Xplusone()
{
    if (Player_Check_Wall(3) == 1)
    {
        system("cls");
        player_field[point_player_y][point_player_x] = 0;
        point_player_x = point_player_x + 1;
        player_field[point_player_y][point_player_x] = 2;
        steps = steps + 1;
        Field_Out();
    }
    if (Player_Check_Exit() == 1)
    {
        system("cls");
        Regenerate_Game();
    }
}

void Xminusone()
{
    if (Player_Check_Wall(4) == 1)
    {
        system("cls");
        player_field[point_player_y][point_player_x] = 0;
        point_player_x = point_player_x - 1;
        player_field[point_player_y][point_player_x] = 2;
        steps = steps + 1;
        Field_Out();
    }
    if (Player_Check_Exit() == 1)
    {
        system("cls");
        Regenerate_Game();
    }
}

void Player_Move()
{
    string one, two, thre, four;
    char symbol = _getch();
    while ((symbol = _getch())) 
    {
        if (layout == 0)
        {
            switch (symbol) {
            case 56:
                Yminusone();
                break;
            case 50:
                Yplusone();
                break;
            case 54:
                Xplusone();
                break;
            case 52:
                Xminusone();
                break;
            default:
                break;
            }
        }
        if (layout == 1)
        {
            switch (symbol) {
            case 72:
                Yminusone();
                break;
            case 80:
                Yplusone();
                break;
            case 77:
                Xplusone();
                break;
            case 75:
                Xminusone();
                break;
            default:
                break;
            }
        }
        if (symbol == 53)
        {
            score_out = score;
            OutProgress();
            break;
        }
    }
}

void Difficult_Menu()
{
    int temp_int, ok = 0;
    string temp_str;
    cout << "Текущий уровень сложности: ";
    if (type_game == 0)
    {
        cout << "низкая сложность" << endl;
    }
    else if (type_game == 1)
    {
        cout << "высокая сложность" << endl;
    }
    if (type_game == 0)
    {
        cout << "1 - Сменить сложность на повышенную" << endl;
    }
    if (type_game == 1)
    {
        cout << "1 - Сменить сложность на пониженную" << endl;
    }
    cout << "0 - Выйти из главное меню" << endl;
    cout << "Пожалуйста, выберите пунт меню: ";
    do {
        cin >> temp_str;
        temp_int = atoi(temp_str.c_str());
        cout << endl;
        if (temp_int >= 0 && temp_int <= 1)
        {
            ok = 1;
        }
        else
        {
            cout << "Пожалуйста, повторите попытку: ";
            ok = 0;
        }
    } while (ok != 1);
    switch (temp_int)
    {
    case 0:
        break;
    case 1:
        if (type_game == 0)
        {
            type_game = 1;
            break;
        }
        if (type_game == 1)
        {
            type_game = 0;
            break;
        }
        break;
    }
}

void Random_Menu()
{
    int temp_int, ok = 0;
    string temp_str;
    cout << "Текущий тип: ";
    if (type_game == 0)
    {
        cout << "без рандома" << endl;
    }
    else if (type_game == 1)
    {
        cout << "с рандомом" << endl;
    }
    if (type_game == 0)
    {
        cout << "1 - Включить рандом" << endl;
    }
    if (type_game == 1)
    {
        cout << "1 - Выключить рандом" << endl;
    }
    cout << "0 - Выйти из главное меню" << endl;
    cout << "Пожалуйста, выберите пунт меню: ";
    do {
        cin >> temp_str;
        temp_int = atoi(temp_str.c_str());
        cout << endl;
        if (temp_int >= 0 && temp_int <= 1)
        {
            ok = 1;
        }
        else
        {
            cout << "Пожалуйста, повторите попытку: ";
            ok = 0;
        }
    } while (ok != 1);
    switch (temp_int)
    {
    case 0:
        break;
    case 1:
        switch (layout)
        {
        case 0:
            generator = 1;
            break;
        case 1:
            generator = 0;
            break;
        }
        break;
    }
}

void Layout_Menu()
{
    int temp_int, ok = 0;
    string temp_str;
    cout << "Текущий тип: ";
    if (layout == 0)
    {
        cout << "нампад (4 8 2 6)" << endl;
    }
    else if (layout == 1)
    {
        cout << "стрелочки" << endl;
    }
    if (layout == 0)
    {
        cout << "1 - Включить стрелочки" << endl;
    }
    if (layout == 1)
    {
        cout << "1 - Включить нампад" << endl;
    }
    cout << "0 - Выйти из главное меню" << endl;
    cout << "Пожалуйста, выберите пунт меню: ";
    do {
        cin >> temp_str;
        temp_int = atoi(temp_str.c_str());
        cout << endl;
        if (temp_int >= 0 && temp_int <= 1)
        {
            ok = 1;
        }
        else
        {
            cout << "Пожалуйста, повторите попытку: ";
            ok = 0;
        }
    } while (ok != 1);
    switch (temp_int)
    {
    case 0:
        break;
    case 1:
        switch (layout)
        {
        case 0:
            layout = 1;
            break;
        case 1:
            layout = 0;
            break;
        }
        break;
    }
}

void Settings_Menu()
{
    int temp_int, ok = 0;
    string temp_str;
    cout << "Добро пожаловать в меню настроек игры Лабиринт" << endl;
    cout << "1 - Настройка сложности" << endl;
    cout << "2 - Настройка рандома" << endl;
    cout << "3 - Настройки раскладки" << endl;
    cout << "9 - Сохранить настройки" << endl;
    cout << "0 - Выйти из главное меню" << endl;
    cout << "Пожалуйста, выберите пунт меню: ";
    do {
        cin >> temp_str;
        temp_int = atoi(temp_str.c_str());
        cout << endl;
        if (temp_int >= 0 && temp_int <= 3 || temp_int == 9)
        {
            ok = 1;
        }
        else
        {
            cout << "Пожалуйста, повторите попытку: ";
            ok = 0;
        }
    } while (ok != 1);
    switch (temp_int)
    {
    case 0:
        break;
    case 1:
        Difficult_Menu();
        Settings_Menu();
        break;
    case 2:
        Random_Menu();
        Settings_Menu();
        break;
    case 3:
        Layout_Menu();
        Settings_Menu();
        break;
    case 9:
        break;
    }
}

void StartGame()
{
    Lab_Generate();
    Field_Fill();
    if (generator == 0)
    {
        Player_Generate();
        Exit_Generate();
    }
    else
    {
        Player_Exit_Generate();
    }
    system("cls");
    Field_Out();
    Player_Move();
}

void Menu()
{
    int temp_int, ok = 0;
    string temp_str;
    cout << "Добро пожаловать в меню игры Лабиринт" << endl;
    cout << "Перед началом игры, советуем провести настройки" << endl;
    cout << "1 - Настройки" << endl;
    cout << "2 - Начать игру" << endl;
    cout << "3 - Посмотреть рекорды" << endl;
    cout << "0 - Выйти из меню" << endl;
    cout << "Пожалуйста, выберите пунт меню: ";
    do {
        cin >> temp_str;
        temp_int = atoi(temp_str.c_str());
        cout << endl;
        if (temp_int >= 0 && temp_int <= 3)
        {
            ok = 1;
        }
        else
        {
            cout << "Пожалуйста, повторите попытку: ";
            ok = 0;
        }
    } while (ok != 1);
    switch (temp_int)
    {
    case 0:
        exit(0);
        break;
    case 1:
        Settings_Menu();
        Menu();
        break;
    case 2:
        StartGame();
        Menu();
        break;
    case 3:
        InputProgress();
        Menu();
        break;
    }
}

int main()
{
	setlocale(LC_ALL, "Russian");
    InputSettings();
    Menu();
	system("pause");
	return 0;
}