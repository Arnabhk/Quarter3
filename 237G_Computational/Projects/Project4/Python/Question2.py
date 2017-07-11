import numpy as np
import csv

def question2():
    #data = csv.reader(open("Google_data.csv","rb"),delimiter=",")
    #data = np.loadtxt(open("Google_data.csv", "rb"), delimiter=",")
    data = np.loadtxt('Google_data.csv', dtype='datetime,float', delimiter=',', usecols=(0, 1), unpack=True, skiprows=1)
    print(data)


