#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>



typedef struct alias_s {
	char * a_str;// alias string
	char **r_cmd;// replacement cmd

}Alias;

Alias *aliases;
int aliasc=0;



char line[512];



/*
 *the main cmd to be execute is cmd[0]
 *if main cmd is found
 *newline char and empty strings as addtional cmd args triggers error from execvp, but execvp executes still and does not return
 *if main cmd is a proper string but not found it will throw cmd not found error, child killedwith return value, and return to main loop
 *above proper strings inclue \n and empty string
 * if main cmd is null char child gets destroyed by segfault without return value and seemingly nothing happen
 *
 *
 *
 *
 *
 *
 */




int run_general(char **cmd, int *cmdc, int *redirection_fd) {
	int i=0;
	char *tmp;
	// parse cmd for $ char
	while (cmd[i]!=NULL) {
		tmp=strdup(cmd[i]);
		if(tmp[0]=='$') {// if arg is environment var
			tmp=tmp+1;
			// if environment var does not exist cmd gets replaced by NULL
			cmd[i]=getenv(tmp);
			
		}
		


		i++;
	}		
	

	// if one of aliases
	for (i=0;i<aliasc;i++) {
                if(!strcmp(aliases[i].a_str,cmd[0])) {// if alias existed
				cmd=aliases[i].r_cmd;                                
                }
        }

			 	
	int pid=fork();		
	
	if (pid==0) {// child process
		if (*redirection_fd!=-1){// redirection 
			dup2(*redirection_fd,STDOUT_FILENO);



		}	
		int ret=execvp(cmd[0],cmd);
		
		if (ret==-1) {
			write(STDERR_FILENO,"Error: command not found\n",strlen("Error: command not found\n"));
			exit(errno);// without exit your parent is perma waiting on child to finish and users
				    // are passing cmd to child.  
	
		}	

	}

	if(pid<0) {// child creation failed
		return 0;

	}

	
	
	waitpid(pid,NULL,0);
	return 0;
	
	

} 



// exit the shell

void func_exit(){

	exit(0);
}


// function that parse the alias cmd and args and handles the cmd arg requirements

int func_alias(char **cmd, int *cmdc) {
	//char tmp2[512];
	//assume alias names are not "", or \n, \0
	//assume replacement string not \n, ""
	//if either a_str or r_str arg is \n we pretend that arg is missing
	if (*cmdc==1)  {// print all alias
		for (int i = 0; i < aliasc; i++) { 
        
			write(STDOUT_FILENO,aliases[i].a_str,strlen(aliases[i].a_str));
	
			write(STDOUT_FILENO,"=",strlen("="));
			write(STDOUT_FILENO,"'",strlen("'"));
			
			int j=0;
			while (aliases[i].r_cmd[j]!=NULL) {

				if(j!=0) {// when not first cmd print space
					 write(STDOUT_FILENO," ",strlen(" "));
	
				}	
				write(STDOUT_FILENO,aliases[i].r_cmd[j],strlen(aliases[i].r_cmd[j]));
				j++;
		
			}	

			write(STDOUT_FILENO,"'",strlen("'"));
			write(STDOUT_FILENO,"\n",strlen("\n"));

    		} 	

		return 0;
	}	

	else if (*cmdc==2) {// display given alias
		for (int i=0;i<aliasc;i++) {
                	if(!strcmp(aliases[i].a_str,cmd[1])) {// if alias existed
               			write(STDOUT_FILENO,aliases[i].a_str,strlen(aliases[i].a_str));
				write(STDOUT_FILENO,"=",strlen("="));
				write(STDOUT_FILENO,"'",strlen("'"));
				int j=0;
				while (aliases[i].r_cmd[j]!=NULL) {

	                                if(j!=0) {// when not first cmd print space
        	                                 write(STDOUT_FILENO," ",strlen(" "));

                	                }
                        	        write(STDOUT_FILENO,aliases[i].r_cmd[j],strlen(aliases[i].r_cmd[j]));
                                	j++;

                        	}
                             	write(STDOUT_FILENO,"'",strlen("'"));
								write(STDOUT_FILENO,"\n",strlen("\n"));

                                return 0;
                        }

		}
		write(STDERR_FILENO,"Error: alias not found\n",strlen("Error: alias not found\n"));
		return 1;
	
	}
	else  {// set up new alias
	       int existed=0;
	       int e_i;
	       int tar_i;
	       if (aliases!=NULL) {
			for (int i=0;i<aliasc;i++) {
				if(!strcmp(aliases[i].a_str,cmd[1])) {// if alias existed
					existed=1;
					e_i=i;
					break;
				}
			}
		}



		if (existed) {
			tar_i=e_i;
		}
		else{
			tar_i=aliasc;
		
			aliases = realloc(aliases, (aliasc+1) * sizeof(Alias));
		}

		aliases[tar_i].a_str=strdup(cmd[1]);// set up alias name
		aliases[tar_i].r_cmd=malloc((512+1)*sizeof(char*));
		int i=0;
                int j=2;// alias_cmd alias_name replacecmd1....

		while(cmd[j]!=NULL) {// fill replace command

	                aliases[tar_i].r_cmd[i]=strdup(cmd[j]);

	                

			i++;
			j++;
		
		}
		
		


		aliases[tar_i].r_cmd[i]=NULL; //NULL termination
		if(!existed) {
			aliasc++;	
		}

		
		return 0;
	}


}

// wrapper function for setenv()
int func_export(char ** cmd,int *cmdc) {
	// assume newline char wont be the env variable
	char tmp2[512];
	char name[512];
	char value[512];
	strcpy(tmp2,cmd[1]);
	strcpy(name,strtok(tmp2,  "="));// name
	strcpy(value,strtok(NULL,  "="));// value
	setenv(name,value,1);// overwrite if exist
	
	return 0;

}

// unset all environment variables passed with unsetenv
int func_unset(char **cmd,int *cmdc) {
	char tmp2[512];
	int i=1;
	while(cmd[i]!=NULL) {
		strcpy(tmp2,cmd[i]);
		if(getenv(tmp2)==NULL) {// check if environment variable exist
			write(STDOUT_FILENO,"unset: environment variable not present\n",strlen("unset: environment variable not present\n"));
		}
		else{		
			unsetenv(tmp2);
		}
		i++;
	}

	return 0;
}




/**
 *
 *returns 0 if cmd[0 ]not built in func, 1 if built in
 *if is built in call the corresponding built in function
 *
 */


int sort_built_in (char **cmd,int *cmdc) {
	if(!strcmp(cmd[0],"exit")) {
		func_exit();
		return 1;
		
	}
	
	


	else if(!strcmp(cmd[0],"alias")) {
		func_alias(cmd,cmdc);
		return 1;
	}

        else if(!strcmp(cmd[0],"export")) {
                func_export(cmd,cmdc);
                return 1;
        }
	
	else if(!strcmp(cmd[0],"unset")) {
                func_unset(cmd,cmdc);
                return 1;
        }


	else{
		return 0;
	}
}







/*
 *this function parse an input line into array of tokenized args
 *this means newline chars are removed from line when parse
 *Notice if an empty line(a line with only \n by simply pressing return) is parsed, the parsed result is NULL char
 * at cmd[0]
 *
 *
 *
 *
 *line: the line to be parsed. after parsing only the first token is kept
 *cmd: initially empty. after parsing it is an array of cmd argument tokens excluding redirection char and file
 *cmdc: number of cmd args
 *redirect_fd: fd of the redirection file in cmd arg.-1 if no redirection file or cant be opened
 *
 * 
 *
 */

int parse_line(char *line,char **cmd,int *cmdc,int *redirect_fd){
	*redirect_fd=-1;
	*cmdc=0;
	char *tmp;
	char tmp_line[512];
	char tmp2[512];
	strcpy(tmp_line,line);
	int i=0;
	int redirect=0;


	

	
	tmp=strtok(line,  " "); // Fariha had " -/" but having - means ls -al wont run
	if(tmp==NULL) {// NULL char
		return 1;

	}
	if (!strcmp(tmp,">")||!strcmp(tmp,">\n")){// is first token redirection char
		
		//write(STDERR_FILENO, "Redirection error: first char is >\n", strlen("Redirection error: first char is >\n"));
	
		write(STDERR_FILENO, "Redirection error\n", strlen("Redirection error\n"));	
		return 1;
	}


  	while (tmp!=NULL) {// tokenize line until empty

		if (!strcmp(tmp,">")||!strcmp(tmp,">\n")) {// check > char
							   // the > format means > in the middle must be follow by a space or \n
							   // if followed by space then strcmp(tmp,>)can detect it
			redirect=1;
			break;
		}

		

  		
		cmd[i]=tmp;
		i=i+1;
		tmp=strtok(NULL," ");

 	}	
	
	

    // check special characters
	// \n and "" and \0 gets treated as valid tokens and therefore args and cause issues to execvp(), but that is intended if they are passed 
	// in as args so dont care
	// \n "" and \n if passed as main arg can cause huge issue
	// \0 passed in as main arg will cause whole line to be skipped
	// \n after parsing becomes \0 if it is the only arg
	// assume only \n can be parsed and need to deal with, and by definition if a token as newline char it has to be the last token in a line
	// as of now "" is the only unhandled exception and will be passed to built_in and run_general
	
	
	
	
	
	int len=strlen(cmd[i-1]);

	if (!strcmp(cmd[i-1]+len-1,"\n")) {// when last token has \n or is \n
    	char str[sizeof(cmd[i-1])];
		strcpy(str,cmd[i-1]);
		str[strcspn(str, "\n")] = 0;
		if (!strcmp(cmd[i-1],"\n")) {// when last token is \n
			cmd[i-1] =  '\0'; // dereference token  EMPTYING IS NOT ENOUGH
		}
		else {
			cmd[i-1]=str; // remove \n from last token
		}
	}

	if(cmd[0]!=NULL) {// if cmd[0]is null we want to make sure cmdc stays at 0
                                       
		*cmdc=i;
                              
	}


	// NULL termination of args is handled in run_general

	if (redirect==1) {
		strcpy(tmp_line,strchr(tmp_line,'>'));
		

		if(!strcmp(tmp_line,">\n")) {// no file to direct to if >\n
			//write(STDERR_FILENO, "Redirection error:NO FILE\n", strlen("Redirection error:NO FILE\n"));
			write(STDERR_FILENO, "Redirection error\n", strlen("Redirection error\n"));

			return 1;	
		}

 		// now at least it is > \n so no seg fault if strtok
			
		strcpy(tmp2,strtok(tmp_line,  " "));// >
        	

                strcpy(tmp2,strtok(NULL,  " "));// arg after >
		


                if(!strcmp(tmp2,"\n")) {// no file to redirect to if > \n
                        
			//write(STDERR_FILENO, "Redirection error:NO FILE\n", strlen("Redirection error:NO FILE\n"));
			write(STDERR_FILENO, "Redirection error\n", strlen("Redirection error\n"));

                        return 1;
                }

                else if(!strcmp(tmp2,">")||tmp2[0]=='>') {// too many redirection > >\n  or  > > \n 
							
                        //write(STDERR_FILENO, "Redirection error:TOO MANY >\n", strlen("Redirection error:TOO MANY >\n"));
                        write(STDERR_FILENO, "Redirection error\n", strlen("Redirection error\n"));

			return 1;
                }

                else {	

			if(strtok(NULL," ")) {// too many redirection files if the argument after file argument exists
                                //write(STDERR_FILENO, "Redirection error:TOO MANY FILE\n", strlen("Redirection error:TOO MANY FILE\n"));
                                write(STDERR_FILENO, "Redirection error\n", strlen("Redirection error\n"));

                                return 1;
                        }

			// often this args comes in the form of file.txt\n
			if(tmp2[strlen(tmp2)-1]=='\n') {// if last char is newline char
							
				tmp2[strcspn(tmp2, "\n")] = 0;
			}
			

			FILE *file=fopen(tmp2,"w+");
                        *redirect_fd=fileno(file);
                        if(*redirect_fd==-1) {// cannot open file to write
                                //write(STDERR_FILENO, "Redirection error:CANT OPEN FILE\n", strlen("Redirection error:CANT OPEN FILE\n"));
                                write(STDERR_FILENO, "Redirection error\n", strlen("Redirection error\n"));

				return 1;
                        }

                }

	}

	

  	return 0;


}


/*
 *reads all lines smaller than max line length, if longer discard parts after max line length
 *a loop function if interactive mode, keep prompting user for addition
 *inputs, then parses the input and attempt to run them as cmd args
 *
 *if batch mode, reads and echo every lines in batch file and attempt to execute line
 *as cmd args. If attempt fail move to next line. Terminate if all lines are read
 *
 * argc==1: interactive
 * argc==2: batch
 * other argc: wrong argc number shell won't run
 *
 */



int main(int argc, char** argv) {
	char *open_bfile_err="Error: could not open batchfile\n";
	int batch_fd = -1;
	int redirect_fd = -1;
	int cmdc=0;
	char **cmd;
	
		       


   	if (argc == 1) {//interactive mode
   		while(1){			
			// do we add prompt in new line for long lines?
			// do we check eof char?
			// so far no

   			write(STDOUT_FILENO, "wish> ", strlen("wish> "));
			// leave command length check to others
   			fgets(line,512,stdin);// null char? and other edge case?
   			//write(STDOUT_FILENO, line, strlen(line));
			cmd=malloc((sizeof(line)+1)*sizeof(char*));
			char input[sizeof(line)];
			strcpy(input,line);

   			if(!parse_line(input,cmd,&cmdc,&redirect_fd)) {// if line is not empty or redirection check is passed
				 //write(STDOUT_FILENO, "made it\n", strlen("made it\n"));

				if(cmd[0]==NULL) {// empty line will cause this
					//write(STDOUT_FILENO, "you passed null cmd", strlen("you passed null cmd"));
					
					
					
				}
		
				else{
					cmd[cmdc]=NULL; //NULL termination
			



					if (sort_built_in(cmd,&cmdc)){	
				   	   
			      	
					}		
		   			else{
						
						run_general(cmd,&cmdc,&redirect_fd);
	
					}	
				}
			}
			


			


   		}
   	}

   	else if (argc == 2) {//batch mode
		batch_fd = open(argv[1], O_RDONLY);
		if (batch_fd == -1) {
		   	write(STDERR_FILENO, open_bfile_err, strlen(open_bfile_err));
		   	return 1;
		}
		FILE *file=fdopen(batch_fd,"r");

	   	while (fgets(line,512,file)) {
	   		
			write(STDOUT_FILENO, line, strlen(line));
			cmd=malloc((sizeof(line)+1)*sizeof(char*));
			char input[sizeof(line)];
            strcpy(input,line);



            	if(!parse_line(input,cmd,&cmdc,&redirect_fd)) {// if line is not empty or redirection check is passed
				 	//write(STDOUT_FILENO, "made it\n", strlen("made it\n"));

					if(cmd[0]==NULL) {// empty line will cause this
					//write(STDOUT_FILENO, "you passed null cmd", strlen("you passed null cmd"));
					
					
					
					}
		
					else{
						cmd[cmdc]=NULL; //NULL termination
			



						if (sort_built_in(cmd,&cmdc)){	
			   	   		
			      	
						}		
		   				else{
						
						run_general(cmd,&cmdc,&redirect_fd);
	
						}	
					}
				}
		}               
	}

	else  {

   	        write(STDERR_FILENO, "too many arguments\n", strlen("too many arguments\n"));
   	     	return 1;
   
	}


	

   	return 0;





}
