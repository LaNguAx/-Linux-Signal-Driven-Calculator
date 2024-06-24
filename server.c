#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <time.h>

#define MAX_ARRAY_SIZE 5
#define MAX_STRING_LEN 25



void handleError(int error_code);
char** loadLinesToBuffer(int fd, int* num_of_students);
void handleSignal(int signal);


int main(int argc, char** argv) {

	signal(SIGUSR1, handleSignal);

	// stay in waiting state untill signal is received..
	while(1) {
		pause();
	}

	while(wait(NULL) != -1);

	exit(0);
}



void handleSignal(int sig) {

	if(sig != SIGUSR1) return;

	int status;

	if(fork() == 0) {

		int fd = open("to_srv.txt", O_RDONLY);
		handleError(fd);

		int line_count = 0;
		char** data = loadLinesToBuffer(fd, &line_count);
		close(fd);

		// delete to_srv.txt
		handleError(unlink("to_srv.txt"));

		int sentProcessID = atoi(data[0]);
		int num1 = atoi(data[1]);
		int num2 = atoi(data[2]);
		int operation = atoi(data[3]);

		char responseBuffer[MAX_STRING_LEN];

		switch(operation) {

			case 1:
				sprintf(responseBuffer, "Result is: %d\n", num1+num2);
				break;
			case 2:
				sprintf(responseBuffer, "Result is: %d\n", num1-num2);
				break;
			case 3:
				sprintf(responseBuffer, "Result is: %d\n", num1*num2);
				break;
			case 4:
				// if division by 0
				if(num2 == 0) strcpy(responseBuffer, "You can't divide by 0!\n");
				else
					sprintf(responseBuffer, "Result is: %d\n", num1/num2);

				break;
			default:

				printf("Invalid operation entered..");
				kill(sentProcessID, SIGUSR2);
				break;

		}

		// now write response to specific user file
		char pathBuffer[MAX_STRING_LEN];
		sprintf(pathBuffer, "response_client_%d.txt", sentProcessID);
		int userFd = open(pathBuffer, O_RDWR | O_CREAT | O_TRUNC, 0666);
		handleError(userFd);

		int bytesWrite = write(userFd, responseBuffer, strlen(responseBuffer));
		if(bytesWrite < 0) {
			close(userFd);
			exit(-1);
		}
		close(userFd);

		// send a signal back to process ID that its done calculating the response.
		kill(sentProcessID, SIGUSR1);
		exit(0);

	}
	wait(&status);

	signal(SIGUSR1, handleSignal);


}


void handleError(int error_code) {

        if(error_code < 0) {
                perror(NULL);
                exit(-1);
        }
}

char** loadLinesToBuffer(int fd, int* num_of_students) {

        handleError(fd);

        char** namesArray = NULL;
        char buffer[MAX_ARRAY_SIZE][MAX_STRING_LEN] = {0};
        char current_char = '\0';
        int i = 0;
        int numOfStudents = 0;

        int bytesRead = -1;
        do {
                bytesRead = read(fd, &current_char, 1);
                //if error then handle it.
                handleError(bytesRead);

                if(bytesRead  <= 0) break;

                if(current_char == '\n') {

                        buffer[numOfStudents][i] = '\0';
                        numOfStudents++;

                        i = 0;
                        continue;
                }
                buffer[numOfStudents][i] = current_char;
                i++;

        } while(bytesRead > 0);


        namesArray = (char**)calloc(numOfStudents, sizeof(char*));

        // allocate memory for each student name exact size
        for(int i = 0; i < numOfStudents; i++) {

                namesArray[i] = (char*)calloc(strlen(buffer[i])+1, sizeof(char));
                strcpy(namesArray[i], buffer[i]);

                //namesArray[i][strlen(buffer[i])+1] = '\0';
        }

        *num_of_students = numOfStudents;
        return namesArray;
}
