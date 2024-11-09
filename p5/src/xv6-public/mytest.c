#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "memlayout.h"
#include "mmu.h"
#include "wmap.h"




int main (int rag, char * argv[]){
        char * greetings="this is mytest!";

        

        
        printf(1,"%s\n",greetings);
        
        
        char * arr = (char*)wmap(0x60005000, 2*PGSIZE, MAP_FIXED | MAP_SHARED );
        char val='q';
        for (int i = 0; i < PGSIZE-1; i++) {
                arr[i] = val;
        }

        printf(1,"this is what wmap()returns: %s !\n",arr);
        struct wmapinfo winfo;
        winfo.total_mmaps = 0;
        printf(1,"this is what wmapinfo()returns: %d !\n",getwmapinfo(&winfo));
        printf(1,"totalmaps: %d !\n",winfo.total_mmaps);
        printf(1,"n_loaded_page: %d !\n",winfo.n_loaded_pages[0]);
        printf(1,"n_loaded_page: %d !\n",winfo.n_loaded_pages[1]);
        //printf(1,"this is what wmap()returns: %d !\n",wmap(0x60000000, PGSIZE + 8, MAP_SHARED | MAP_POPULATE));
        //printf(1,"this is what wmap()returns: %d !\n",wmap(0x60005000, PGSIZE+1, MAP_FIXED | MAP_SHARED | MAP_POPULATE));
        //printf(1,"this is what wmap()returns: %d !\n",wmap(0x60009000, PGSIZE + 8, MAP_FIXED | MAP_SHARED | MAP_POPULATE));
        //printf(1,"this is what wmap()returns: %d !\n",wmap(KERNBASE-3*PGSIZE, PGSIZE+1, MAP_FIXED | MAP_SHARED | MAP_POPULATE));
        //printf(1,"this is what wmap()returns: %d !\n",wmap(0x60005000, PGSIZE+1, MAP_FIXED | MAP_SHARED | MAP_POPULATE));
        



        /*
        printf(1,"this is what wmap()returns: %d !\n",wmap(0x70000000, PGSIZE+1, MAP_SHARED | MAP_POPULATE));
        printf(1,"this is what wmap()returns: %d !\n",wmap(0x70000000, PGSIZE+1, MAP_SHARED | MAP_POPULATE));
        printf(1,"this is what wmap()returns: %d !\n",wmap(0x70000000, PGSIZE+1, MAP_SHARED | MAP_POPULATE));
        printf(1,"this is what wmap()returns: %d !\n",wmap(-1, PGSIZE+1, MAP_SHARED | MAP_POPULATE));
        */
        


        /*printf(1,"this is what wmap()returns: %d !\n",wmap(0x70000000, 8192, MAP_SHARED | MAP_POPULATE));
        printf(1,"this is what wmap()returns: %d !\n",wmap(0x70000000, 8192, MAP_SHARED | MAP_POPULATE));
        printf(1,"this is what wmap()returns: %d !\n",wmap(0x70000000, 8192, MAP_SHARED | MAP_POPULATE));
        printf(1,"this is what wmap()returns: %d !\n",wmap(0x70000000, 8192, MAP_SHARED | MAP_POPULATE));
        printf(1,"this is what wmap()returns: %d !\n",wmap(0x70000000, 8192, MAP_SHARED | MAP_POPULATE));
        printf(1,"this is what wmap()returns: %d !\n",wmap(0x70000000, 8192, MAP_SHARED | MAP_POPULATE));*/

        /*
        struct pgdirinfo info;
        info.n_upages = -1;

        struct wmapinfo winfo;
        winfo.total_mmaps = 0;


        printf(1,"this is what getpgdirinfo()returns: %d !\n",getpgdirinfo(&info));
        printf(1,"n_upages: %d !\n",info.n_upages);
        
        printf(1,"this is what wmapinfo()returns: %d !\n",getwmapinfo(&winfo));
        printf(1,"totalmaps: %d !\n",winfo.total_mmaps);
        printf(1,"n_loaded_page: %d !\n",winfo.n_loaded_pages[0]);
        printf(1,"n_loaded_page: %d !\n",winfo.n_loaded_pages[1]);
        */

        //printf(1,"this is what wmap()returns: %d !\n",wmap(0x60000000, 8192, MAP_SHARED | MAP_POPULATE));
        //printf(1,"this is what fsetoff()returns%d !\n",fsetoff(0,0));
        //printf(1,"this is what wunmap()returns: %d !\n",wunmap(10));

        exit();


}