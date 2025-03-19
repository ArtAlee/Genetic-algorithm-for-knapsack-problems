import pandas as pd
import matplotlib.pyplot as plt
import numpy as np


def calculate_statistics(data, column):
    mean = data[column].mean()
    variance = data[column].var()
    std_dev = data[column].std()
    return mean, variance, std_dev


a_max_values = []
bruteforce_first_means = []
bruteforce_all_means = []
genetic_means = []
exact_ratios = []


a_max_dict = {
    1: int(2 ** (24 / 0.8)),  # 1,073,741,824
    2: int(2 ** (24 / 1.0)),  # 16,777,216
    3: int(2 ** (24 / 1.2)),  # 1,048,576
    4: int(2 ** (24 / 1.4)),  # 131,072
    5: int(2 ** (24 / 0.8)),  # 1,073,741,824
    6: int(2 ** (24 / 1.0)),  # 16,777,216
    7: int(2 ** (24 / 1.2)),  # 1,048,576
    8: int(2 ** (24 / 1.4))   # 131,072
}


for i in range(5, 9):

    bruteforce_data = pd.read_csv(f"knapsack_solutions_{i}.csv")
    genetic_data = pd.read_csv(f"genetic_knapsack_solutions_{i}.csv")


    bf_first_mean, bf_first_var, bf_first_std = calculate_statistics(bruteforce_data, "First Solution Time (s)")
    bf_all_mean, bf_all_var, bf_all_std = calculate_statistics(bruteforce_data, "All Solutions Time (s)")


    ga_mean, ga_var, ga_std = calculate_statistics(genetic_data, "Time Taken (s)")


    exact_solutions = genetic_data[genetic_data["Best Fitness"] == 0]
    exact_ratio = len(exact_solutions) / len(genetic_data)


    a_max_values.append(a_max_dict[i])
    bruteforce_first_means.append(bf_first_mean)
    bruteforce_all_means.append(bf_all_mean)
    genetic_means.append(ga_mean)
    exact_ratios.append(exact_ratio)


    print(f"\nResults for knapsack_solutions_{i}.csv (a_max = {a_max_dict[i]})")
    print("Metric\tBruteforce (First Solution)\tGenetic Algorithm\tExact Solution Ratio")
    print("-" * 80)
    print(f"Mean\t{bf_first_mean:.6f}\t{ga_mean:.6f}\t{exact_ratio:.6f}")
    print(f"Variance\t{bf_first_var:.6f}\t{ga_var:.6f}\t-")
    print(f"Std Dev\t{bf_first_std:.6f}\t{ga_std:.6f}\t-")
    print("\nBruteforce (All Solutions):")
    print(f"Mean: {bf_all_mean:.6f}")
    print(f"Variance: {bf_all_var:.6f}")
    print(f"Std Dev: {bf_all_std:.6f}")
    print(f"\nChromosomes per generation: 100")


plt.figure(figsize=(15, 10))


# plt.subplot(2, 2, 1)
# plt.plot(a_max_values, bruteforce_first_means, marker='o', label='Bruteforce (First Solution)')
# plt.xlabel('a(max)')
# plt.ylabel('Среднее время (сек)')
# plt.title('Полный перебор - время одного решения от a(max) (mod)')
# plt.grid(True)
# plt.xscale('log')  # Logarithmic x-axis for a_max
# plt.yscale('log')  # Logarithmic y-axis for time
# plt.xticks(a_max_values, [f'{x:.0e}' for x in a_max_values])  # Custom x-ticks with scientific notation


# plt.subplot(2, 2, 2)
# plt.plot(a_max_values, bruteforce_all_means, marker='o', label='Bruteforce (All Solutions)')
# plt.xlabel('a(max)')
# plt.ylabel('Среднее время (сек)')
# plt.title('Полный перебор - среднее время для всех решений от a(max) (mod)')
# plt.grid(True)
# plt.xscale('log')
# plt.yscale('log')
# plt.xticks(a_max_values, [f'{x:.0e}' for x in a_max_values])
#

# plt.subplot(2, 2, 3)
# plt.plot(a_max_values, genetic_means, marker='o', label='Genetic Algorithm')
# plt.xlabel('a(max)')
# plt.ylabel('Среднее время (сек)')
# plt.title('Генетический алгортим - среднее время от a(max) (mod)')
# plt.grid(True)
# plt.xscale('log')
# plt.yscale('log')
# plt.xticks(a_max_values, [f'{x:.0e}' for x in a_max_values])
#

# plt.subplot(2, 2, 4)
plt.plot(a_max_values, exact_ratios, marker='o', label='Exact Solution Ratio')
plt.xlabel('a(max)')
plt.ylabel('Процент точно решенных задач')
plt.title('Зависимость процента точно решенных задач от a(max) (mod)')
plt.grid(True)
plt.xscale('log')
plt.xticks(a_max_values, [f'{x:.0e}' for x in a_max_values])


plt.tight_layout()
plt.show()