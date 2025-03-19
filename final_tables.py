import pandas as pd



def calculate_statistics(data, column):
    mean = data[column].mean()
    variance = data[column].var()
    std_dev = data[column].std()
    return mean, variance, std_dev



for i in range(1, 9):
    # Load data
    bruteforce_data = pd.read_csv(f"knapsack_solutions_{i}.csv")
    genetic_data = pd.read_csv(f"genetic_knapsack_solutions_{i}.csv")


    bf_first_mean, bf_first_var, bf_first_std = calculate_statistics(bruteforce_data, "First Solution Time (s)")
    bf_all_mean, bf_all_var, bf_all_std = calculate_statistics(bruteforce_data, "All Solutions Time (s)")


    ga_mean, ga_var, ga_std = calculate_statistics(genetic_data, "Time Taken (s)")


    exact_solutions = genetic_data[genetic_data["Best Fitness"] == 0]
    exact_ratio = len(exact_solutions) / len(genetic_data)


    print(f"\nResults for knapsack_solutions_{i}.csv")
    print("Metric\tBruteforce (First Solution)\tGenetic Algorithm\tExact Solution Ratio")
    print("-" * 80)


    print(f"Mean\t{bf_first_mean:.6f}\t{ga_mean:.6f}\t{exact_ratio:.6f}")
    print(f"Variance\t{bf_first_var:.6f}\t{ga_var:.6f}\t-")
    print(f"Std Dev\t{bf_first_std:.6f}\t{ga_std:.6f}\t-")


    print("\nBruteforce (All Solutions):")
    print(f"Mean: {bf_all_mean:.6f}")
    print(f"Variance: {bf_all_var:.6f}")
    print(f"Std Dev: {bf_all_std:.6f}")


    chromosomes = 100
    print(f"\nChromosomes per generation: {chromosomes}")
