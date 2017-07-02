#!/usr/bin/python3

import sys
import subprocess

MIN_LENGTH=1000
MAX_LENGTH=10000
REPETITIONS=10
STEP=100

for n in range(MIN_LENGTH, MAX_LENGTH+1, STEP):
    time_sum=0
    for i in range(0, REPETITIONS):
        time=subprocess.Popen(["./test.sh", str(n)], stderr=subprocess.PIPE).communicate()[1].split()
        time_sum+=float(time[1])
    print("{0} {1}".format(n, time_sum/REPETITIONS).replace('.', ','), file=sys.stderr)
