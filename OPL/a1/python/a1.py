# Used http://rosettacode.org/wiki/Combinations#Python for reference
from itertools import combinations #import combination tool

k = int(input("Enter k: ")) #prompt and get user input
tmp = int(input("Enter n: ")) #prompt and get user input
tmp = tmp + 1 
n = range(1,tmp) #convert temp to range and assign it to n

output = list(combinations(n,k)) # generate combinations list
print(output) #print combinations
