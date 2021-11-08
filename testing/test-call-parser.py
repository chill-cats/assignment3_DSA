from random import randint, choice, randrange
from string import ascii_lowercase, ascii_letters, digits, whitespace
import subprocess
import argparse
from pathlib import Path
import re

nameAlphabets = ascii_letters + digits + '_'  # valid alphabets for name
stringAlphabets = ascii_letters + ' ' + digits

def mutateLine(line):
    lineToMutate = list(line)
    alphabets = digits + ascii_letters
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

def main():
    argsParser = argparse.ArgumentParser(
    description="Run test program with random CALL instruction line passed in stdin")
    argsParser.add_argument('path', type=Path, help="Path to executable file")
    argsParser.add_argument('N', type=int, help="Number of iteration")
    args = argsParser.parse_args()
    
    regex = re.compile(r"^CALL [a-z]\w*\((?:|(?:\d+|'[\dA-Za-z\s]*'|[a-z]\w*)(?:,(?:\d+|'[\dA-Za-z\s]*'|[a-z]\w*))*)\)$")

    for it in range(args.N):
        name = generateIdentifierName()
        paramList = generateParamList()
        inputLine = 'CALL ' + name + paramList[0]
        shouldMutate = choice([0, 1])

        if shouldMutate == 1:
            inputLine = mutateLine(inputLine)
        expectedOutput = 'CALL\n' + f'{name}'
        for param in paramList[1]:
            expectedOutput += f'\n{param}'
        print(f"Running test #{it}")

        isValid = regex.match(inputLine) != None
        runResult = subprocess.run(args.path, input = bytearray(inputLine, encoding='utf-8'), stdout = subprocess.PIPE)
        runValid = runResult.returncode == 0
        if isValid != runValid:
            print(f'Input:\n{inputLine}')
            print(f'Expected: {isValid}!')
            print(f'GOT: {runValid}!')      
            exit(1)
        if isValid and not shouldMutate and runResult.stdout.decode() != expectedOutput:
            print(f'Ouptut not match!')
            print(f'Input:\n{inputLine}')
            print(f'Expected:\n{expectedOutput}')
            print(f'Got:\n{runResult.stdout.decode()}')
            exit(1)


def generateLiteralNumber() -> str:
    numberLen = randint(1, 100)
    num = [choice(['1', '2', '3', '4', '5', '6', '7', '8', '9'])]
    for _ in range(numberLen - 1):
        num += choice(digits)
    return "".join(num)


def generateLiteralString() -> str:
    strLen: int = randint(1, 100)
    string = ['\'']
    for _ in range(strLen):
        string += choice(stringAlphabets)
    string += '\''
    return "".join(string)


def generateIdentifierName() -> str:
    nameLen = randint(1, 100)
    name = [choice(ascii_lowercase)]
    for _ in range(nameLen):
        name.append(choice(nameAlphabets))
    return "".join(name)


def generateParamList() -> tuple[str, list[str]]:
    paramCount = randint(0, 512)
    output = [f'(']
    outputList: list[str] = []
    for _ in range(paramCount):
        t = choice([1, 2, 3])
        if t == 1:
            name = generateIdentifierName()
            output += name
            outputList.append(name)
        elif t == 2:
            string = generateLiteralString()
            output += string
            outputList.append(string)
        else:
            number = generateLiteralNumber()
            output += number
            outputList.append(number)
        output += ','
    if output[len(output) - 1] == ',':
        output.pop()
    output += ')'
    return ("".join(output), outputList)


if __name__ == '__main__':
    main()
