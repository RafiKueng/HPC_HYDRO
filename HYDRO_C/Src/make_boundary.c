/*
  A simple 2D hydro code
  (C) Romain Teyssier : CEA/IRFU           -- original F90 code
  (C) Pierre-Francois Lavallee : IDRIS      -- original F90 code
  (C) Guillaume Colin de Verdiere : CEA/DAM -- for the C version
*/

#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>

#include "parametres.h"
#include "make_boundary.h"
#include "utils.h"
void
make_boundary(long idim, const hydroparam_t H, hydrovar_t * Hv)
{

    // - - - - - - - - - - - - - - - - - - -
    // Cette portion de code est � v�rifier
    // d�tail. J'ai des doutes sur la conversion
    // des index depuis fortran.
    // - - - - - - - - - - - - - - - - - - -
    long i, ivar, i0, j, j0;
    double sign;
    WHERE("make_boundary");

 
    if (idim == 1) {

        // Left boundary
        for (ivar = 0; ivar < H.nvar; ivar++) {
            for (i = 0; i < ExtraLayer; i++) {
                sign = 1.0;
                if (H.boundary_left == 1) {
                    i0 = ExtraLayerTot - i - 1;
                    if (ivar == IU) {
                        sign = -1.0;
                    }
                } else if (H.boundary_left == 2) {
                    i0 = 2;
                } else {
                    i0 = H.nx + i;
                }
                for (j = H.jmin + ExtraLayer; j < H.jmax - ExtraLayer; j++) {
                    Hv->uold[IHv(i, j, ivar)] = Hv->uold[IHv(i0, j, ivar)] * sign;
                    MFLOPS(1, 0, 0, 0);
                }
            }
        }

	/* fprintf(stderr,"PFL H.nvar %d H.nx %d\n",H.nvar,H.nx);
	fprintf(stderr,"PFL ExtraLayer %d ExtraLayerTot %d\n",ExtraLayer,ExtraLayerTot);
	fprintf(stderr,"PFL H.jmin %d H.jmax %d\n",H.jmin,H.jmax); */

        // Right boundary
        for (ivar = 0; ivar < H.nvar; ivar++) {
            for (i = H.nx + ExtraLayer; i < H.nx + ExtraLayerTot; i++) {
                sign = 1.0;
                if (H.boundary_right == 1) {
                    i0 = 2 * H.nx + ExtraLayerTot - i - 1;
                    if (ivar == IU) {
                        sign = -1.0;
                    }
                } else if (H.boundary_right == 2) {
                    i0 = H.nx + ExtraLayer;
                } else {
                    i0 = i - H.nx;
                }
                for (j = H.jmin + ExtraLayer; j < H.jmax - ExtraLayer; j++) {
		  /* fprintf(stderr,"PFL %d %d\n",i,j); */ 
                    Hv->uold[IHv(i, j, ivar)] = Hv->uold[IHv(i0, j, ivar)] * sign;
		    /*		  fprintf(stderr,"PFL \n"); */

                    MFLOPS(1, 0, 0, 0);
                }
            }
        }
    } else {

        // Lower boundary
        j0 = 0;
        for (ivar = 0; ivar < H.nvar; ivar++) {
            for (j = 0; j < ExtraLayer; j++) {
                sign = 1.0;
                if (H.boundary_down == 1) {
                    j0 = ExtraLayerTot - j - 1;
                    if (ivar == IV) {
                        sign = -1.0;
                    }
                } else if (H.boundary_down == 2) {
                    j0 = ExtraLayerTot;
                } else {
                    j0 = H.ny + j;
                }
                for (i = H.imin + ExtraLayer; i < H.imax - ExtraLayer; i++) {
                    Hv->uold[IHv(i, j, ivar)] = Hv->uold[IHv(i, j0, ivar)] * sign;
                    MFLOPS(1, 0, 0, 0);
                }
            }
        }

        // Upper boundary
        for (ivar = 0; ivar < H.nvar; ivar++) {
            for (j = H.ny + ExtraLayer; j < H.ny + ExtraLayerTot; j++) {
                sign = 1.0;
                if (H.boundary_up == 1) {
                    j0 = 2 * H.ny + ExtraLayerTot - j - 1;
                    if (ivar == IV) {
                        sign = -1.0;
                    }
                } else if (H.boundary_up == 2) {
                    j0 = H.ny + 1;
                } else {
                    j0 = j - H.ny;
                }
                for (i = H.imin + ExtraLayer; i < H.imax - ExtraLayer; i++) {
                    Hv->uold[IHv(i, j, ivar)] = Hv->uold[IHv(i, j0, ivar)] * sign;
                    MFLOPS(1, 0, 0, 0);
                }
            }
        }
    }
}                               // make_boundary

/*
** Exchange the boundary conditions with neighboring domains that are on
** different processes. We use MPI_IRecv() and MPI_ISend() to exchange the
** data. This means we have to make sure that all data was transvered before
** we can do the computation for the most outer layer of the grid using
** MPI_get_boundary_end().
*/

void
MPI_get_boundary_start(long idim, const hydroparam_t H, hydrovar_t * Hv, MPI_Request *MPI_req)
{
    long i, ivar, i0, j, j0, k;
    double sign;
    WHERE("MPI_get_boundary_start");
		
	/*
	** (CR) We communicate each cell of the array separately. This should be improved at some point!
	*/
//	MPI_Irecv(values, count, type, source, tag, comm, req)
//	H.iMPIError = MPI_Irecv(Hv, 1, columntype, H.iProc-1, tag, MPI_COMM_WORLD,req);
//	MPI_Isend(values, count, datatype, dest, tag, comm, req)
//	H.iMPIError = MPI_Isend(Hv, 1, columntype, H.iProc-1, tag, MPI_COMM_WORLD,MPI_Request *request);
 
	if (idim == 1) {
		// Make sure MPI_req is allocated
		assert(MPI_req != NULL);

		/* Get values from the left domain. */
		if (H.iProc == 0)
		{
			// Set physical boundary conditions
			for (ivar = 0; ivar < H.nvar; ivar++) {
				for (i = 0; i < ExtraLayer; i++) {
					sign = 1.0;
	                if (H.boundary_left == 1) {
						i0 = ExtraLayerTot - i - 1;
	       	            if (ivar == IU) {
    						sign = -1.0;
       					}
					} else if (H.boundary_left == 2) {
						i0 = 2;
					} else {
						i0 = H.nx + i;
					}
                	for (j = H.jmin + ExtraLayer; j < H.jmax - ExtraLayer; j++) {
                    	Hv->uold[IHv(i, j, ivar)] = Hv->uold[IHv(i0, j, ivar)] * sign;
						MFLOPS(1, 0, 0, 0);
                	}
            	}
        	}
		} else 	{
			// Exchange MPI boundary conditions
			MPI_Irecv( &Hv->uold[IHv(0, 0, ID)], 1, H.MPI_Hydro_vars, H.iProc-1, 0, MPI_COMM_WORLD, MPI_req );
			MPI_Irecv( &Hv->uold[IHv(1, 0, ID)], 1, H.MPI_Hydro_vars, H.iProc-1, 0, MPI_COMM_WORLD, MPI_req+1 );

			MPI_Isend( &Hv->uold[IHv(2, 0, ID)], 1, H.MPI_Hydro_vars, H.iProc-1, 0, MPI_COMM_WORLD, MPI_req+2 );
			MPI_Isend( &Hv->uold[IHv(3, 0, ID)], 1, H.MPI_Hydro_vars, H.iProc-1, 0, MPI_COMM_WORLD, MPI_req+3 );
		}

		/* Get values from the right domain. */
		if (H.iProc < H.iNProc - 1)
		{
			// Set physical boundary conditions 
			for (ivar = 0; ivar < H.nvar; ivar++) {
				for (i = H.nx + ExtraLayer; i < H.nx + ExtraLayerTot; i++) {
					sign = 1.0;
					if (H.boundary_right == 1) {
						i0 = 2 * H.nx + ExtraLayerTot - i - 1;
						if (ivar == IU) {
							sign = -1.0;
						}
					} else if (H.boundary_right == 2) {
						i0 = H.nx + ExtraLayer;
					} else {
						i0 = i - H.nx;
					}
					for (j = H.jmin + ExtraLayer; j < H.jmax - ExtraLayer; j++) {
						/* fprintf(stderr,"PFL %d %d\n",i,j); */ 
						Hv->uold[IHv(i, j, ivar)] = Hv->uold[IHv(i0, j, ivar)] * sign;
						/* fprintf(stderr,"PFL \n"); */
						MFLOPS(1, 0, 0, 0);
					}
				}
			}
		} else {
			/* Dont do this for the most right domain. */
			MPI_Irecv( &Hv->uold[IHv(H.nx+ExtraLayer, 0, ID)], 1, H.MPI_Hydro_vars, H.iProc+1, 0, MPI_COMM_WORLD, MPI_req+4 );
			MPI_Irecv( &Hv->uold[IHv(H.nx+ExtraLayer+1, 0, ID)], 1, H.MPI_Hydro_vars, H.iProc+1, 0, MPI_COMM_WORLD, MPI_req+5 );

			MPI_Isend( &Hv->uold[IHv(H.nx+ExtraLayer-2, 0, ID)], 1, H.MPI_Hydro_vars, H.iProc+1, 0, MPI_COMM_WORLD, MPI_req+6 );
			MPI_Isend( &Hv->uold[IHv(H.nx+ExtraLayer-1, 0, ID)], 1, H.MPI_Hydro_vars, H.iProc+1, 0, MPI_COMM_WORLD, MPI_req+7 );
		}
    } else {
        // Lower boundary
        j0 = 0;
        for (ivar = 0; ivar < H.nvar; ivar++) {
            for (j = 0; j < ExtraLayer; j++) {
                sign = 1.0;
                if (H.boundary_down == 1) {
                    j0 = ExtraLayerTot - j - 1;
                    if (ivar == IV) {
                        sign = -1.0;
                    }
                } else if (H.boundary_down == 2) {
                    j0 = ExtraLayerTot;
                } else {
                    j0 = H.ny + j;
                }
                for (i = H.imin + ExtraLayer; i < H.imax - ExtraLayer; i++) {
                    Hv->uold[IHv(i, j, ivar)] = Hv->uold[IHv(i, j0, ivar)] * sign;
                    MFLOPS(1, 0, 0, 0);
                }
            }
        }

        // Upper boundary
        for (ivar = 0; ivar < H.nvar; ivar++) {
            for (j = H.ny + ExtraLayer; j < H.ny + ExtraLayerTot; j++) {
                sign = 1.0;
                if (H.boundary_up == 1) {
                    j0 = 2 * H.ny + ExtraLayerTot - j - 1;
                    if (ivar == IV) {
                        sign = -1.0;
                    }
                } else if (H.boundary_up == 2) {
                    j0 = H.ny + 1;
                } else {
                    j0 = j - H.ny;
                }
                for (i = H.imin + ExtraLayer; i < H.imax - ExtraLayer; i++) {
                    Hv->uold[IHv(i, j, ivar)] = Hv->uold[IHv(i, j0, ivar)] * sign;
                    MFLOPS(1, 0, 0, 0);
                }
            }
        }
    }
}                               // MPI_get_boundary_start

/*
** Make sure that all boundary cells have been successfully exchanged between
** the processes before we continue.
*/
void
MPI_get_boundary_end(long idim, const hydroparam_t H, hydrovar_t * Hv, MPI_Request *MPI_req)
{
	int count, offset, MPIError;
	MPI_Status *status;

	status = malloc(8*sizeof(status));
	offset = 0;

	if ( idim == 1) {
		// Dont do this if we sweep the grid in ny direction.
		if (H.iProc == 0 || H.iProc == H.iNProc-1)
		{
			// For the most outer domains we have less b.c. to exchange.
			count = 1;
			if ( H.iProc == 0 )
			{
				offset = 4;
			}
		} else {
			count = 2;
		}
		MPIError = MPI_Waitall(4*count, MPI_req+offset, status);
		assert( MPIError == 0 );
	}
}                               // MPI_get_boundary_end

void
MPI_make_boundary(long idim, const hydroparam_t H, hydrovar_t * Hv)
{
	/*
	** Exchange the boundary conditions with neighboring domains that are on
	** different processes. 
	*/
	// Allocate H->MPI_req !!!
	MPI_Request *MPI_req;
	MPI_req = malloc(8*sizeof(MPI_Request));

	// Initiate send and receive requests
	MPI_get_boundary_start(idim, H, Hv, MPI_req);

	// Make sure the data was successfully exchanged before we continue.
	MPI_get_boundary_end(idim, H, Hv, MPI_req);

	// (CR) Debug
	MPI_Barrier( MPI_COMM_WORLD );
	// Free MPI_req
	Free(MPI_req);
}                               // MPI_get_boundary

//EOF
