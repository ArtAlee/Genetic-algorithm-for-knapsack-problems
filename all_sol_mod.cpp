#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <numeric>
#include <thread>
#include <mutex>
#include <queue>

using namespace std;
using namespace chrono;

mutex mtx;


const long long A_MAX_1 = static_cast<long long>(pow(2, 24/0.8));
const long long A_MAX_2 = static_cast<long long>(pow(2, 24));
const long long A_MAX_3 = static_cast<long long>(pow(2, 24/1.2));
const long long A_MAX_4 = static_cast<long long>(pow(2, 24/1.4));
const vector<long long> A_MAX_VALUES = {A_MAX_1, A_MAX_2, A_MAX_3, A_MAX_4};

vector<string> split(const string& s, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

vector<vector<long long>> load_problems(const string& filename) {
    vector<vector<long long>> problems;
    ifstream file(filename);
    string line;

    if (file.is_open()) {
        while (getline(file, line)) {
            vector<string> tokens = split(line, ',');
            vector<long long> problem;
            for (const string& token : tokens) {
                problem.push_back(stoll(token));
            }
            problems.push_back(problem);
        }
        file.close();
    } else {
        cerr << "Не удалось открыть файл: " << filename << endl;
    }
    return problems;
}

tuple<double, double, int> solve_knapsack_bruteforce(const vector<long long>& items, long long target_weight, long long a_max) {
    int n = items.size() - 1;
    vector<long long> weights(items.begin(), items.end() - 1);

    double first_solution_time = 0.0;
    int solutions_count = 0;

    auto start_time = high_resolution_clock::now();

    for (int r = 1; r <= n; ++r) {
        vector<bool> v(n);
        fill(v.begin(), v.begin() + r, true);
        do {
            long long current_sum = 0;
            for (int i = 0; i < n; ++i) {
                if (v[i]) {
                    current_sum = (current_sum + weights[i]) % a_max;
                }
            }
            if (current_sum == target_weight % a_max) {
                solutions_count++;
                if (first_solution_time == 0.0) {
                    first_solution_time = duration<double>(high_resolution_clock::now() - start_time).count();
                }
            }
        } while (prev_permutation(v.begin(), v.end()));
    }

    double all_solutions_time = duration<double>(high_resolution_clock::now() - start_time).count();
    return make_tuple(first_solution_time, all_solutions_time, solutions_count);
}

struct Result {
    int problem_number;
    double first_solution_time;
    double all_solutions_time;
    int solutions_count;
};

void worker(queue<int>& problem_indices, const vector<vector<long long>>& problems, vector<Result>& results, long long a_max) {
    while (true) {
        int problem_index;
        {
            lock_guard<mutex> lock(mtx);
            if (problem_indices.empty()) break;
            problem_index = problem_indices.front();
            problem_indices.pop();
        }

        long long target_weight = problems[problem_index].back();
        auto [first_time, all_time, solution_count] = solve_knapsack_bruteforce(problems[problem_index], target_weight, a_max);

        lock_guard<mutex> lock(mtx);
        results[problem_index] = {problem_index + 1, first_time, all_time, solution_count};
        cout << "Задача " << (problem_index + 1) << " решена: найдено " << solution_count << " решений" << endl;
    }
}

void save_results(const vector<Result>& results, const string& filename) {
    ofstream file(filename);
    if (file.is_open()) {
        file << "Problem Number,First Solution Time (s),All Solutions Time (s),Number of Solutions\n";
        for (const auto& result : results) {
            file << result.problem_number << ","
                 << (result.first_solution_time > 0 ? to_string(result.first_solution_time) : "N/A") << ","
                 << result.all_solutions_time << ","
                 << result.solutions_count << "\n";
        }
        file.close();
    } else {
        cerr << "Не удалось открыть файл: " << filename << endl;
    }
}

int main() {
    for (int i = 5; i <= 8; ++i) {
        string input_filename = "knapsack_problems_" + to_string(i) + ".csv";
        string output_filename = "knapsack_solutions_" + to_string(i) + ".csv";

        vector<vector<long long>> problems = load_problems(input_filename);
        vector<Result> results(problems.size());

        queue<int> problem_indices;
        for (size_t j = 0; j < problems.size(); ++j) {
            problem_indices.push(j);
        }

        unsigned int num_threads = thread::hardware_concurrency();
        if (num_threads == 0) num_threads = 4;
        cout << "Обрабатываем " << input_filename << " с использованием " << num_threads << " потоков и A_MAX = " 
             << A_MAX_VALUES[i-5] << endl;

        vector<thread> threads;
        for (unsigned int j = 0; j < num_threads; ++j) {
            threads.emplace_back(worker, ref(problem_indices), cref(problems), ref(results), A_MAX_VALUES[i-5]);
        }

        for (auto& t : threads) {
            t.join();
        }

        save_results(results, output_filename);
        cout << "Результаты сохранены в " << output_filename << endl;
    }

    return 0;
}
