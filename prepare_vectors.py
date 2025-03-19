import random
import pyperclip

def generate_knapsack_vectors(count=50, length=24, min_weight=1, max_weight=1073741824):
    for _ in range(count):
        yield [random.randint(min_weight, max_weight) for _ in range(length)]

def vectors_to_table(vectors):
    return "\n".join("\t".join(map(str, vector)) for vector in vectors)

# Генерация 50 рюкзачных векторов
vectors = list(generate_knapsack_vectors())

# Преобразование в таблицу
table = vectors_to_table(vectors)

# Копирование в буфер обмена
pyperclip.copy(table)

# Сохранение в файл
with open("knapsack_vectors.txt", "w") as f:
    f.write(table)

print("Таблица рюкзачных векторов скопирована в буфер обмена и сохранена в файл 'knapsack_vectors.txt'.")
