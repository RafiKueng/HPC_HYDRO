#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include "parametres.h"

#ifndef Square
#define Square(x) ((x) * (x))
#endif /*  */

#ifndef MAX
#define MAX(x, y) ((x) > (y)? (x): (y))
#endif /*  */
#ifndef MIN
#define MIN(x, y) ((x) < (y)? (x): (y))
#endif /*  */


/* DEBUG FUNCTIONS */

#define DEBUG 1

// only print on rank0
#define dbg_print(...)           if (DEBUG && H->rank==0) { printf(__VA_ARGS__); }; MPI_Barrier(MPI_COMM_WORLD); usleep(100);
// rank to print on is first arg
#define dbg_rprint(rank, ...)    if (DEBUG && H->rank==rank) { printf(__VA_ARGS__); }; MPI_Barrier(MPI_COMM_WORLD); usleep(100);
// print on all, but in turns
#define dbg_sprint(...)          if (DEBUG) {                                        \
                                    int i_i=0;                                          \
                                    for ( i_i = 0; i_i < H->n_procs; ++i_i) {             \
                                        MPI_Barrier ( MPI_COMM_WORLD );                    \
                                        if ( i_i == H->rank ) {                         \
                                            printf (__VA_ARGS__);                                  \
                                }};  MPI_Barrier ( MPI_COMM_WORLD ); usleep(100); }




#ifndef Free
// Make sure that the pointer is unusable afterwards.
#define Free(x) do { if ((x)) { free((x)); }; (x) = NULL; } while (0)
#endif /*  */
double **allocate(long imin, long imax, long nvar);
double *DMalloc(long n);
long *IMalloc(long n);

// 0 means perfect memory management from the code ;-)
#define MallocGuard 0
// static const long MallocGuard = 0;
void printuold(const hydroparam_t H, hydrovar_t * Hv);
void printarray(double *a, long n, const char *nom);
void printarrayi(long *a, long n, const char *nom);
void printarrayv(double *a, long n, const char *nom, const hydroparam_t H);
void timeToString(char *buf, const double timeInS);
double cclock(void);

#ifndef PRINTUOLD
#define PRINTUOLD(x, y) if ((x).prt) { printuold((x), (y)); }
#define PRINTUOLDPF(x, y) { printuold((x), (y)); }
#define PRINTARRAY(x, y, z, t) if ((t).prt) { printarray((x), (y), (z)); }
#define PRINTARRAYI(x, y, z, t) if ((t).prt) { printarrayi((x), (y), (z)); }
#define PRINTARRAYV(x, y, z, t) if ((t).prt) { printarrayv((x), (y), (z), (t)); }
#endif /*  */

#ifndef WHERE
#define WHERE(n)
#endif /*  */

#define RESTRICT __restrict

#endif // UTILS_H_INCLUDED
