#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#endif

#include <cstdlib>
#include <thread>
#include <chrono>
#include "bot.h"

int field[100][100];
// -1 - mine
// 0-8 - cell
int field_player[100][100];
// -2 - closed
// -1 - flag
// 0-8 - opened
const int N = 11, M = 11, K = 10;
bool is_bot = false;

class sleep;

void print_field() {
    #if defined(_WIN32) || defined(_WIN64)
        system ("cls");
    #else
        system("clear");
    #endif
    std::cout << "  ";
    for (int i = 0; i < M; ++i) {
        std::cout << " " << i + 1;
        if (i+ 1 < 10) std::cout << " ";
    }
    std::cout << std::endl;
    for (int i = 0; i < N; i++) {
        std::cout << i + 1 << " ";
        if (i+ 1 < 10) std::cout << " ";
        for (int j = 0; j < M; j ++) {
            if (field_player[i][j] == -2) std::cout << "_  ";
            else if (field_player[i][j] == -1) std::cout << "f  ";
            else if (field_player[i][j] == -3) std::cout << "*  ";
            else std::cout << field_player[i][j] << "  ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl << std::endl;
}

void dfs(int x, int y) {
    if (x < 0 || x >= N || y < 0 || y >= M || field_player[x][y] > -2) return;
    field_player[x][y] = field[x][y];
    if (field[x][y] > 0) return;
    for (int i2 = x - 1; i2 <= x + 1; ++i2) {
        for (int j2 = y - 1; j2 <= y + 1; ++j2) {
            if (i2 != x || j2 != y) dfs(i2, j2);
        }
    }
}
// true не взорвались
// false взорвались
bool open_cell(int x, int y) {
    if (field[x][y] == -1) return false;
    if (field[x][y] > 0) {
        field_player[x][y] = field[x][y];
        return true;
    }
    dfs(x, y);
    return true;
}

bool is_win() {
    int opened = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            if (field_player[i][j] >= 0) opened++;
        }
    }
    return (N * M - K == opened);
}

void end_game(bool is_win = false) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            if (field_player[i][j] == -1) field_player[i][j] = -2;
            if (field[i][j] == -1) field_player[i][j] = -3;
        }
    }
    print_field();
    std::cout << "Вы " << (is_win? "ВЫИГРАЛИ!!!\n" : "ПРОИГРАЛИ!\n")
              << "Что бы начать новую игру введите любую строку: " << std::endl;
    std::string s;
    std::cin >> s;
    is_bot = false;
}

void wait(int msec) {
    std::this_thread::sleep_for(std::chrono::milliseconds(msec));
}

void safe_field() {
    std::ofstream fout("field.txt", std::ios_base::trunc);
    if (!fout.is_open()){
        std::cout << "Файл field.txt не существует" << std::endl;
        exit(0);
    }
    fout << N << " " << M << " " << K << std::endl;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            fout << field_player[i][j] << " ";
        }
        fout << std::endl;
    }
    fout .close();
}
int main() {
    #if defined(_WIN32) || defined(_WIN64)
        SetConsoleCP(1251);
        SetConsoleOutputCP(1251);
    #endif

    while (true) {
        srand(time(0));
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                field[i][j] = 0;
                field_player[i][j] = -2;
            }
        }
        for (int i = 0; i < K; ++i) {
            while (true) {
                int x = rand() % N;
                int y = rand() % M;
                if (field[x][y] != -1) {
                    field[x][y] = -1;
                    break;
                }
            }
        }
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                if (field[i][j] != -1) {
                    field[i][j] = 0;
                    for (int i2 = i - 1; i2 <= i + 1; ++i2) {
                        for (int j2 = j - 1; j2 <= j + 1; ++j2) {
                            if (i2 >= 0 && i2 < N && j2 >= 0 && j2 < M && field[i2][j2] == -1) ++field[i][j];
                        }
                    }
                }
            }
        }
        while (true) {
            print_field();
            std::cout << "Введите команду: " << std::endl;
            /*
             \o x, y - открыть клетку с координатами x и y;
             \f x y - поставить флаг на клетку с координатами x и y;
             \n - начать новую игру;
             \q - выйти из игры;
             \b - играет бот;
             */
            std::string command;
            std::fstream fin;
            if (is_bot) {
                safe_field();
                bot();
                fin.open("command.txt");
                if (!fin.is_open()){
                    std::cout << "Файл command.txt не существует" << std::endl;
                    return 0;
                }
                fin >> command;
                std::cout << command << " ";
            } else {

                std::cin >> command;
            }
            if (command == "\\o") {
                int x, y;
                if (is_bot) {
                    fin >> x >> y;
                    std::cout << x << " " << y << std::endl;
                    wait(1000);
                } else {
                    std::cout << "Введите координаты: ";
                    std::cin >> x >> y;
                }
                --x; --y;
                if (x < 0 || x >= N || y < 0 || y >= M || field_player[x][y] >= 0) continue;
                if (!open_cell(x, y)) {
                    end_game();
                    break;
                }
                if(is_win()) {
                    end_game(true);
                    break;
                }
                // открытие клеток
                // ....
            } else if (command == "\\f") {
                int x, y;
                if (is_bot) {
                    fin >> x >> y;
                    std::cout << x << " " << y << std::endl;
                    wait(1000);
                } else {
                    std::cout << "Введите координаты";
                    std::cin >> x >> y;
                }
                --x; --y;
                if (x < 0 || x >= N || y < 0 || y >= M || field_player[x][y] >= 0) continue;
                if (field_player[x][y] == -1) field_player[x][y] = -2;
                else field_player[x][y] = -1;
            } else if (command == "\\n") {
                break;
            } else if (command == "\\b") {
                is_bot = true;
                break;
            } else if (command == "\\q") {
                return 0;
            }
        }
    }
}
