import Question1
import Question2
import Question3
import Question4
import Question5

print("######Computational Finance - Project 1###########")
print("Nitish Ramkumar")

# Question1
n = 10000;
seed = 2000;
Question1.question1(n, seed)

# Question2
probs = [0.3, 0.35, 0.2, 0.15]
probvals = [-1, 0, 1, 2]
Question2.question2(probs, probvals, n, seed)

# Question3
n = 44
p = 0.64
tot = 1000
Question3.question3(n, p, tot, seed)

# Question 4
lambd = 1.5
n = 10000
Question4.question4(n, lambd, seed)

# Question 5
Question5.question5(seed)

