#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <fstream>
#include <iomanip>
#include <algorithm>

using namespace std;

string toPattern(int x) {
    string s(3, '0');
    for (int i = 2; i >= 0; --i) {
        s[i] = char('0' + (x & 1));
        x >>= 1;
    }
    return s;
}

struct PairResult {
    string a;
    string b;
    double pA;
    double pB;
};

double simulatePair(int aPattern, int bPattern, int trials, uint64_t seed) {
    if (aPattern == bPattern) return 0.5;
    mt19937_64 rng(seed);
    uniform_int_distribution<int> bit(0, 1);
    int winsA = 0;
    for (int t = 0; t < trials; ++t) {
        int state = 0;
        int len = 0;
        while (true) {
            state = ((state << 1) & 7) | bit(rng);
            ++len;
            if (len >= 3) {
                if (state == aPattern) {
                    ++winsA;
                    break;
                }
                if (state == bPattern) {
                    break;
                }
            }
        }
    }
    return static_cast<double>(winsA) / trials;
}

int main() {
    const int TRIALS = 200000;
    vector<string> patterns;
    for (int i = 0; i < 8; ++i) patterns.push_back(toPattern(i));

    vector<vector<double>> probA(8, vector<double>(8, 0.0));
    vector<double> avgAsPatternA(8, 0.0), avgAsPatternB(8, 0.0);
    double overallA = 0.0;
    vector<PairResult> pairs;

    for (int b = 0; b < 8; ++b) {
        for (int a = 0; a < 8; ++a) {
            uint64_t seed = 1469598103934665603ULL ^ (static_cast<uint64_t>(a) << 32) ^ static_cast<uint64_t>(b) ^ 20260518ULL;
            double p = simulatePair(a, b, TRIALS, seed);
            probA[b][a] = p;
            avgAsPatternA[a] += p;
            avgAsPatternB[b] += (1.0 - p);
            overallA += p;
            pairs.push_back({patterns[a], patterns[b], p, 1.0 - p});
        }
    }

    for (int i = 0; i < 8; ++i) {
        avgAsPatternA[i] /= 8.0;
        avgAsPatternB[i] /= 8.0;
    }
    overallA /= 64.0;
    double overallB = 1.0 - overallA;

    ofstream csv("MonteCarlo_Table.csv");
    csv << "B\\A";
    for (const auto& p : patterns) csv << ',' << p;
    csv << "\n";
    csv << fixed << setprecision(6);
    for (int b = 0; b < 8; ++b) {
        csv << patterns[b];
        for (int a = 0; a < 8; ++a) csv << ',' << probA[b][a];
        csv << "\n";
    }
    csv.close();

    ofstream txt("MonteCarlo_ReportData.txt");
    txt << fixed << setprecision(6);
    txt << "Лабораторная работа №10. Вариант 1 — Метод Монте-Карло\n\n";
    txt << "Количество симуляций для каждой пары комбинаций: " << TRIALS << "\n\n";
    txt << "Таблица вероятностей победы игрока A над игроком B\n";
    txt << setw(8) << "B\\A";
    for (const auto& p : patterns) txt << setw(10) << p;
    txt << "\n";
    for (int b = 0; b < 8; ++b) {
        txt << setw(8) << patterns[b];
        for (int a = 0; a < 8; ++a) txt << setw(10) << probA[b][a];
        txt << "\n";
    }
    txt << "\nСредний шанс победы игрока A вне зависимости от комбинаций: " << overallA << "\n";
    txt << "Средний шанс победы игрока B вне зависимости от комбинаций: " << overallB << "\n\n";

    txt << "Средняя вероятность победы комбинации, если ее выбирает игрок A:\n";
    for (int i = 0; i < 8; ++i) txt << patterns[i] << ": " << avgAsPatternA[i] << "\n";
    txt << "\nСредняя вероятность победы комбинации, если ее выбирает игрок B:\n";
    for (int i = 0; i < 8; ++i) txt << patterns[i] << ": " << avgAsPatternB[i] << "\n";

    auto maxIt = max_element(pairs.begin(), pairs.end(), [](const PairResult& x, const PairResult& y) { return x.pA < y.pA; });
    auto minIt = min_element(pairs.begin(), pairs.end(), [](const PairResult& x, const PairResult& y) { return x.pA < y.pA; });
    txt << "\nНаиболее выгодная пара для игрока A: A=" << maxIt->a << ", B=" << maxIt->b << ", P(A)=" << maxIt->pA << "\n";
    txt << "Наиболее невыгодная пара для игрока A: A=" << minIt->a << ", B=" << minIt->b << ", P(A)=" << minIt->pA << "\n";
    txt.close();

    cout << fixed << setprecision(6);
    cout << "Таблица вероятностей победы игрока A над игроком B\n";
    cout << setw(8) << "B\\A";
    for (const auto& p : patterns) cout << setw(10) << p;
    cout << "\n";
    for (int b = 0; b < 8; ++b) {
        cout << setw(8) << patterns[b];
        for (int a = 0; a < 8; ++a) cout << setw(10) << probA[b][a];
        cout << "\n";
    }
    cout << "\nСредний шанс победы игрока A: " << overallA << "\n";
    cout << "Средний шанс победы игрока B: " << overallB << "\n";
    cout << "Данные сохранены в MonteCarlo_Table.csv и MonteCarlo_ReportData.txt\n";

    return 0;
}
