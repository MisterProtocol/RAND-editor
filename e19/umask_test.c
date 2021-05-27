#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

mode_t getumask(void);

main()
{
  mode_t mask = read_umask();
  printf("umask=%o\n", mask);
}


/*mode_t*/
read_umask (void)
{
  mode_t mask = umask (0);
  umask (mask);
  return mask;
}
