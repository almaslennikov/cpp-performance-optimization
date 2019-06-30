import csv
import random

names = ['Alexander', 'Alan', 'John', 'Jake', 'Mark', 'Mary', 'Anna', 'Alice', 'Bob', 'Katy']
positions = ['Engineer', 'HR', 'Intern', 'Manager', 'Security', 'Administrator', 'Janitor']

with open('employees.csv', 'w', newline='') as csvfile:
    writer = csv.writer(csvfile, delimiter=',')
    print([random.choice(names), random.choice(positions), str(random.randint(20, 70)), str(random.randint(5, 350) * 1000)])
    for i in range(5000000):
        writer.writerow([random.choice(names), random.choice(positions), str(random.randint(20, 70)), str(random.randint(5, 350) * 1000)])