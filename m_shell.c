#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>

/*
 *  Programmer: Marc Jayson Baucas - 0825427
 *  Program: CENG
 *
 *	References: 	
 *	  Tutorial - Write a Shell in C 
 *      Link: https://brennan.io/2015/01/16/write-a-shell-in-c/
 *      By: Stephen Brennan
 *
 *	  Parser Code in Moodle (lex.c, Makefile)
 *	    By: Professor Xining Li (CIS3110)
 * 
 *	Assumptions:
 *		Quotation marks are always used to enclose specific strings, there should be no cases where a string is led by a quotation mark but not closed by another
 *      
 */

/* 
 * Name: convert_to_int()
 * Description: Converts string to digits (considers hexadecimal, decimal)
 * Parameters: string (char*) - String that contains the values to be converted into numerical form
 */  
int convert_to_int(char * string)
{
	// If string is hexadecimal
	if(string[strspn(string, "0123456789abcdefABCDEFx")] == 0)
	{
		return (int) strtol(string, NULL, 0);
	}
	// Normal string to number
	else if (string[strspn(string, "0123456789.")] == 0)
	{
		return atoi(string);	
	}
	else
	{
		fprintf(stderr, "m_shell > CONVERSION ERROR: %s is not a valid input, the value will be replaced with 0\n", string);
		return 0;
	}
}

/*
 *	Name: handler()
 *	Description: Handles any child processes that have not been closed and waits for them
 *  Parameters: N/A
 */
void handler()
{
	int status;
	waitpid(-1, &status, WNOHANG);
}

/* 
 * Name: get_input()
 * Description: Retrieves user input and removes return key and replaces with null terminator
 * Parameters: N/A
 */ 
char * get_input()
{
	char * buffer;
	int buffer_size = 100;
	int i = 0;

	buffer = malloc(sizeof(char) * buffer_size);
	fgets(buffer, 100, stdin);

	/* Catch invalid inputs */
	if (buffer[0] == '\0' || buffer[0] == '\n')
	{
		fprintf(stderr, "m_shell > INPUT ERROR: Input from buffer is empty\n");
		free(buffer);
		buffer = malloc(sizeof(char) * 9);
		sprintf(buffer, "%s", "override");
	}
	else if (buffer[0] == ' ' && (buffer[1] == '\n' || buffer[1] == '\0'))
	{
		fprintf(stderr, "m_shell > INPUT ERROR: Input from buffer is just a space\n");
		free(buffer);
		buffer = malloc(sizeof(char) * 9);
		sprintf(buffer, "%s", "override");
	}

	/* Replace return key with nul terminator */
	for (i = 0; i < strlen(buffer); i++)
	{
		if (buffer[i] == '\n')
		{
			buffer[i] = '\0';
		}
	}	

	return buffer;
}

/* 
 * Name: parse_inputs()
 * Description: Parses inputs and returns arguments in an array
 * Parameters: input (char*) - string that contains the input form to user that will be parsed into arguments
 */
char ** parse_inputs(char * input)
{
	char * token; 
	char ** arguments;
	char * temp_string;
	char * temp_dest;

	int arg_pos = 0;
	int arg_size = 1;
	int in_string = 0;

	arguments = malloc(sizeof(char*) * arg_size);

	token = strtok(input, " ");
	while (token != NULL) 
	{
		// If the token belongs to the same argument (Quotation Mark)
		if (in_string == 1)
        {
        	// Backtrack position to append to same argument
        	arg_pos--;
        	temp_string = token;
        	// Build temporary placeholder for new string
        	temp_dest = malloc(sizeof(char) * (strlen(temp_string) + strlen(arguments[arg_pos]) + 2));
        	sprintf(temp_dest, "%s %s", arguments[arg_pos], temp_string);

        	// Replace argument with new string
        	free(arguments[arg_pos]);
       		arguments[arg_pos] = malloc(sizeof(char) * (strlen(temp_dest) + 1));
       		sprintf(arguments[arg_pos], "%s", temp_dest);

       		// Free temporary placeholder
       		free(temp_dest);
        }              
        else // Normal passthrough
        {
        	arguments[arg_pos] = malloc(sizeof(char) * (strlen(token) + 1));
        	sprintf(arguments[arg_pos], "%s", token);
        }

        // If string starts with quotation mark
		if (arguments[arg_pos][0] == '"')
        {
        	in_string = 1;
        }
        
        // If string ends wih quotation mark
        if (arguments[arg_pos][strlen(arguments[arg_pos]) - 1] == '"')
        {
        	in_string = 0;
        }

        // Resize argument array to accomodate any additional arguments
        arg_pos++;
        if (arg_pos >= arg_size)
        {
        	arg_size++;
			arguments = realloc(arguments, arg_size * sizeof(char*));
			if (!arguments)
			{
				fprintf(stderr, "m_shell > MEMORY ERROR: Reallocation Error (Main Parser). Program will exit\n");
				exit(-1);
			}
		}
       
		token = strtok(NULL, " ");
	}	
	arguments[arg_size-1] = NULL; // Similar to null terminating a string
	return arguments;
}

/* 
 * Name: exe_command()
 * Description: Execute command based on arguments passed
 * Parameters: arguments (char**) - string array that will be parsed as commanfs to be executed
 */
int exe_command(char ** arguments)
{
	int pid = 0;	
	int i = 0;
	int arg_count = 0;
	int temp_num = 0;
	int sum = 0;
	int status = 0;
	int file_flag = 0;
	int background_flag = 0;
	double ave = 0.0;
	FILE * file;
	long file_size = 0;

	int argument_flag = 0;
	char ** used_arguments;
	char ** output_arguments;
	char ** input_arguments;
	char ** background_arguments;
	char * temp_buffer;
	char * input_buffer;

	signal(SIGCHLD, handler);

	/* Get the number of arguments */
	for (i = 0; arguments[i] != NULL; i++)
	{
		arg_count++;
	}
	arg_count--;

	/* Input and Output stream rerouting sections */
	/* Only run if number of arguments is greater than 1 */
	if (arg_count > 0){
		/* Output rerouting to file */
		if (strcmp(arguments[arg_count - 1], ">") == 0)
		{
			/*Rerout STDOUT stream to file */
			file = freopen(arguments[arg_count], "w+", stdout);
			if (file == NULL)
			{
				perror("File was not created");
				return 1;
			}
			
			argument_flag = 1; // Set to use output arguments in argument parser
			arg_count-=2;      // Change argument count to exclude ">" and the file to be used for output

			// Build and populate argument array 
			output_arguments = malloc(sizeof(char*) * (arg_count + 2));
			for (i = 0; i < arg_count + 1; i++)
			{
				output_arguments[i] = malloc(sizeof(char) * (strlen(arguments[i]) + 1));
				sprintf(output_arguments[i], "%s", arguments[i]);
			}

			output_arguments[arg_count + 1] = NULL;
			file_flag = 1; // Set file flag to close after use later on
		}
		else if (strcmp(arguments[arg_count - 1], "<") == 0)
		{
			/* Rerout STDIN stream from file */
			file = freopen(arguments[arg_count], "r", stdin);
			if (file == NULL)
			{
				perror("No Such File");
				return 1;
			}

			/* Get the input file size */
			fseek(file, 0, SEEK_END);
			file_size = ftell(file);
			fseek(file, 0, SEEK_SET);

			/* Allocate buffer with file contents */
			temp_buffer = malloc((file_size + 1) * sizeof(char));
			fread(temp_buffer, file_size, 1, file);
			temp_buffer[file_size] = '\0';

			/* Format buffer contents */
			for (i = 0; i < strlen(temp_buffer); i++)
			{
				if (temp_buffer[i] == '\n')
				{
					temp_buffer[i] = ' ';
				}

				if (temp_buffer[i] == EOF)
				{
					temp_buffer[i] = '\0';
				}
			}

			/* Close and restore STDIN */
			fclose(file);
			freopen("/dev/tty", "r", stdin);

			/* Create main buffer for processed input */
			input_buffer = malloc(sizeof(char) * (strlen(temp_buffer) + strlen(arguments[0]) + 2));
			sprintf(input_buffer, "%s %s", arguments[0], temp_buffer);

			/* Deal with unused buffers and set necessary flags */
			input_arguments = parse_inputs(input_buffer);
			free(temp_buffer);
			free(input_buffer);
			argument_flag = 2;

			/* Get official argument count */
			arg_count = 0;
			for (i = 0; input_arguments[i] != NULL; i++)
			{
				arg_count++;
			}
			arg_count--;
		}
		else if (strcmp(arguments[arg_count], "&") == 0)
		{
			background_flag = 1;
			arg_count--;

			// Build and populate argument array 
			background_arguments = malloc(sizeof(char*) * (arg_count + 2));
			for (i = 0; i < arg_count + 1; i++)
			{
				background_arguments[i] = malloc(sizeof(char) * (strlen(arguments[i]) + 1));
				sprintf(background_arguments[i], "%s", arguments[i]);
			}

			background_arguments[arg_count + 1] = NULL;
			argument_flag = 3;
		}	
	}
	
	/* Argument multiplexer that decides which argument array to use */
	/* Output */
	if(argument_flag == 1)
	{
		used_arguments = output_arguments;
	}
	/* Input */
	else if (argument_flag == 2)
	{
		used_arguments = input_arguments;
	}
	/* Background */
	else if (argument_flag == 3)
	{
		used_arguments = background_arguments;
	}
	/* Default*/
	else
	{
		used_arguments = arguments;
	}

	/* Exit shell */	
	if (strcmp(used_arguments[0], "exit") == 0)
	{
		exit(0);
	}

	pid = fork();
	/* Argument Parser */
	/* Child process */
	if (pid == 0)
	{
		/* Get the sum */
		if (strcmp(used_arguments[0], "add") == 0)
		{
			printf("m_shell > ");
			if (arg_count == 0){
				printf("ARGUMENT ERROR: There are no numbers to add\n");
				return 1;
			}

			for (i = 1; i < arg_count; i++)
			{
				temp_num = convert_to_int(used_arguments[i]);
				sum = sum + temp_num;
				printf("%d + ", temp_num);
			}
			temp_num = convert_to_int(used_arguments[arg_count]);
			sum = sum + temp_num;
			printf("%d = %d\n", temp_num, sum);
			exit(0);
		}
		/* Get the average */
		else if (strcmp(used_arguments[0], "ave") == 0)
		{
			printf("m_shell > ");	
			if (arg_count == 0){
				printf("ARGUMENT ERROR: There are no numbers to get the average\n");
				return 1;
			}
	        printf("(");
			for (i = 1; i < arg_count; i++)
			{
				temp_num = convert_to_int(used_arguments[i]);
				sum = sum + temp_num;
				printf("%d + ", temp_num);
			}
			temp_num = convert_to_int(used_arguments[arg_count]);
			sum = sum + temp_num;
			ave = (double)sum/(double)arg_count;
			printf("%d) / %d = %.2f\n", temp_num, arg_count, ave);	
			exit(0);		
		}
		/* Display the number of arguments */
		else if (strcmp(used_arguments[0], "args") == 0)
		{
			printf("m_shell > argc = %d, args = ", arg_count);
			if (arg_count > 0)
			{
				for (i = 1; i < arg_count; i++)
				{
					printf("%s, ", used_arguments[i]);
				}
				printf("%s\n", used_arguments[arg_count]);
			}
			else
			{
				printf("\n");
			}
			exit(0);
		}
		/* Override - do nothing */
		else if (strcmp(used_arguments[0], "override") == 0)
		{
			exit(0);
		}
		/* Execute built in processes */
		else
		{
			execvp(used_arguments[0], used_arguments);
			perror("COMMAND ERROR: ");
			exit(-1);
		}
	}
	/* Continue parent process */
	/* Fork error */
	else if (pid < 0)
	{
		perror("FORKING ERROR: ");
	}

	/* Wait for the process to end if not ran in background */
	if (!background_flag)
	{
		waitpid(pid, &status, 0);
	}
	else
	{
		printf("+Background: %s[%d]\n", used_arguments[0], pid);
	}

	/* Closes STDOUT stream after use */
	if(file_flag == 1)
	{
		fclose(file);
		freopen("/dev/tty", "a", stdout); // Restore STDOUT stream after being closed
	}

	if(argument_flag > 0)
	{
		for (i = 0; i < arg_count + 1; i++)
		{
			free(used_arguments[i]);
		}	
		free(used_arguments);
	}

	return 1;
}

// Main Loop
int main()
{
	char * line;
	char ** arguments;
	int i = 0;
	int status = 0;

    while(status != -1)
    {
    	printf("m_shell >");
    	line = get_input(); // Retrieve input from shell interface
        arguments = parse_inputs(line); // Parse the input from get_input() function
        status = exe_command(arguments); // Execute arguments from parse_inputs() function

        /* Free main allocated objects */
        free(line);
		for (i = 0; arguments[i] != NULL; i++)
		{
			free(arguments[i]);
		}	
        free(arguments);
    }
    return 0;
}