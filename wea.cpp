/* srand example */
#include <stdio.h>      /* printf, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

int main ()
{
  printf ("First number: %d\n", rand()%10);
  srand (time(NULL));
  printf ("Random number: %d\n", rand()%100);
  srand (4412);
  printf ("Again the first number: %d\n", rand()%100);

  return 0;
}