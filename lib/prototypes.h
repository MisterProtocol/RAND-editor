/*
 * prototypes of functions in the "lib" directory
 */

#include <stdio.h> /* needed for FILE definition */

long getlong (FILE *);
short getshort (FILE *);
int intss (void);
void putlong (long, FILE *);
void putshort (int, FILE *);
void delay (int);
#ifdef FIONREAD
int empty (int);
#endif /* FIONREAD */
char *my_move (char*, char*, unsigned long);
#ifdef HOSTTMPNAMES
char *mygethostname (void);
#endif /* HOSTTMPNAMES */
void fatal (void);
