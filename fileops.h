/* fileops.h
 
   functions for reading (writing handled elsewhere) datafiles
   NB: uses globals File_data, File_size, File_idx, and File_path
*/

#ifndef File_fileops_h
#define File_fileops_h

#include <string.h>

uint8_t *File_data = 0;
uint16_t File_size = 0;
uint8_t  File_idx = 0;
char*    File_path = 0;

void File_init()
{
  File_idx = 1;
}

bool File_reloadFromPath()
{
  if( 0 == File_path ) return false;    // abort?

  if( File_data ) {                     // reload?
    free( File_data );
    File_size = 0;
    File_data = 0;
  }

  FILE *f = fopen(File_path, "rb");
  if( 0 == f ) return false;            // abort?

  fseek(f, 0, SEEK_END);
  File_size = ftell( f );
  File_data = (uint8_t*)calloc( File_size, 1 );

  fseek(f, 0, SEEK_SET);
  fread(File_data, 1, File_size, f);

  fclose( f );
  
  return true;
}

/** set File_path to template and load */
bool File_loadFromTemplateG()
{
  if( File_path ) free( File_path );    // free?
  File_path = malloc( 4096 );           // alloc
  if( 0 == File_path ) return false;    // abort?
  memset(File_path, 0, 4096);           // clear
  snprintf(File_path, 4096, "./TEMPLATE-G.SPT");

  return File_reloadFromPath();
}

/** calculate File_path from File_index */
void File_pathFromIdx()
{
  if( File_path ) free( File_path );    // free?
  File_path = malloc( 4096 );           // alloc
  if( 0 == File_path ) return;          // abort?
  memset(File_path, 0, 4096);           // clear
  snprintf(File_path, 4096, "./PATT%02d.SPT", File_idx);
}

/** calculate File_path from File_index and load */
bool File_loadFromIdx()
{
  File_pathFromIdx();
  return File_reloadFromPath();
}

#endif
