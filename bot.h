#pragma once
#include <iostream>
#include <fstream>
int bot() {
    std::ifstream fin("field.txt");
    std::ofstream fout("command.txt", std::ios_base::trunc);
    if (!fin.is_open()) {
        std::cout << "Файл field.txt не существует";
        return 1;
    }
    if (!fout.is_open()) {
        std::cout << "Файл command.txt не существует";
        return 1;
    }
    int n, m, k;
    fin >> n >> m >> k;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            int x;
            fin >> x;
            if (x == -2) {
                fout << "\\o " << i + 1 << " " << j + 1;
                fin.close();
                fout.close();
            }
        }
    }
    return 0;
}