import os
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('--test', type=str, default=None)
args = parser.parse_args()

test = args.test
assert(test is not None)


# make = 'make'
# os.system(make)
run = './readchain tests/t{}'.format(test)
os.system(run)

