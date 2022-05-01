%Used http://rosettacode.org/wiki/Combinations#Prolog for reference
comb_Prolog(L, M, N) :- %make combinations function
    length(L, M), %list of inputs
    fill(L, 1, N). %generate combinations
 
fill([], _, _).
 
fill([H | T], Min, Max) :- 
    between(Min, Max, H),
    H1 is H + 1, %add 1 to H
    fill(T, H1, Max). %recurse
