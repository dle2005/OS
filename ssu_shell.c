#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

char **tokenize(char *line);

int main(int argc, char* argv[]) {
	char  line[MAX_INPUT_SIZE];            
	char  **tokens;              
	int i;
    
    pid_t pid;
    int fd[2];
    int in = 0;

	FILE* fp;
	if(argc == 2) {
		fp = fopen(argv[1],"r");
		if(fp < 0) {
			printf("File doesn't exists.");
			return -1;
		}
	}

	while(1) {			
		bzero(line, sizeof(line));
		if(argc == 2) { // batch mode
			if(fgets(line, sizeof(line), fp) == NULL) { // file reading finished
				break;	
			}
			line[strlen(line) - 1] = '\0';
		} else { // interactive mode
			printf("$ ");
			scanf("%[^\n]", line);
			getchar();
		}

		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line);

        int pipe_count = 0;
        for(int i = 0; tokens[i] != NULL; i++) {
            if(!strcmp(tokens[i], "|")) pipe_count++;
        }

        char ***inputs = (char ***)malloc(MAX_INPUT_SIZE * sizeof(char**));
        char *ptr = strtok(line,"|");
        int num_comm = 0;
        while(ptr != NULL) {
            inputs[num_comm] = tokenize(ptr);
            num_comm++;
            ptr = strtok(NULL, "|");
        }



        for(int i = 0; i < pipe_count + 1; i++) {
            if(pipe(fd) < 0) {
            }

            if((pid = fork()) < 0) {
            }
            else if(pid == 0) {
                dup2(in, 0);
                if(i < pipe_count) {
                    dup2(fd[1], 1);
                }
                close(fd[0]);

//                if(execvp(inputs[i][0], inputs[i]) == -1) {
//                    printf("SSUShell : Incorrect command\n");
//                    exit(3);
//                }
                        

                if(!strcmp(inputs[i][0], "ttop")) {
                    if(execvp("./ttop", inputs[i]) == -1) {
                        printf("SSUShell : Incorrect command\n");
                        exit(3);
                    }
                }
                else if(!strcmp(inputs[i][0], "pps")) {
                    if(execvp("./pps", inputs[i]) == -1) {
                        printf("SSUShell : Incorrect command\n");
                        exit(3);
                    }
                }
                else {
                    if(execvp(inputs[i][0], inputs[i]) == -1) {
                        printf("SSUShell : Incorrect command\n");
                        exit(3);
                    }
                }
                
            }
            else {
                wait(NULL);
                close(fd[1]);
                in = fd[0];
            }
        }

		for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		free(tokens);

	}
	return 0;
}

char **tokenize(char *line)
{
  char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  int i, tokenIndex = 0, tokenNo = 0;

  for(i =0; i < strlen(line); i++){

    char readChar = line[i];

    if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
      token[tokenIndex] = '\0';
      if (tokenIndex != 0){
	tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
	strcpy(tokens[tokenNo++], token);
	tokenIndex = 0; 
      }
    } else {
      token[tokenIndex++] = readChar;
    }
  }
 
  free(token);
  tokens[tokenNo] = NULL ;
  return tokens;
}


