/*
Array and Sorting:
Write a program that will read and sort the given array. Odd numbers first in accending order, followed by even numbers in decending order. Cannot use library functions, but any algorithm is acceptable. 
 */

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv){

  /* Test if number of arguments appropriate; in this case we need 2, the program name and the file to read */
  if (argc != 2) {
    printf("Unexpected number of arguments(%d)\n", argc);
    return 0;
  }

  /* Attempt to open file to read; argv[1] accepts the first argument */
  FILE* fp = fopen(argv[1], "r");
  if (fp == NULL) {
    return 0;
  }

  /* Define the first variables needed */
  int size; 
  int fInt;
  int i;
  int r = 0;
 
  /* Set up malloc using the size of the array; file pointer now points to first element of the array, not the size */
  fscanf(fp, "%d\n", &size);
  int * original = (int*)malloc(size* sizeof(int));

  /* Populate array; in this we store the file input with fInt, then use that to put integer into array; r is to check if there was an array made */
  for(i=0; fscanf(fp,"%d\t",&fInt) != EOF; i++){
      original[i] = fInt;
      r++;
  }

  /* if it is the end of file, r will be 0; so nothing was in array(size was still in the file, though), which is an error; close file either way, since the information is now stored */
  if (r == 0){
    printf("The file only has one input: the size of the array; there is no array.");
    fclose(fp);
    exit(0);
   }
  else{
    fclose(fp);
  }

  /* Sort it; check from left side if a number is even; switch it with the first odd number from the right side; 1st loop checks left side entirely, 2nd loop compares the right up to where the left side pointer is; checking the back end to the front end pointer allows for functionallity if even or odd number of elements  */
  int a;
  int b;
  for (a = 0; a < size; a++){
    if(original[a]%2 == 0){
      for (b = (size - 1); b > a; b--){
	if(original[b]%2 !=0){
	  int temp = original[a];
	    original[a] = original[b];
	    original[b] = temp;
	    break;
	  }
      }
    }
  }

  int o = 0;
  int x;
  int y;

  while (original[o]%2 != 0){
    o++;
  }
  
  /* sorts the odd numbers first, accending order */
  for( x = 0; x < o; x++){
    for( y = 0; y <=(o-1); y++){
      if(original[x] < original[y]){
	int temp = original[x];
	original[x] = original[y];
	original[y] = temp;
      }
    }
  }

  /* sorts the even numbers second, deccending order */
  for( x = o; x < size; x++){
    for( y = o; y < size; y++){
      if(original[x] > original[y]){
	int temp = original[x];
	original[x] = original[y];
	original[y] = temp;
      }
    }
  }

  int t;
  for(t = 0; t < size; t++){
    printf("%d\t", original[t]);
  }

  free(original);

  // Main function MUST return an integer, as defined above
  return 0;
}
