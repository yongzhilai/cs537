#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "unistd.h"
#include "mapreduce.h"


struct queue {
  char **store;     // where T is the data type you're working with
  size_t size;  // the physical array size
  size_t count; // number of items in queue
  size_t head;  // location to pop from
  size_t tail;  // location to push to
};

struct queue map_queue;
int file_counter=0;// counting how many file has been processed
int partition_counter=0; // counting how many partition has been processed

struct pair {
    char* key;
    char* value;
};

struct partition_info {
    struct pair** pairs;
    size_t num_pairs;
    size_t iterator;
    pthread_mutex_t lock;
};






struct partition_info** partitions;
int num_partition;





pthread_mutex_t mapper_lock;
pthread_mutex_t reducer_lock;
pthread_cond_t mapper_cond; 

Partitioner partitioner;
Reducer reducer;
Mapper mapper;




// push file in to a queue buffer
int push( struct queue *q, char *new_value )
{
  if ( q->count == q->size )
  {
    // queue full, handle as appropriate
    return 0;
  }
  else
  {
    q->store[q->tail] = strdup(new_value);
    q->count++;
    q->tail = ( q->tail + 1 ) % q->size;
  }
  return 1;
}

// pop file from a queue buffer
int pop( struct queue *q, char **value ){


  if ( q->count == 0 )
  {
    // queue is empty, handle as appropriate
    return 0;
  }
  else
  {
    *value=strdup(q->store[q->head]);
    q->count--;
    q->head = ( q->head + 1 ) % q->size;
  }

  return 1;
}

// qsort comparator
int cmp(const void* a, const void* b) {
    char* str1 = (*(struct pair **)a)->key;
    char* str2 = (*(struct pair **)b)->key;
    return strcmp(str1, str2);
}

char* get_next(char* key, int partition_number) {
    if (partitions[partition_number]->iterator == partitions[partition_number]->num_pairs) {
	return NULL;
    }
    struct pair *tmp = partitions[partition_number]->pairs[partitions[partition_number]->iterator];
    if (!strcmp(tmp->key, key)) {
	partitions[partition_number]->iterator++;
	return tmp->value;
    }
    return NULL;
}
// reducer threads function
void *reduce_loop() {
    int partitionid;
    while(1) {
        pthread_mutex_lock(&reducer_lock);
        if (partition_counter==num_partition) {// all partitions are reduced
            pthread_mutex_unlock(&reducer_lock); 
            return NULL;
        }
        partitionid=partition_counter;
        partition_counter++;
        
        pthread_mutex_unlock(&reducer_lock); 



        /*since everthing in the partition is sorted and grouped iterating over it will with any key will get
        * all occurence of that key.
        *
        *when a key is done iterating over, as get_next returns NULL, we just conveniently go to next
        *key, which has to a different key because partition is sorted
        *
        * simply iterating and counting will parse and count all keys
        */

        while (partitions[partitionid]->iterator < partitions[partitionid]->num_pairs) {
	        reducer(partitions[partitionid]->pairs[partitions[partitionid]->iterator]->key, get_next, partitionid);
        }
    }

    

}

// default hash partitioner
unsigned long MR_DefaultHashPartition(char *key, int num_partitions) {
    unsigned long hash = 5381;
    int c;
    while ((c = *key++) != '\0')
        hash = hash * 33 + c;
    return hash % num_partitions;
}

// store processed key value pairs into partition assigned by partitioner
void MR_Emit(char *key, char *value){
    //printf("emit() called\n");
    struct pair *tmp= (struct pair *)malloc(sizeof(struct pair));
    int partitionid=partitioner(strdup(key),num_partition);
    
    tmp->key=strdup(key);
    tmp->value=strdup(value);
    
    
    pthread_mutex_lock(&partitions[partitionid]->lock);

    int num_pairs=partitions[partitionid]->num_pairs;

    partitions[partitionid]->pairs=(struct pair **)realloc(partitions[partitionid]->pairs,(num_pairs+1)*sizeof(struct pair*));

    partitions[partitionid]->pairs[num_pairs]=tmp;
    partitions[partitionid]->num_pairs++;
    pthread_mutex_unlock(&partitions[partitionid]->lock);
    

}

// mapper thread function
void *map_loop() {
    char *tmp=NULL;
    //mapper();
    while(1) {
        pthread_mutex_lock(&mapper_lock);
        
        while(map_queue.count==0) {// mesa semantics!!!!!
            if (file_counter==map_queue.size) {// at least one thread is not asleep while mapping the last file
                pthread_cond_broadcast(&mapper_cond);
                pthread_mutex_unlock(&mapper_lock); 
                return NULL;

            }
            pthread_cond_wait(&mapper_cond,&mapper_lock);
        }
 
        pop(&map_queue,&tmp);
        file_counter++;
        pthread_mutex_unlock(&mapper_lock); 


        
        mapper(tmp);

        //printf("arg is: %s\n",tmp);

    }
    


    
}

void mutex_initializer() {
    if (pthread_mutex_init(&mapper_lock,NULL) != 0) { 
        printf("mutex init has failed\n"); 
        exit(1); 
    } 

    if (pthread_mutex_init(&reducer_lock,NULL) != 0) { 
        printf("mutex init has failed\n"); 
        exit(1); 
    }

    if (pthread_cond_init(&mapper_cond,NULL) != 0) { 
        printf("mutex init has failed\n"); 
        exit(1); 
    }
    for (int i = 0; i < num_partition; i++) {

        pthread_mutex_init(&partitions[i]->lock, NULL);
    } 

}

void MR_Run(int argc, char *argv[], 
	    Mapper map, int num_mappers, 
	    Reducer reduce, int num_reducers, 
	    Partitioner partition, int num_partitions) {

    

    partitioner = partition;
    mapper = map;
    reducer = reduce;

    num_partition=num_partitions;
    partitions=(struct partition_info **)malloc(num_partition*sizeof(struct partition_info*));

    for (int i=0; i< num_partition; i++) {
        partitions[i]=(struct partition_info*)malloc(sizeof(struct partition_info));
        partitions[i]->pairs=(struct pair **)malloc(sizeof(struct pair*));
        partitions[i]->num_pairs=0;
        partitions[i]->iterator=0;

    }
    
    
    map_queue.store = malloc( sizeof (char *) * argc );
    if ( map_queue.store ){
        map_queue.size = argc-1;
        map_queue.count = map_queue.head = map_queue.tail = 0;
    }

    

    mutex_initializer();


    // mapper 

    pthread_t map_threads[num_mappers];
    for (int i = 0; i < num_mappers; i++) {
        
        pthread_create(&map_threads[i], NULL, map_loop, NULL);
        
    }


    for(int i=1;i<argc;i++) {

        char *tmp = argv[i];

        pthread_mutex_lock(&mapper_lock);
        if(push(&map_queue,tmp)==0){
            printf("cannot add file\n");
            return ;
        }
        pthread_cond_signal(&mapper_cond);
        pthread_mutex_unlock(&mapper_lock);

    }

    for (int i = 0; i < num_mappers; i++) {
        
        pthread_join(map_threads[i], NULL);
    }
    

    //






    for (int i = 0; i < num_partition; i++) {
        //printf("partition num is: %d, there are %d num pairs\n",i,(int)partitions[i]->num_pairs);
        for (int j = 0; j < partitions[i]->num_pairs; j++) {
            //printf("%s\n",partitions[i]->pairs[j]->key);
        
        }
        //printf("\n");
        
    }

    // sort
    for (int i = 0; i < num_partition; i++) {
        qsort(partitions[i]->pairs, partitions[i]->num_pairs, sizeof(struct pair*), cmp);
        
    }

    //reducer

    pthread_t reduce_threads[num_reducers];
    for (int i = 0; i < num_reducers; i++) {
        
        pthread_create(&reduce_threads[i], NULL, reduce_loop, NULL);
        
    }

    for (int i = 0; i < num_reducers; i++) {
        
        pthread_join(reduce_threads[i], NULL);
    }

    
    



            
    }

