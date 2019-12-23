#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Global loop variables */
int i, j, count;

/* Input variables */
int cacheSize, blockSize, totalSets, associativity, LRU;
unsigned long long int input, tag, setIndex;

/* Struct declaration & definition for the cache, set, and line */
typedef struct cache_line{
  unsigned long long int tag;
  struct cache_line* next;
  unsigned int valid;
  unsigned int age;
} cache_line;
typedef cache_line* line;
typedef cache_line** cache;

/* Struct declaration for the hit/miss counters */
typedef struct counter{
  int mReads;
  int mWrites;
  int cHits;
  int cMisses;
}counter;

/* Declare methods */
void initializeCache(cache L, int numsets);
void freeCache(cache L, int numsets);
void cacheRead(cache L, int setindex, unsigned long long int t, int assoc, counter* c, int lru);
void cacheWrite(cache L, int setindex, unsigned long long int t, int assoc, counter* c, int lru);
int searchSet(cache L, int setindex, unsigned long long int t);
void insertFIFO(cache L, int setindex, unsigned long long int t, int assoc);
void insertLRU(cache L, int setindex, unsigned long long int t, int assoc);
int bitCountOffset(int blocksize);
int bitCountSet(int setcount);
unsigned long long int getSetIndex(unsigned long long int address, int setcount, int blocksize);
unsigned long long int getTag(unsigned long long int address, int setcount, int blocksize);

/* ============= Main Method ============== */
int main(int argc, char** argv){
 
  /* Test for 6 arguments */
  if(argc != 6){

    printf("error");
    return 0;

  }

  /* Accept and test parameter <cache size> for format and power of 2 */
  i = 0;
  while(argv[1][i] != '\0'){
    if(argv[1][i] > '9' || argv[1][i] < '0'){
      printf("error");
      return 0;
    }
    i++;
  }

  cacheSize = atoi(argv[1]);

  if(ceil(log2(cacheSize))!= floor(log2(cacheSize))){
      printf("error");
      return 0;
  }

  /* Accept and test parameter <block size> for format and power of 2 */
 i = 0;
  while(argv[2][i] != '\0'){
    if(argv[2][i] > '9' || argv[2][i] < '0'){
      printf("error");
      return 0;
    }
    i++;
  }
  
  blockSize = atoi(argv[2]);
  
  if(ceil(log2(blockSize))!= floor(log2(blockSize))){
      printf("error");
      return 0;
  }

  /* Accept and test parameter <cache policy>; answers are fifo or lru only */
  char** policy = &(argv[3]);

  if(strcmp(*policy, "fifo") == 0){
    LRU = 0;
  }
  else if(strcmp(*policy, "lru") == 0){
    LRU = 1;
  }
  else{
    printf("error");
    return 0;
  }

  /* Accept and test parameter <associativity>; direct, assoc, and assoc:n only; n must be power of 2 */
  char** association = &(argv[4]);

  if (strcmp(*association, "direct") == 0){
    associativity = 1;
    totalSets = cacheSize/blockSize;
  }
  else if(strcmp(*association, "assoc") == 0){
    associativity = cacheSize/blockSize;
    totalSets = 1;
  }
  else if (sscanf(argv[4],"assoc:%d", &i)){
    associativity = i;

    i = 6;
    while(argv[4][i] >= '0' && argv[4][i] <= '9'){
      i++;
    }
    if(i == 6){
      printf("error");
      return 0;
    }

    if(ceil(log2(associativity)) != floor(log2(associativity))){
      printf("error");
      return 0;
    }

    totalSets = cacheSize/(blockSize * associativity);
  }
  else{
      printf("error");
      return 0;
  }

  /* Accept, test, and create pointer to parameter <trace file> */
  FILE* fp = fopen(argv[5], "r");
  if (fp == NULL){
    printf("error6");
    return 0;
  }

  /* Make and initialize cache L1 */
  cache L1 = malloc(totalSets * sizeof(cache_line*));
  initializeCache(L1, totalSets);

  /* Counter Struct */
  counter* c = malloc(sizeof(counter));
  c->mReads = 0;
  c->mWrites = 0;
  c->cHits = 0;
  c->cMisses = 0;
 
  /* Accept inputs from the files */
  char rw;
  while(fscanf(fp, "%c %llx\n", &rw, &input) == 2){
    
    setIndex = getSetIndex(input, totalSets, blockSize);
    tag = getTag(input, totalSets, blockSize);

    if(rw == 'R'|| rw == 'r'){
      cacheRead(L1, setIndex, tag, associativity, c, LRU);
    } 
    else if(rw == 'W' || rw == 'w'){
      cacheWrite( L1, setIndex, tag, associativity, c, LRU);
    }
    else if(rw == '#'){
      break;
    }
    else{
      printf("error");
      break;
    }
  }

  /* Count printer */
  printf("Memory reads: %d\nMemory writes: %d\nCache hits: %d\nCache misses: %d\n", c->mReads, c->mWrites, c->cHits, c->cMisses);

  /* Free memory */
  freeCache(L1, totalSets);
  free(c);
  fclose(fp);
  return 0;
}

/* ============================== Methods ============================== */

void initializeCache(cache L, int numsets){
  int x;
  for(x = 0; x < numsets; x++){
    L[x] = NULL;
  }
}

void freeCache(cache L, int numsets){
  int x; 
  for(x = 0; x < numsets; x++){
    line temp;
    line head = L[x];
    while(head != NULL){
      temp = head;
      head = head->next;
      free(temp);
    }
  }
  free(L);
}

void cacheRead(cache L, int setindex, unsigned long long int t, int assoc, counter* c, int lru){
  if(searchSet(L,setindex,t)){
    c->cHits = c->cHits + 1;
    }
  else{
    c->cMisses = c->cMisses + 1;
    c->mReads = c->mReads + 1;
    /* if(lru){
      insertLRU(L, setindex, t, assoc);
    }
    else{
      insertFIFO(L, setindex, t, assoc);
      }*/
    insertFIFO(L, setindex, t, assoc);
  }
}

void cacheWrite(cache L, int setindex, unsigned long long int t, int assoc, counter* c, int lru){
  if(searchSet(L,setindex,t)){
    c->cHits = c->cHits + 1;
    c->mWrites = c->mWrites + 1;
  }
  else{
    c->cMisses = c->cMisses + 1;
    c->mReads = c->mReads + 1;
    c->mWrites = c->mWrites + 1;
    /*  if(lru){
      insertLRU(L, setindex, t, assoc);
    }
    else{
      insertFIFO(L, setindex, t, assoc);
    }
    */
    insertFIFO(L, setindex, t, assoc);
  }
}

int searchSet(cache L, int setindex, unsigned long long int t){
  line temp = L[setindex];
  while(temp != NULL){
    if(temp->tag == t){
       return 1;
    }
    else{
      temp = temp->next;
    }
  }
  return 0;
}


void insertFIFO(cache L, int setindex, unsigned long long int t, int assoc){
  line temp = L[setindex];
  line new = (line)malloc(sizeof(cache_line));
  int x;
  new->valid = 1;
  new->tag = t;
  new->age = 0;
  new->next = NULL;
  if(L[setindex] == NULL){
    L[setindex] = new;
  }
  else{
    x = 1;
    while(temp->next != NULL){
      temp = temp->next;
      x++;
    }
    temp->next = new;
    if(x == assoc){
      line head = L[setindex];
      L[setindex] = head->next;
      free(head);
    }
  }
}

void insertLRU(cache L, int setindex, unsigned long long int t, int assoc){
  line temp = L[setindex];
  line new = (line)malloc(sizeof(cache_line));
  int x,y;
  int comp = 0;
  new->valid = 1;
  new->tag = t;
  new->age = 0;
  new->next = NULL;
  if(L[setindex] == NULL){
    L[setindex] = new;
  }
  else{
    x = 1;y = 1;
    while(temp->next != NULL){
      x++;
      if(temp->age > comp){
	comp = temp->age;
	y = x;
      }
      else{
	temp = temp->next;
      }
    }
    if(x < assoc){
      temp->next = new;
    }
    else{
      temp = L[setindex];
      for(x = 0; x < y; x++){
	temp = temp->next;
      }
      line p = temp->next;
      new->next = p;
      temp->next = new;
      free(temp);
      line temp = L[setindex];
      while(temp != NULL){
	temp->age = temp->age + 1;
      }
    }
  }
}
/* Calculate the bit count for the offset, set, and tag */
int bitCountOffset(int blocksize){
  int ret = (int)round(log2(blocksize));
  return ret;
}
int bitCountSet(int setcount){
  int ret = (int)round(log2(setcount));
  return ret;
}

/* Calculate set index */
unsigned long long int getSetIndex(unsigned long long int address, int setcount, int blocksize){
  int setbits = bitCountSet(setcount);
  int offbits = bitCountOffset(blocksize);
  int tagbits = 48 - setbits - offbits;
  offbits = offbits + tagbits;
  unsigned long long int ret = (address<<tagbits) & 0xFFFFFFFFFFFF;
  ret = (ret>>offbits) & 0xFFFFFFFFFFFF;
  return ret;
}

/* Calculate tag */
unsigned long long int getTag(unsigned long long int address, int setcount, int blocksize){
  int setbits = bitCountSet(setcount);
  int offbits = bitCountOffset(blocksize);
  offbits= setbits + offbits ;
  unsigned long long int ret = (address>>offbits) & 0xFFFFFFFFFFFF ;
  return ret;
}
