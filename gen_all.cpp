#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <algorithm>
#include <iomanip>

const double BRUTE_FORCE_TIME = 5.0;

struct Result {
    int problemNumber;
    double timeTaken;
    int bestFitness;
    bool stoppedByCondition;
    int lastGeneration;
};

std::vector<std::vector<int>> load_problems(const std::string& filename) {
    std::vector<std::vector<int>> problems;
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        std::vector<int> problem;
        size_t pos = 0;
        std::string token;
        while ((pos = line.find(',')) != std::string::npos) {
            token = line.substr(0, pos);
            problem.push_back(std::stoi(token));
            line.erase(0, pos + 1);
        }
        problem.push_back(std::stoi(line));
        problems.push_back(problem);
    }
    file.close();
    return problems;
}

int fitness(const std::vector<int>& individual, const std::vector<int>& weights, int target_weight) {
    int total_weight = 0;
    for (size_t i = 0; i < individual.size(); i++) {
        if (individual[i] == 1) total_weight += weights[i];
    }
    return std::abs(target_weight - total_weight);
}

std::vector<int> create_individual(int n) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);

    std::vector<int> individual(n);
    for (int i = 0; i < n; i++) {
        individual[i] = dis(gen);
    }
    return individual;
}

std::vector<std::vector<int>> create_population(int pop_size, int n) {
    std::vector<std::vector<int>> population(pop_size);
    for (int i = 0; i < pop_size; i++) {
        population[i] = create_individual(n);
    }
    return population;
}

std::vector<std::vector<int>> tournament_selection(const std::vector<std::vector<int>>& population,
                                                 const std::vector<int>& fitnesses,
                                                 int tournament_size = 3) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::vector<std::vector<int>> selected;

    for (size_t i = 0; i < population.size(); i++) {
        std::vector<int> candidates(tournament_size);
        for (int j = 0; j < tournament_size; j++) {
            candidates[j] = std::uniform_int_distribution<>(0, population.size() - 1)(gen);
        }
        int winner = *std::min_element(candidates.begin(), candidates.end(),
                                     [&](int a, int b) { return fitnesses[a] < fitnesses[b]; });
        selected.push_back(population[winner]);
    }
    return selected;
}

std::pair<std::vector<int>, std::vector<int>> crossover(const std::vector<int>& parent1,
                                                       const std::vector<int>& parent2) {
    std::random_device rd;
    std::mt19937 gen(rd());
    int point = std::uniform_int_distribution<>(1, parent1.size() - 1)(gen);

    std::vector<int> child1(parent1.begin(), parent1.begin() + point);
    child1.insert(child1.end(), parent2.begin() + point, parent2.end());

    std::vector<int> child2(parent2.begin(), parent2.begin() + point);
    child2.insert(child2.end(), parent1.begin() + point, parent1.end());

    return {child1, child2};
}

std::vector<int> mutate(std::vector<int> individual, double mutation_rate = 0.01) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);

    for (size_t i = 0; i < individual.size(); i++) {
        if (dis(gen) < mutation_rate) {
            individual[i] = 1 - individual[i];
        }
    }
    return individual;
}

Result genetic_algorithm(const std::vector<int>& problem, int target_weight,
                        int pop_size = 10000, int max_generations = 1000,
                        double mutation_rate = 0.03) {
    std::vector<int> weights(problem.begin(), problem.end() - 1);
    int n = weights.size();
    auto population = create_population(pop_size, n);
    int best_fitness = INT_MAX;
    int no_improvement_count = 0;

    auto start_time = std::chrono::high_resolution_clock::now();
    auto last_improvement_time = start_time;

    int generation = 0;
    for (; generation < max_generations; generation++) {
        std::vector<int> fitnesses(pop_size);
        for (int i = 0; i < pop_size; i++) {
            fitnesses[i] = fitness(population[i], weights, target_weight);
        }

        int current_best = *std::min_element(fitnesses.begin(), fitnesses.end());
        if (current_best < best_fitness) {
            best_fitness = current_best;
            no_improvement_count = 0;
            last_improvement_time = std::chrono::high_resolution_clock::now();
        } else {
            no_improvement_count++;
        }

        if (best_fitness == 0) break;
        if (no_improvement_count >= 2) break;

        auto current_time = std::chrono::high_resolution_clock::now();
        double time_elapsed = std::chrono::duration<double>(current_time - start_time).count();
        if (time_elapsed > 2 * BRUTE_FORCE_TIME) break;

        auto selected = tournament_selection(population, fitnesses);
        std::vector<std::vector<int>> next_population;
        for (size_t i = 0; i < selected.size() - 1; i += 2) {
            auto [child1, child2] = crossover(selected[i], selected[i + 1]);
            next_population.push_back(mutate(child1, mutation_rate));
            next_population.push_back(mutate(child2, mutation_rate));
        }
        population = next_population;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    double time_taken = std::chrono::duration<double>(end_time - start_time).count();

    bool stopped_by_condition = (no_improvement_count >= 2) ||
                               (std::chrono::duration<double>(end_time - start_time).count() > 2 * BRUTE_FORCE_TIME);

    return {0, time_taken, best_fitness, stopped_by_condition, generation};
}

void process_file(int file_num) {
    std::string input_file = "knapsack_problems_" + std::to_string(file_num) + ".csv";
    std::string output_file = "genetic_knapsack_solutions_" + std::to_string(file_num) + ".csv";

    auto problems = load_problems(input_file);
    std::vector<Result> results;
    int perfect_solved = 0;
    double sum_fitness = 0;

    for (size_t i = 0; i < problems.size(); i++) {
        int target_weight = problems[i].back();
        Result result = genetic_algorithm(problems[i], target_weight);
        result.problemNumber = i + 1;

        results.push_back(result);
        if (result.bestFitness == 0) perfect_solved++;
        sum_fitness += result.bestFitness;

        std::cout << "Problem " << i + 1 << "/" << problems.size()
                  << " solved: Best Fitness = " << result.bestFitness
                  << ", Time = " << std::fixed << std::setprecision(2) << result.timeTaken << "s\n";
    }

    std::ofstream out(output_file);
    out << "Problem Number,Time Taken (s),Best Fitness,Stopped By Condition,Last Generation\n";
    for (const auto& r : results) {
        out << r.problemNumber << "," << r.timeTaken << "," << r.bestFitness << ","
            << (r.stoppedByCondition ? "true" : "false") << "," << r.lastGeneration << "\n";
    }
    out.close();

    double percentage_solved = (static_cast<double>(perfect_solved) / problems.size()) * 100;

    std::cout << "\nResults for " << input_file << ":\n";
    std::cout << std::string(80, '-') << "\n";
    std::cout << std::left << std::setw(10) << "Problem" << std::setw(15) << "Time (s)"
              << std::setw(15) << "Best Fitness" << std::setw(20) << "Stopped By Condition"
              << std::setw(15) << "Last Generation" << "\n";
    std::cout << std::string(80, '-') << "\n";

    for (const auto& r : results) {
        std::cout << std::left << std::setw(10) << r.problemNumber
                  << std::setw(15) << std::fixed << std::setprecision(2) << r.timeTaken
                  << std::setw(15) << r.bestFitness
                  << std::setw(20) << (r.stoppedByCondition ? "true" : "false")
                  << std::setw(15) << r.lastGeneration << "\n";
        std::cout << std::string(80, '-') << "\n";
    }

    std::cout << "\nTotal problems: " << problems.size() << "\n";
    std::cout << "Exactly solved problems: " << perfect_solved << "\n";
    std::cout << "Percentage solved: " << percentage_solved << "%\n";
    std::cout << "Average best fitness: " << sum_fitness / problems.size() << "\n";
}

int main() {
    for (int i = 1; i <= 4; i++) {
        process_file(i);
        std::cout << "\n\n";
    }
    return 0;
}