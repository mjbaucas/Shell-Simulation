/* Program Name */
m_shell

/* Programmer */
Marc Jayson Baucas 0825427

/* Description */
The program m_shell is a linux based program to simulate simple shell functions. This program was written in c under an ubuntu 14.01 Linux environment. m_shell takes in an input form the user similar to how a user uses the terminal on any Linux shell. 

/*Compilation */
    - cd to the main program directory
    - Generate the makefile by typing make in the terminal

/* Execution */
    - Make sure the program is compiled
    - Make sure user is in the main program directory
    - Type in './m_shell'

/* Testing */

    Input: ls
    Output: Makefile	m_shell	   m_shell.c    README.txt

    Input: ls -l  
    Output: -rwxrwxrwx 1 marc marc   156 Jan 29 16:57 Makefile
            -rwxrwxrwx 1 marc marc 22600 Jan 29 16:58 m_shell
            -rwxrwxrwx 1 marc marc 11603 Jan 29 16:57 m_shell.c
            -rwxrwxrwx 1 marc marc     0 Jan 30  2017 README.txt

    Input: gedit &
    Output: open gedit in background

    Input: ls < ls.txt (file contains: "-l")
    Output: -rwxrwxrwx 1 marc marc   156 Jan 29 16:57 Makefile
            -rwxrwxrwx 1 marc marc 22600 Jan 29 16:58 m_shell
            -rwxrwxrwx 1 marc marc 11603 Jan 29 16:57 m_shell.c
            -rwxrwxrwx 1 marc marc     0 Jan 30  2017 README.txt

    Input: args 1 2 3 > output.txt 
    Output: (In output.txt)
            argc = 3, args = 1, 2, 3
	

    Input: add 1 2 3
    Output: 1 + 2 + 3 = 6

    Input: args 1 2 3 4 5 "six, seven"
    Output: argc = 6, args = 1, 2 ,3 ,4, 5, "six, seven"

    Input: ave 1 2 3
    Output: (1 + 2 + 3)/3 = 2.00
