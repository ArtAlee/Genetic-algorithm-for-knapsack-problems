import pandas as pd
from tabulate import tabulate


bruteforce_data = pd.read_csv("knapsack_solutions.csv")
genetic_data = pd.read_csv("genetic_knapsack_solutions.csv")


def calculate_statistics(data, column):
    mean = data[column].mean()
    variance = data[column].var()
    std_dev = data[column].std()
    return mean, variance, std_dev


bf_first_solution_mean, bf_first_solution_var, bf_first_solution_std = calculate_statistics(bruteforce_data, "First Solution Time (s)")
bf_all_solutions_mean, bf_all_solutions_var, bf_all_solutions_std = calculate_statistics(bruteforce_data, "All Solutions Time (s)")


ga_time_mean, ga_time_var, ga_time_std = calculate_statistics(genetic_data, "Time Taken (s)")


exact_solutions = genetic_data[genetic_data["Best Fitness"] == 0]
exact_solution_ratio = len(exact_solutions) / len(genetic_data)


table = [
    ["Среднее значение", bf_first_solution_mean, ga_time_mean, exact_solution_ratio],
    ["Дисперсия", bf_first_solution_var, ga_time_var, None],
    ["Среднее квадратичное откл.", bf_first_solution_std, ga_time_std, None]
]


headers = ["", "Время нахождения одного решения полным перебором", "Время нахождения точного решения генетическим алгоритмом", "Доля задач, точно решённых генетическим алгоритмом"]
print(tabulate(table, headers=headers, tablefmt="grid"))


print("\nВремя нахождения всех решений полным перебором:")
print(f"Среднее значение: {bf_all_solutions_mean}")
print(f"Дисперсия: {bf_all_solutions_var}")
print(f"Среднее квадратичное отклонение: {bf_all_solutions_std}")


chromosomes_per_generation = 100
print(f"\nКоличество хромосом в поколении: {chromosomes_per_generation}")