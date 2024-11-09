#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>


int main (int argc, char *argv[]) {
	       
        int j;
        int i;
        char* name;
        char outname[120];
        struct stat buf;
        int err;
        FILE * file;
        char * line;
        size_t len = 0;
        int fileno = 2;
        char *all_lines;
        char *tmp;
	FILE * tfile = fopen(argv[1], "w+");



	if(argc < 3) {
		printf("wisc-tar: tar-file file1 […]\n");
		exit(1);
	}

	if(argv[2] == NULL) {
		exit(1);
	}
	

	
        while ( fileno < argc ) {
		
		//file name
	        name  = argv[fileno];
        	for(j = 0; j < 120; j++) {// potentially pad filename 
                	if(j <= strlen(name)){
                        	outname[j] = name[j];
                	}

			else {
                        	outname[j] = '\0';
                	}
        	}
		
		fwrite(outname, 1, sizeof(outname), tfile);
                //printf("%s\n",str_filename);
		


		//size 
        	err = stat(name, &buf);
        	if(err != 0)	{
			 printf("wisc-tar: cannot open %s\n",name);
                	exit(1);
        	}
        	fwrite(&buf.st_size, 8, 1, tfile);



		// content
	        file = fopen(name, "r");
        	if (file == NULL) {
                	printf("wisc-tar: cannot open %s\n",name);
        		exit(1);
		}

		
		all_lines = "";
	        tmp="";
        	while(getline(&line, &len, file) != -1) {
            	    tmp=all_lines;
           	    all_lines=(char*)malloc(strlen(line)+strlen(all_lines)+1);
            	    strcpy(all_lines,tmp);
           	    strcat(all_lines,line);

        	}
       		//printf("%s\n",all_lines);
        	//printf("%ld\n",strlen(all_lines));
       		//printf("%ld\n",strlen(all_lines)+512-strlen(all_lines)%512);
        	char content[strlen(all_lines)+512-strlen(all_lines)%512];
        	for (i=0; i<strlen(all_lines)+512-strlen(all_lines)%512; i++) {// potentially pad content
                	if(i < strlen(all_lines)){
                    		*(content+i) = *(all_lines+i);
                	}

			else {
                    	content[i] = '\0';
                	}


                }

        	//printf("%s\n",content);
        	fwrite(content,sizeof(content), 1, tfile);
		
    		
				
		fileno++;
	}

	return 0;
}

