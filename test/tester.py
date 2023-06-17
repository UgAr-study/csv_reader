import os
import pathlib
import subprocess

exe = '../build/csv_manager'
test_dir = os.path.abspath(os.getcwd())
test_files = [f for f in os.listdir(test_dir) if f.endswith('.csv')]
test_count = 0
for test in test_files:
    print('\n       Test №'+str(test_count)+'\n------ '+test+' ------\nInput:\n')

    with open(os.path.join(test_dir, test)) as file:
        print(file.read())
    test_count += 1
    print('\nOutput:\n')
    subprocess.run([exe, test])