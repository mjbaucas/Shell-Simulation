all: m_shell
m_shell: m_shell.o 
	gcc -o m_shell m_shell.o -lfl  
m_shell.o: m_shell.c
	gcc -Wall -g -c m_shell.c
clean:
	rm -f *.o
	rm -f m_shell
	