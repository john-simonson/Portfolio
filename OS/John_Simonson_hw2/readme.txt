John Simonson
Email: john_simonson@student.uml.edu
Assignment 2
Degree of Success: 100%

sort_ck:
I completed the assignment as descibed and the output matches what
is shown in the help file. I used the segments of code that Prof. Moloney
diplayed in class. Program is error free and produces the correct output.

grep_ck:(with "cs308a2_grep_data_1")
I completed the assignment as described and output matches what is shown
in the help file. I adapted the sort_ck program to work with grep.
Program is error free and produces the correct output.

grep_ck:(with "cs308a2_grep_data_2")
Program gets stuck in a deadlock since grep fills the pipe causing it to
block until something is removed from the pipe. This is due to the second 
data set being larger. This is the expected outcome.

To run:
make
sort_ck: ./sort_ck
grep_ck: ./grep_ck "file name"