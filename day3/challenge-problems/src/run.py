import argparse
import os

parser = argparse.ArgumentParser(description='Process some integers.')
parser.add_argument('--test', type=int, default=1)
args = parser.parse_args()

run = './readindexed tests/t{}.idx'.format(args.test)
os.system(run)

test = 'cat tests/t{}.txt'.format(args.test)
os.system(test)
