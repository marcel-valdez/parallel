@echo off
cl mult_matrices.c /Od /DAH#%1 /DAW#%2 /DBW#%3
start /B mult_matrices.exe