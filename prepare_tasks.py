import random
import pyperclip
import csv

A_MAX = int(2**(24/1.4))


def generate_knapsack_vectors(count=50, length=24, min_weight=1, max_weight=A_MAX):
    for _ in range(count):
        yield [random.randint(min_weight, max_weight) for _ in range(length)]

def generate_knapsack_problems(vectors, min_ratio=0.1, max_ratio=0.5):
    problems = []
    for vector in vectors:
        for _ in range(random.randint(10, 20)):
            selected_items = random.sample(vector, k=int(len(vector) * random.uniform(min_ratio, max_ratio)))
            target_weight = sum(selected_items) % A_MAX
            problems.append([*vector, target_weight])
    return problems

def save_to_csv(filename, data):
    with open(filename, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerows(data)

vectors = list(generate_knapsack_vectors())

save_to_csv("knapsack_vectors.csv", vectors)

problems = generate_knapsack_problems(vectors)

save_to_csv("knapsack_problems_8.csv", problems)

csv_data = "\n".join(",".join(map(str, problem)) for problem in problems)
pyperclip.copy(csv_data)

print("Таблицы рюкзачных векторов и задач о рюкзаке сохранены в CSV и скопированы в буфер обмена.")
