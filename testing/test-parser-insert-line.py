import pathlib
from random import choice
import re
import argparse
import subprocess
import curses
from utils import generator 
from time import sleep
from utils import probing

def main(_):
    parser = argparse.ArgumentParser(
        description="Run test program with random insert line passed in stdin")
    parser.add_argument('path', type=pathlib.Path,
                        help="Path to executable file")
    parser.add_argument('N', type=int, help="Number of iteration")
    args = parser.parse_args()
    regex = re.compile(r"^(INSERT) ([a-z]\w*)(?: (\d+))?$")

    curses.noecho()
    curses.start_color()
    curses.use_default_colors()

    testcaseOutputWin = curses.newwin(1, curses.COLS - 1, 0, 0)
    expectedOutputWin = curses.newwin(curses.LINES - 6, int((curses.COLS - 1) / 2), 1, 0)
    programOutputWin = curses.newwin(curses.LINES - 6, int((curses.COLS - 1) / 2), 1, int((curses.COLS - 1) / 2))

    for it in range(args.N):
        line = generator.generate_insert()
        shouldMutate = choice([0, 1])
        if shouldMutate == 1:
            line = probing.mutateLine(line)

        valid = False

        match = regex.search(line)
        expectedOutput = ""
        if match:
            valid = True
            expectedOutput += match.group(1)
            expectedOutput += '\n';
            expectedOutput += match.group(2)
            if match.group(3):
                expectedOutput += '\n'
                expectedOutput += str(int(match.group(3)))

        testcaseOutputWin.erase()
        testcaseOutputWin.addstr(f"Running testcase {it}: ")
        testcaseOutputWin.addstr(f"\"{line}\"")
        testcaseOutputWin.addstr(f". Expected: {valid}, output: ")

        runResult = subprocess.run(args.path, input = bytearray(line, encoding='utf-8'), stdout = subprocess.PIPE)
        programOutValid = runResult.returncode == 0
        testcaseOutputWin.addstr(f"{programOutValid}, ")
        
        expectedOutputWin.erase()
        expectedOutputWin.addstr("Expected output: ")
        if match:
            expectedOutputWin.addstr(expectedOutput)
        else:
            expectedOutputWin.addstr("N/A")

        expectedOutputWin.refresh()

        programOutputWin.erase()
        programOutputWin.addstr("Program output: ")
        if programOutValid:
            programOutputWin.addstr(runResult.stdout.decode())
        else:
            programOutputWin.addstr("N/A")
        programOutputWin.refresh()
        if programOutValid != valid or expectedOutput != runResult.stdout.decode():
            testcaseOutputWin.addstr("Not OK!")
            testcaseOutputWin.getkey()
            exit(1)
        else:
            testcaseOutputWin.addstr("OK!")
        testcaseOutputWin.refresh()

if __name__ == '__main__':
    curses.wrapper(main)   
