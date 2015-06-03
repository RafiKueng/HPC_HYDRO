/*
  A simple 2D hydro code
  (C) Romain Teyssier : CEA/IRFU           -- original F90 code
  (C) Pierre-Francois Lavallee : IDRIS      -- original F90 code
  (C) Guillaume Colin de Verdiere : CEA/DAM -- for the C version
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>

#include "debug.h"

#include "parametres.h"
#include "utils.h"
#include "vtkfile.h"



/**
 * @brief writes a snapshot / step to a vtk file
 * 
 * @param step ...
 * @param H ...
 * @param Hv ...
 * @return void
 */
void vtkfile ( long step, const hydroparam_t H, hydrovar_t * Hv ) {

    char name[160];
    FILE *fic;
    long i, j, nv;

    LOC ( H.rank );
    WHERE ( "vtkfile" );

    // Files are named outputvtk_rank_step.vts
    sprintf ( name, "outputvtk_%04i_%05ld.vts", H.rank, step );

    fic = fopen ( name, "w" );
    if ( fic == NULL ) {
        ERR ( "Cannot open file %s\n", name );
        exit ( 1 );
    }
    fprintf ( fic, "<?xml version=\"1.0\"?>\n" );
    fprintf ( fic, "<VTKFile type=\"StructuredGrid\">\n" );
    fprintf ( fic, "<StructuredGrid WholeExtent=\" %ld %ld %ld %ld %ld %ld\">\n", ( long ) 0,
              H.nx, ( long ) 0, H.ny, ( long ) 0, ( long ) 0 );
    fprintf ( fic, "<Piece Extent=\" %ld %ld %ld %ld %ld %ld\">\n", ( long ) 0, H.nx, ( long ) 0, H.ny, ( long ) 0, ( long ) 0 );
    fprintf ( fic, "<Points>\n" );
    fprintf ( fic,
              "<DataArray type=\"Float32\" format=\"ascii\" NumberOfComponents=\"3\">\n" );
    for ( j = 0; j < H.ny + 1; j++ ) {
        for ( i = 0; i < H.nx + 1; i++ ) {
            fprintf ( fic, "%f %f %f\n", i * H.dx, j * H.dx, 0.0 );
        }
    }
    fprintf ( fic, "</DataArray>\n" );
    fprintf ( fic, "</Points>\n" );
    name[0] = 0;
    for ( nv = 0; nv < IP; nv++ ) {
        if ( nv == ID )
            sprintf ( name, "%s varID", name );
        if ( nv == IU )
            sprintf ( name, "%s varIU", name );
        if ( nv == IV )
            sprintf ( name, "%s varIV", name );
        if ( nv == IP )
            sprintf ( name, "%s varIP", name );
    }

    // declaration of the variable list
    fprintf ( fic, "<CellData Scalars=\"%s\">\n", name );
    name[0] = 0;
    for ( nv = 0; nv <= IP; nv++ ) {
        if ( nv == ID )
            sprintf ( name, "varID" );
        if ( nv == IU )
            sprintf ( name, "varIU" );
        if ( nv == IV )
            sprintf ( name, "varIV" );
        if ( nv == IP )
            sprintf ( name, "varIP" );

        //Definition of the cell values
        fprintf ( fic, "<DataArray type=\"Float32\" Name=\"%s\" format=\"ascii\">\n", name );

        // the image is the interior of the computed domain
        for ( j = H.jmin + ExtraLayer; j < H.jmax - ExtraLayer; j++ ) {
            for ( i = H.imin + ExtraLayer; i < H.imax - ExtraLayer; i++ ) {
                fprintf ( fic, "%lf ", Hv->uold[IHv ( i, j, nv )] );
            }
            fprintf ( fic, "\n" );
        }
        fprintf ( fic, "</DataArray>\n" );
    }
    fprintf ( fic, "</CellData>\n" );
    fprintf ( fic, "</Piece>\n" );
    fprintf ( fic, "</StructuredGrid>\n" );
    fprintf ( fic, "</VTKFile>\n" );
    fclose ( fic );
}



void timingfile_init ( hydroparam_t* H ) {
    
    LOC ( H->rank );

    char name[160];

    sprintf ( name, "timing_%04i.cvs", H->rank );

    H->timing_file = fopen ( name, "w" );

    
}



inline void timingfile_write ( long step, double time, const hydroparam_t H) {

    fprintf ( H.timing_file, "%ld, %e\n", step, time );

}


void timingfile_finish ( hydroparam_t* H ) {
    fclose ( H->timing_file );
}

void write_stat ( double elapsed, long nsteps, long nstates, const hydroparam_t H ) {
    
    LOC ( H.rank );

    char name[160];
    FILE* fic;

    sprintf ( name, "stats.txt" );

    fic = fopen ( name, "w" );
    
    fprintf ( fic, "stats\n" );
    fprintf ( fic, "nsteps: %ld\n", nsteps );
    fprintf ( fic, "nstates_written: %ld\n", nstates );
    fprintf ( fic, "wall_time: %e\n", elapsed );
    
    fclose ( fic );
}



