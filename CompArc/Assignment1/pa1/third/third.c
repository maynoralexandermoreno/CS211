#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//-----------Print Function --------------------
void printX(unsigned short* b){
  int i,j,x,c;
  c = 1; x = 0;
  for(i=0;i<16;i++){
    if(b[i] == 1){
      for(j = 0; j < i; j++){
	c = 2*c;
      }
      x = x + c;
      c = 1;
    }
  }
  printf("%d\n", x);
}

//----------Main Function -----------------------
int main(int argc, char** argv){
  if (argc != 2) {
    printf("Unexpected number of arguments(%d)\n", argc);
    return 0;
  }

  FILE* fp = fopen(argv[1], "r");
  if (fp == NULL) {
    return 0;
  }

  int x;
  fscanf(fp, "%d\n", &x);

//---------create and initialize array-----------------------
  unsigned short bits[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


//-----------populate array----------------
  int count;
  count = 0;
  while (x != 0){
    bits[count] = (unsigned short)x%2;
    x = x/2;
    count++;
  }

//-------proceed to recieve commands------------
  char f[5];
  char g[] = "get";
  char s[] = "set";
  char c[] = "comp";
  int n;
  int v;
  while((fscanf(fp,"%s\t%d\t%d\n",f,&n,&v)) != EOF){
    if(strcmp(f,g)== 0){
      printf("%d\n", bits[n]);
    }
    if(strcmp(f,c) == 0){
      if(bits[n] == v){
	  bits[n]++;
      }
      else{
	bits[n] = v;
      }
      printX(bits);
    }
    if(strcmp(f,s) == 0){
      bits[n] = v;
      printX(bits);
    }
  }

  return 0;
}
