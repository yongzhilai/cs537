#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    // parsing cmd line	
    char* ss = NULL;
    char* rs = NULL;
    FILE* file = NULL;
    int opt;
    int s = 0, r = 0, f = 0;
    int o = 0, n = 0, c = 0;
    FILE* ofile = NULL;
    int lnum;

    //printf("the cmd args are: ");
    
    /*for (int i = argc-1; i >=0 ; i--) {
    	int opt; 
      	printf("%s\n ",argv[i]);
    }*/
    // put ':' in the starting of the 
    // string so that program can  
    //distinguish between '?' and ':'  
    while((opt = getopt(argc, argv, ":s:r:f:o:n:c")) != -1)  
    {  
        switch(opt)  
        {  
            case 's':
		s=1;  
		ss = optarg;
		if (ss == NULL) {
		    printf("NULL for Search String\n");
		    exit(1);
		}  
		break;
            case 'r': 
		r=1;
	        rs = optarg;
		if (rs == NULL) {
		    printf("NULL for replace String\n");
		    exit(1);
		}
		break;	
            case 'f': 
	        f=1;	
                file = fopen(optarg, "r");
           	// checking if the file is open successfully
           	if (file == NULL) {
                	printf("wisc-sed: cannot open file\n");
                	exit(1);
   	        }
                break;  

	    case 'o':
		o=1;
		ofile = fopen(optarg, "w");
                // checking if the file is open successfully
                if (ofile == NULL) {
                        printf("wisc-sed: cannot write to output file\n");
                        exit(1);
                }

		break;

	    case 'n':
		n=1;
		lnum= atoi(optarg);
		break;

	    case 'c':
		c=1;
		break;
          
            case ':':  // mandatory option are missing args 
                printf("usage: wisc-sed [optional flags] -s <search string> -r <replacement string> -f <file>\n");
		exit(1);  
                break;  
            case '?':  // ignore strange args
                //printf("unknown option: %c\n", optopt); 
                break;  
        }  
    }  

   if (s==0||r==0||f==0) {
	printf("usage: wisc-sed [optional flags] -s <search string> -r <replacement string> -f <file>\n");
	exit(1);
   }
    

    
    char *all_lines = "";
    char *line = NULL;
    size_t len = 0;
    char * tmp;
    char * rs_tmp;
    char * token;
    int str_err=0;
    int cnt=0;
    int n_switch=0;
    //printf("right before getline\n");
    while (getline(&line, &len, file) != -1) {// parse each line
	    //printf("Line read: %s\n",line);
	    
            tmp=line;
	    if (c==1) {
	        tmp=strcasestr(tmp,ss);
	    }
	    else{
	        tmp=strstr(tmp,ss);
	    }
	    if (!strcmp("",ss)||!strcmp(ss,rs)){// if ss is empty string [strstr()returns tmp] or ss is same with rs then no job needs to be done

		str_err=1;
	    }
	    if (cnt==lnum-1) {
		n=0;
		n_switch=1;
	    }

	    //printf("%s\n",tmp);
	    while (tmp&&(!str_err)&&(n==0)) {// search string exist
	        //printf("%s\n",tmp);
		token=(char*)malloc(strlen(line)+strlen(rs)+1-strlen(ss));// for each replace max size is line + rs - ss
		strncpy(token,line,strlen(line)-strlen(tmp));
		//printf("%s\n",token);
	    	tmp=tmp+(int)strlen(ss);// get trailing
	        //printf("%s\n",tmp);
		rs_tmp=(char*)malloc(strlen(tmp)+strlen(rs)+1);
	    	strcpy(rs_tmp,rs);
	    	strcat(rs_tmp,tmp);// concat rs and trailing
		//printf("%s\n",tmp);
	    	strcat(token,rs_tmp);// reverse the split
	    	line=token;
		//printf("%s\n",line);
		
			
	    	if (c==1){
		    tmp = strcasestr(tmp,ss);
		}
		else{
		    tmp = strstr(tmp,ss);
		}
		
            }
	    if (n_switch==1){
		n=1;
	    }
            //printf("Finished Line: %s\n",line);
	    tmp=all_lines;
	    all_lines=(char*)malloc(strlen(line)+strlen(all_lines)+1);
	    strcpy(all_lines,tmp);
	    strcat(all_lines,line);
	    cnt++;
    }

    fclose(file);
    if (o==0) {
    	printf("%s",all_lines);
    }
    else {
        fputs(all_lines,ofile);
	fclose(ofile);
	
    }	    
	
    
    //free(tmp);
    //free(rs_tmp);
    //free(token);
    return 0;
    



}


