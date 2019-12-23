#include <stdlib.h>
#include <stdio.h>

//--------------Define the Node ----------------------
typedef struct Node{
  int val;
  struct Node* next;
} Node;

typedef Node* hashTable;
//---------------------------------
 /* CreateKey method; (value) mod (size) will create keys from 0 to (size - 1)
    Positive numbers will return the remainder i.e 9 mod 10k = 9 % 10000 = 9, 
    Negative numbers will require looking at how c calculates the remainder
    % is defined as (a/b)*b + a%b = a
    -1 mod 10000 = 9999, since following rules of mod, -1 = (-1 * 10000) + 9999
    % would calculate it as (-1/10000)*10000 + a%b = -1; 0 * 10000 + a%b = -1
    a%b would be considered -1
    to turn -1%10000 into the actual 9999, we just have to add -1 to 10000
    if the number is more negative than -10000, just add 10000 until it's >=-10k
*/
int createKey(int x){
  int key;
  while(x < -10000){
    x = x + 10000;
    } 
  if (x < 0){
    key = 10000 + x;
  }
  else{
    key = x % 10000;
    } 
  return key;
 }

//------------------ insertNode function -------------------
int insertNode(hashTable* table, Node* temp){
  int key;
  key = createKey(temp->val);
  Node* ptr = (table[key]);
  if (ptr == NULL){
    table[key] = temp;
    return 1;
  }
  else{
    while(ptr != NULL){
      if(ptr->val == temp->val){
	return 0;
      }
      ptr = ptr->next;
    }
    temp->next = table[key];
    table[key] = temp;
    return 1;
  }
}

//------------------ searchNode function ------------------------
int searchNode(hashTable* table, int value){
  int key;
  key = createKey(value);
  int found = 0;
  if(table[key] == NULL){
    return found;
  }
  else{
    Node* ptr = table[key];
    while(ptr != NULL){
      if(ptr->val == value){
	found = 1;
	return found;
      }
      ptr = ptr->next;
    }
    found = 0;
    return found;
  }
}

//----------- MAIN -------------------------------------

int main(int argc, char** argv){
  //-------------- Try opening and reading the file ---------------------
  if(argc != 2){
    return 0;
  }
  
  FILE* fp = fopen(argv[1], "r");
  if (fp == NULL){
    return 0;
  }

  // --------- Set an array of Node arrays with 10k buckets ------------------
  Node** table = (Node**)malloc(sizeof(Node*) * 10000); 
  int i;
  for (i = 0; i < 10000; i++){
   table[i] = NULL; 
  }                                   // each bucket in the array is set to NULL

  //---------------- Read the entire file ----------------
  char ch;
  int val;
  while(fscanf(fp, "%c\t%d\n", &ch, &val) != EOF){
      if (ch == 'i'){
	Node* temp = (Node*)malloc(sizeof(Node));
	  temp->val = val;
	  temp->next = NULL;
	  int inserted = insertNode(table, temp);
	  if (inserted == 1){
	    printf("inserted\n");
	  }
	  else{
	    printf("duplicate\n");
	  }
      }
      else if (ch == 's'){
	int found = searchNode(table, val);
	if (found == 1){
	  printf("present\n");
	}
	else{
	  printf("absent\n");
	}
      }
      else continue;
    }

  //--------------- here we close and free everything ------------
  fclose(fp);
  for(i = 0; i < 10000; i++){
    Node* ptr = table[i];
    while(ptr != NULL){
      Node* temp = ptr;
      ptr = ptr->next;
      free(temp);
    }
  }
  free(table);
  return 0;
}
