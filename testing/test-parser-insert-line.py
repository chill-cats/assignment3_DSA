import pathlib
from random import choice
import re
import argparse
import subprocess
from utils import generator 
from utils import probing

def main():
    parser = argparse.ArgumentParser(
        description="Run test program with random insert line passed in stdin")
    parser.add_argument('path', type=pathlib.Path,
                        help="Path to executable file")
    parser.add_argument('N', type=int, help="Number of iteration")
    args = parser.parse_args()
    regex = re.compile(r"^(INSERT) ([a-z]\w*)(?: (\d+))?$")


if __name__ == '__main__':
    main()
