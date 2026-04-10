/* uiSPT_xfer.h

   user interface for transfer from grooveseq to lineseq etc
*/
#ifndef uiSPT_xfer_h
#define uiSPT_xfer_h

#include "ui.h"
#include "uiSPT_common.h"
#include "SPT_parse.h"
#include "SPT_xfer.h"

void SPT_ui_xferG_wprint(WINDOW* w, uint8_t gIdx)
{
  char *srcNames[11] = {      // source names
    "kick", "snar", "clhh", "ophh", "clap", "htom", "mtom", "ltom",
    "prc1", "prc2", "prc3"
  };
  wprintw( w, "%s:", srcNames[gIdx] );
  
  // 16-step pattern text recomputed below
  char patternText[17] = "................"; // initially all dots
  
  if( IlGs[gIdx] )                          // definition is valid
  {
    uint8_t numgVs = IlGs[gIdx]->h.numBytesVData / sizeof(SPT_Il_vData);
    for(int gv=0; gv<numgVs; gv++)          // loop over the IlGVData[] entries
    {
      uint16_t time = IlGVData[gIdx][gv].b; // the timecode in 'G'roove vData field 'b'
      uint8_t step = time / 12;             // at 48ppqn, this represents 1/16th steps
      if( step > 15 ) continue;             // skip over any beyond range 0..15
    
      patternText[step] = '=';              // active steps represented with '=' character
    }
  }
  
  wprintw( w, "[%s] ", patternText );       // always display (perhaps only dots)
}

void SPT_ui_xferA_wprint(WINDOW* w, uint8_t aIdx)
{
  char *tgtNames[11] = {           // part names
    "kick", "snar", "clhh", "ophh", "clap", "htom", "mtom", "ltom",
    "prc1", "prc2", "prc3"
  };
  wprintw( w, "L%02d:", aIdx+1 );  // display target lineseq name
  
  // 16-step pattern text recomputed below
  char patternText[17] = "................"; // initially all dots
  
  if( IlAs[aIdx] )                   // lineseq definition is valid
  {
    uint8_t numaVs = IlAs[aIdx]->h.numBytesVData / sizeof(SPT_Il_vData);
    for(int av=0; av<numaVs; av++)   // loop over the IlAVData[] entries
    {
      if( av > 15 ) continue;        // skip over any beyond range 0..15
      if( (IlAVData[aIdx][av].a)&1 ) // trig state is the lsb of the vdata field 'a'
      {
        patternText[av] = 'x';       // active steps represented with 'x' character
      }
    }
  }
  
  wprintw( w, "[%s] ", patternText );            // always display (perhaps only dots)
  
  if( IlAs[aIdx] )                               // lineseq definition is valid
  {
    uint8_t PG0 = IlAs[aIdx]->targetPG[0];       // 0xa4 for 'dsyn'
    uint8_t PG1 = IlAs[aIdx]->targetPG[1];       // 0x28 for 'dsyn'
    uint8_t SD0 = IlAs[aIdx]->targetSD[0];       // 0,1,etc for kick,snar,etc
    uint8_t SD1 = IlAs[aIdx]->targetSD[1];       // always zero
    
    if( 0xa4==PG0 && 0x28==PG1 && 0x00==SD1 ) {  // matches 'dsyn' outlined above
      uint8_t dsynIdx = SD0;                     // targetSD[0] indicates part index #
      wprintw( w, "(%s)", tgtNames[dsynIdx] );   // which we display as named text
    }
    else { // otherwise display hex targetPG and targetSD bytes
      wprintw( w, "(%02x,%02x %02x,%02x)", PG0, PG1, SD0, SD1 );
    }
  }
}

void SPT_ui_xfer_wprint(struct ui_CFG* cfg, WINDOW* w)
{
  displayCwd(w);                      // calls wclear()
  wprintw( w, "NAME:%s\n", Tp.name ); // songname from SPT_Tp chunk
  
  for(int idx=0; idx<=10; idx++){     // display table of rhythm views
    SPT_ui_xferG_wprint( w, idx );
    SPT_ui_xferA_wprint( w, idx );
    wprintw( w, "\n" );
  }
  
  // display a reminder of focused groove and lineseq
  wprintw( w, "\n        (press a/s)           (press A/S)\n", cfg->gIdx, cfg->aIdx );
  SPT_ui_xferG_wprint( w, cfg->gIdx );
  SPT_ui_xferA_wprint( w, cfg->aIdx );
}

void SPT_ui_clearGroove(struct ui_CFG* cfg)
{
  char response = ' ';
  if( IlGs[cfg->gIdx] == 0 )
  {
    wclear( wScrollable );
    wprintw( wScrollable, "G%02d inactive - nothing changed\n", cfg->gIdx );
    wprintw( wScrollable, "Press any key to confirm\n" );
    response = wgetch( wScrollable );
    return;
  }
  
  wclear( wScrollable );
  wprintw( wScrollable, "GROOVE ERASE shown below?\n" );
  SPT_ui_xferG_wprint( wScrollable, cfg->gIdx );
  
  wprintw( wScrollable, "\n\n\nSURE? 'Y' to confirm (any other to abort)\n" );
  response = wgetch( wScrollable );
  if( 'y' != response && 'Y' != response ){
    wprintw( wScrollable, "ABORT OK - nothing changed" );
    return;
  }
  
  SPT_xfer_clearG( cfg->gIdx );      // apply changes
  SPT_overwriteFilePath();           // overwrite current file, saving changes
  wprintw( wScrollable, "DONE!\n" );

  if( !File_reloadFromPath() )       // we might display an error if this fails
  { mvwprintw(stdscr, 0, 45, "ERR %s\n", File_path ); }
}

void SPT_ui_clearLine(struct ui_CFG* cfg)
{
  char response = ' ';
  if( IlAs[cfg->aIdx] == 0 )
  {
    wclear( wScrollable );
    wprintw( wScrollable, "L%02d inactive - nothing changed\n", cfg->aIdx+1 );
    wprintw( wScrollable, "Press any key to confirm\n" );
    response = wgetch( wScrollable );
    return;
  }
  
  wclear( wScrollable );
  wprintw( wScrollable, "LINESEQ ERASE shown below?\n" );
  SPT_ui_xferA_wprint( wScrollable, cfg->aIdx );
  
  wprintw( wScrollable, "\n\n\nSURE? 'Y' to confirm (any other to abort)\n" );
  response = wgetch( wScrollable );
  if( 'y' != response && 'Y' != response ){
    wprintw( wScrollable, "ABORT OK - nothing changed" );
    return;
  }
  
  SPT_xfer_clearA( cfg->aIdx );        // apply changes
  SPT_overwriteFilePath();           // overwrite current file, saving changes
  wprintw( wScrollable, "DONE!\n" );
  
  if( !File_reloadFromPath() )       // we might display an error if this fails
  { mvwprintw(stdscr, 0, 45, "ERR %s\n", File_path ); }
}

/** SPT_ui_xfer_seqInternal - in which sequencers set to internal dsyn */
void SPT_ui_xfer_seqInternal(struct ui_CFG* cfg)
{
  char response = ' ';
  
  wclear( wScrollable );
  wprintw( wScrollable, "INTERNAL ENGINE for **ALL** drum groove and linseq?\n" );
  
  wprintw( wScrollable, "\n\n\nSURE? 'Y' to confirm (any other to abort)\n" );
  response = wgetch( wScrollable );
  if( 'y' != response && 'Y' != response ){
    wprintw( wScrollable, "ABORT OK - nothing changed" );
    return;
  }
  
  SPT_seqGA_internal(); // modify IlGs[] and IlAs[]
  SPT_overwriteFilePath();           // overwrite current file, saving changes
  wprintw( wScrollable, "DONE!\n" );
  
  if( !File_reloadFromPath() )       // we might display an error if this fails
  { mvwprintw(stdscr, 0, 45, "ERR %s\n", File_path ); }
}

/** SPT_ui_xfer_seqExtMIDI - in which sequencers set to external midi */
void SPT_ui_xfer_seqExtMIDI(struct ui_CFG* cfg)
{
  char response = ' ';
  
  wclear( wScrollable );
  wprintw( wScrollable, "EXTERNAL MIDI for **ALL** drum groove and linseq?\n" );
  
  wprintw( wScrollable, "\n\n\nSURE? 'Y' to confirm (any other to abort)\n" );
  response = wgetch( wScrollable );
  if( 'y' != response && 'Y' != response ){
    wprintw( wScrollable, "ABORT OK - nothing changed" );
    return;
  }
  
  SPT_seqGA_extMIDI(); // modify IlGs[] and IlAs[]
  SPT_overwriteFilePath();           // overwrite current file, saving changes
  wprintw( wScrollable, "DONE!\n" );
  
  if( !File_reloadFromPath() )       // we might display an error if this fails
  { mvwprintw(stdscr, 0, 45, "ERR %s\n", File_path ); }
}

/** SPT_ui_xfer_GtoA - in which we transfer groove rhythm to lineseq */
void SPT_ui_xfer_GtoA(struct ui_CFG* cfg)
{
  wclear( wScrollable );

  char areyousure = 'n'; // confirm before proceeding
  wprintw( wScrollable, "COPY source to target shown below?\n" );
  wprintw( wScrollable, "\nSRC (grvpart)        -----      TGT (lineseq)\n" );
  SPT_ui_xferG_wprint( wScrollable, cfg->gIdx ); // NB field order
  SPT_ui_xferA_wprint( wScrollable, cfg->aIdx );
  
  wprintw( wScrollable, "\n\n\nSURE? 'Y' to confirm (any other to abort)\n" );
  areyousure = wgetch( wScrollable );
  if( 'y' != areyousure && 'Y' != areyousure ){
    wprintw( wScrollable, "ABORT OK - nothing changed" );
    return;
  }

  SPT_xfer_GtoA( cfg->gIdx, cfg->aIdx ); // apply changes
  SPT_overwriteFilePath();           // overwrite current file, saving changes
  wprintw( wScrollable, "DONE...\n" );

  if( !File_reloadFromPath() )       // we might display an error if this fails
  { mvwprintw(stdscr, 0, 45, "ERR %s\n", File_path ); }
}

/** SPT_ui_xfer_AtoG - in which we transfer lineseq rhythm to groove */
void SPT_ui_xfer_AtoG(struct ui_CFG* cfg)
{
  wclear( wScrollable );

  char areyousure = 'n'; // confirm before proceeding
  wprintw( wScrollable, "COPY source to target shown below?\n" );
  wprintw( wScrollable, "\nSRC (lineseq)                     TGT (grvpart)\n" );
  SPT_ui_xferA_wprint( wScrollable, cfg->aIdx ); // NB field order
  SPT_ui_xferG_wprint( wScrollable, cfg->gIdx );
  
  wprintw( wScrollable, "\n\n\nSURE? 'Y' to confirm (any other to abort)\n" );
  areyousure = wgetch( wScrollable );
  if( 'y' != areyousure && 'Y' != areyousure ){
    wprintw( wScrollable, "ABORT OK - nothing changed" );
    return;
  }

  SPT_xfer_AtoG( cfg->gIdx, cfg->aIdx ); // apply changes
  SPT_overwriteFilePath();           // overwrite current file, saving changes
  wprintw( wScrollable, "DONE...\n" );

  if( !File_reloadFromPath() )       // we might display an error if this fails
  { mvwprintw(stdscr, 0, 45, "ERR %s\n", File_path ); }
}

#endif /* uiSPT_xfer.h */
