# LowLevel-System-Functions2
Further exploring the uses of lowlevel technical aspects relative to actual real world functionality.
# LowLevel-System-Functions

There are a total of 4 programs, they are all meant to test the different levels low-level functionality can be showcased and used in real world cases.

# Running Each
*1.Strace System Call:* 
Compile: 
cd into Folder called "1"
gcc -o StraceSystemCall StraceSystemCall.c
./StraceSystemCall
strace -f -o temp_file.txt ./StraceSystemCall

 *2. Buffer/Unbuffered:*

 Performance Analysis:

Unbuffered I/O: High system call count (1,000,000), slower execution.

Buffered I/O: Low system call count (few thousand), faster execution.

Compile:
cd into "2" folder
gcc -o buff-IO buff-IO.c
strace -c ./buff-IO
time ./buff-IO
(can inverse with unbuff-IO)

*3. Palindromic Num:*
Compile:
cd into "3" folder
gcc -o PalinNum PalinNum.c -lpthread
./PalinNum


*4. MultiThread File:*
Compile
cd into "4" folder
# Compile
gcc -Wall -Wextra -pthread -o MultiFile MultiFile.c

./MultiFile merged_output.txt 2

ORRR

./MultiFile merged_output.txt 1

(for 4 you can pick 1 or two as options to represent threads usage.)


