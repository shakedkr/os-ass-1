#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  int n;
  printf (1,"%d" ,argc);
  if (argc!=2){
      printf(1,"error with number of args \n");
      exit();
  }
  
  n = my_atoi(argv[1]);
  if (n==-1){
      printf(1, "error with number \n");
  }
      
  exit();
}
