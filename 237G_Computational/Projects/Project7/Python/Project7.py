import Question1
import Question2
import pandas as pd
import numpy as np

print("######Computational Finance - Project 7###########")
print("Nitish Ramkumar")

# Question1
#Use S0 = 10
s0 = 10
answer = Question1.question1(s0)
print(answer.loc[s0,:])
print(answer.to_string())
answer_mat = answer.as_matrix()
error_abs = np.array([(answer_mat[:,j]-answer_mat[:,len(answer_mat[0])-1])/answer_mat[:,len(answer_mat[0])-1]
                     for j in range(0,len(answer_mat[0])-1)])
cols = list(answer.columns)
cols.pop()
print(pd.DataFrame(error_abs.transpose(),columns=cols, index=range(4,16+1)))

# Question2
s0=10
answer = Question2.question2(s0)
print(pd.DataFrame(answer[6,0:6],index=["Explicit_Call","Explicit_Put","Implicit_Call","Implicit_Put",
                                         "Crank-Nicolson_Call","Crank-Nicolson_Put"]))

print(pd.DataFrame(answer[6,6:12],index=["Explicit_Call","Explicit_Put","Implicit_Call","Implicit_Put",
                                         "Crank-Nicolson_Call","Crank-Nicolson_Put"]))
Question2.plotAnswer(answer)
print("As can be seen, the graphs between the call and put are very similar for every method")
