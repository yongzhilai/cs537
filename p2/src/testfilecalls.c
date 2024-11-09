#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"


int main (int rag, char * argv[]){
	char * greetings="this is testfilecalls!";
	printf(1,"%s\n",greetings);
	printf(1,"this is what fgetoff()returns: %d !\n",fgetoff(0));
	printf(1,"this is what fsetoff()returns%d !\n",fsetoff(0,0));

	exit();


}
