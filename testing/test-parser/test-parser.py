from os import walk
from random import randint, choice
import pathlib
import re
import argparse
import string
import subprocess


def generateLinear():
    line = "LINEAR "
    param = randint(0, 999999)
    line += str(param)
    line += " "
    param = randint(0, 999999)
    line += str(param)
    return line


def generateDouble():
    line = "DOUBLE "
    param = randint(0, 999999)
    line += str(param)
    line += ' '
    param = randint(0, 999999)
    line += str(param)
    return line


def generateQuadratic():
    line = "QUADRATIC "
    param = randint(0, 999999)
    line += str(param)
    line += " "
    param = randint(0, 999999)
    line += str(param)
    line += " "
    param = randint(0, 999999)
    line += str(param)
    return line


def mutateLine(line):
    lineToMutate = list(line)
    alphabets = string.digits + string.ascii_letters
    length = len(line)
    mutationCount = randint(0, int(length / 2))
    for _ in range(mutationCount):
        mutationPos = randint(0, length - 1)
        randomLetter = choice(alphabets)
        lineToMutate[mutationPos] = randomLetter

    addSpaceBegin = randint(0, 1)
    if addSpaceBegin == 1:
        lineToMutate = " " + line

    addSpaceEnd = randint(0, 1)
    if addSpaceEnd == 1:
        lineToMutate += " "

    return "".join(lineToMutate)


def generateSetupLine():
    option = randint(0, 2)
    line = ""
    if option == 0:
        line = generateLinear()
    elif option == 1:
        line = generateDouble()
    elif option == 2:
        line = generateQuadratic()

    shouldMutate = randint(0, 1)
    if shouldMutate == 1:
        line = mutateLine(line)

    return line


def main():
    parser = argparse.ArgumentParser(
        description="Run test program with random setup line passed in stdin")
    parser.add_argument('path', type=pathlib.Path,
                        help="Path to executable file")
    parser.add_argument('N', type=int, help="Number of iteration")
    args = parser.parse_args()

    regex = re.compile(r"^(?:(LINEAR) (\d{1,6}) (\d{1,6})|(QUADRATIC) (\d{1,6}) (\d{1,6}) (\d{1,6})|(DOUBLE) (\d{1,6}) (\d{1,6}))$")
    for it in range(args.N):
        line = generateSetupLine()
        valid = False
        regexMatchResult = re.search(regex, line)

        expectedOutput = ""
        if regexMatchResult:
            valid = True
            if regexMatchResult.group(1) == "LINEAR":
                expectedOutput += "LINEAR"
                expectedOutput += f"\n{regexMatchResult.group(2)}"
                expectedOutput += f"\n{regexMatchResult.group(3)}"
            elif regexMatchResult.group(4) == "QUADRATIC":
                expectedOutput += "QUADRATIC"
                expectedOutput += f"\n{regexMatchResult.group(5)}"
                expectedOutput += f"\n{regexMatchResult.group(6)}"
                expectedOutput += f"\n{regexMatchResult.group(7)}"
            elif regexMatchResult.group(8) == "DOUBLE":
                expectedOutput += "DOUBLE"
                expectedOutput += f"\n{regexMatchResult.group(9)}"
                expectedOutput += f"\n{regexMatchResult.group(10)}"

        runResult = subprocess.run(args.path, input = bytearray(line, encoding='utf-8'), stdout = subprocess.PIPE)
        programOutValid = runResult.returncode == 0

        print(f"Running test #{it} with input: \"{line}\", expected: {valid}, got {programOutValid}, ", end='')
        if valid != programOutValid:
            print(f"Not OK!")
            exit(1)
        else:
            print(f"OK!")
            if valid == True:
                print(f"Checking output:\n{expectedOutput}")
                print(f"Program output:\n{runResult.stdout.decode()}")
                if expectedOutput != runResult.stdout.decode():
                    print("Output not correct!")
                    exit(1)


if __name__ == '__main__':
    main()
