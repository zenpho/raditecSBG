/* main.c

   Radikal Technologies Spectralis editor
   raditecSBG by zenpho@zenpho.co.uk

   I am not the author of the SPT fileformat. I am writing
   this to decode and encode pattern data. I want to move
   data between the Groove and Line Sequencers and to
   import and export Standard Midi Format (SMF) files.
*/
#include "ui.h" // includes uiSPT_... headers, requires ncurses

int main(int argc, char **argv)
{
  struct ui_CFG cfg;
  
  ui_init( &cfg );
  ui_mainloop( &cfg ); // exits after 'quit' command
  ui_teardown( &cfg );

  return (0);
}
