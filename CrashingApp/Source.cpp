#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <string.h>

int main(void) 
{ 
  char* pp = 0; 
  char* tpp = 0; 
  strncpy(pp, tpp, 5000); 
  return 0; 
}