#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <numeric>
#include <thread>
#include <mutex>
#include <queue>

using namespace std;


mutex mtx;


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
        cerr << "Unable to open file: " << filename << endl;
    }
    return problems;
}


tuple<double, double, int> solve_knapsack_bruteforce(const vector<long long>& items, long long target_weight) {
    int n = items.size() - 1;
    vector<long long> weights(items.begin(), items.end() - 1);
    double first_solution_time = 0.0;
    double all_solutions_time = 0.0;
    int solutions_count = 0;
    clock_t start_time = clock();


    for (int r = 1; r <= n; ++r) {
        vector<bool> v(n);
        fill(v.begin(), v.begin() + r, true);
        do {
            long long current_sum = 0;
            for (int i = 0; i < n; ++i) {
                if (v[i]) {
                    current_sum += weights[i];
                }
            }
            if (current_sum == target_weight) {
                solutions_count++;
                if (first_solution_time == 0.0) {
                    first_solution_time = static_cast<double>(clock() - start_time) / CLOCKS_PER_SEC;
                }
            }
        } while (prev_permutation(v.begin(), v.end()));
    }

    all_solutions_time = static_cast<double>(clock() - start_time) / CLOCKS_PER_SEC;
    return make_tuple(first_solution_time, all_solutions_time, solutions_count);
}


struct Result {
    int problem_number;
    double first_solution_time;
    double all_solutions_time;
    int solutions_count;
};


void worker(queue<int>& problem_indices, const vector<vector<long long>>& problems, vector<Result>& results) {
    while (true) {
        int problem_index;
        {
            lock_guard<mutex> lock(mtx);
            if (problem_indices.empty()) break;
            problem_index = problem_indices.front();
            problem_indices.pop();
        }

        long long target_weight = problems[problem_index].back();
        auto [first_time, all_time, solution_count] = solve_knapsack_bruteforce(problems[problem_index], target_weight);

        lock_guard<mutex> lock(mtx);
        results[problem_index] = {problem_index + 1, first_time, all_time, solution_count};
        cout << "Problem " << (problem_index + 1) << "/" << problems.size() << " solved: " << solution_count << " solutions found (File: " << problem_indices.size() << " remaining)" << endl;
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
        cerr << "Unable to open file: " << filename << endl;
    }
}

int main() {

    const int num_files = 4;
    vector<string> input_files = {"knapsack_problems_1.csv", "knapsack_problems_2.csv",
                                 "knapsack_problems_3.csv"};
    vector<string> output_files = {"knapsack_solutions_1.csv", "knapsack_solutions_2.csv",
                                  "knapsack_solutions_3.csv"};


    for (int file_idx = 0; file_idx < num_files; ++file_idx) {
        cout << "\nProcessing " << input_files[file_idx] << "..." << endl;


        vector<vector<long long>> problems = load_problems(input_files[file_idx]);


        vector<Result> results(problems.size());


        queue<int> problem_indices;
        for (size_t i = 0; i < problems.size(); ++i) {
            problem_indices.push(i);
        }


        unsigned int num_threads = thread::hardware_concurrency();
        if (num_threads == 0) num_threads = 4;
        cout << "Using " << num_threads << " threads for this file." << endl;


        vector<thread> threads;
        for (unsigned int i = 0; i < num_threads; ++i) {
            threads.emplace_back(worker, ref(problem_indices), cref(problems), ref(results));
        }


        for (auto& t : threads) {
            t.join();
        }


        save_results(results, output_files[file_idx]);


        cout << "\nResults for " << input_files[file_idx] << ":" << endl;
        cout << string(80, '-') << endl;
        cout << left << setw(10) << "Problem" << setw(20) << "First Time (s)" << setw(20) << "All Time (s)" << setw(10) << "Solutions" << endl;
        cout << string(80, '-') << endl;
        for (const auto& result : results) {
            cout << left << setw(10) << result.problem_number
                 << setw(20) << (result.first_solution_time > 0 ? to_string(result.first_solution_time).substr(0, 8) : "N/A")
                 << setw(20) << to_string(result.all_solutions_time).substr(0, 8)
                 << setw(10) << result.solutions_count << endl;
        }
        cout << string(80, '-') << endl;
    }

    cout << "\nAll files processed successfully." << endl;
    return 0;
}
