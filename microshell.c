#include<stdio.h>
#include<unistd.h> /* for cwd(), chdir(), fork(), exec*() */
#include<pwd.h> /* for passwd struct */
#include<time.h> /* for localtime implementation - showdate()*/
#include<stdlib.h> /* for malloc(), realloc() i free() */
#include<string.h> /* for strtok(), strrchr() */
#include<errno.h>
#include<dirent.h> /* for DIR* */
#include<sys/types.h>
#include<sys/wait.h>

#define INPUT_MAX 512

char cwd[PATH_MAX];
char getpath[PATH_MAX];

void help(){
	puts("\033[1;34m\nAuthor: Nicol Radwan"
		 "\nIndex no: 416155\n"
		 "\nAvaible functions:\n"
		 "\t exit - quits the program\n"
		 "\t cd - changes the directory\n"
		 "\t help - shows author and functions\n"
		 "\t date - shows current time\n"
		 "\t login - shows current user name\n"
		 "\n runs executable files"
	     "\n\033[0m");
}

void path(){
    if(getcwd(cwd, sizeof(cwd)) == NULL)
    	perror("getcwd() error");
	else
    	printf("[\e[92m%s\e[39m] $ ", cwd);
}

void cdprev(){
	char *tmp = strrchr(cwd, '/');
	*tmp = '\0';
	chdir(cwd);
}

void cdhome(){
	char *cwd = getenv("HOME");
	chdir(cwd);
}

void cdnext(const char *getpath){
	strcat(cwd, "/");
	DIR* folder = opendir(getpath);
	if (folder){
		closedir(folder);
		strcat(cwd, getpath);
		chdir(cwd);
	}
	else if (ENOENT == errno){
		puts("cd:	No such directory");
	}
}

void showlogin(){
	char *usrlogin;
	struct passwd *pass;
	pass = getpwuid(getuid());
	usrlogin = pass->pw_name;
	printf("%s\n", usrlogin);
}

void showdate(){
	time_t t = time(NULL);
	struct tm* lt = localtime(&t);
	printf("%i.%i.%i %i:%i:%i\n",
		lt->tm_mday, lt->tm_mon+1,
		lt->tm_year+1900, lt->tm_hour,
		lt->tm_min, lt->tm_sec);
}

void executeit(char **tokens){	
	int status;
	pid_t pid;
	pid = fork();

	if(pid == 0){
		if(execvp(tokens[0], tokens) == -1);
		printf("Invalid command.\n");
		exit(1);
	} else {
		waitpid(pid, &status, WUNTRACED);
	}	
}

char **split_into_tokens(char *usrinput){
	char separators[] = " \n";
	int count = 0;	

	char **tokens = malloc(sizeof(char *));
	char *token = strtok(usrinput, separators);
	
	while (token != NULL){

			tokens[count++] = token;
			
			tokens = realloc(tokens, sizeof(char *) * (count+1));
			token = strtok(NULL, separators);
	}	
	tokens[count] = NULL;
	return tokens;
}


int main(int argc, char* argv[]){
	
	char usrinput[INPUT_MAX];
	char **tokens;
	
	while (1){

		path();
		fgets(usrinput, INPUT_MAX, stdin);
		
		tokens = split_into_tokens(usrinput);

		if(strcmp(usrinput, "help")==0 && tokens[1]==NULL){
			help();
		} else if(strcmp(usrinput, "cd" )==0 && tokens[1]==NULL) {
			cdhome();
		} else if(strcmp(usrinput, "cd")==0 && strcmp(tokens[1], "..")==0) {
			cdprev();
		} else if(strcmp(usrinput, "cd")==0 && tokens[1]!=NULL) {
			strcpy(getpath, tokens[1]);
			cdnext(getpath);
		} else if(strcmp(usrinput, "login")==0 && tokens[1]==NULL){
			showlogin();
		} else if(strcmp(usrinput, "date")==0 && tokens[1]==NULL){
			showdate();
		} else if(strcmp(usrinput, "exit")==0) {
			break;	
		} else {
			executeit(tokens);
		}   
		free(tokens);			
	}
	return (0);
}