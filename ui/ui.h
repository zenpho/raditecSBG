/*  ui.h

    user interface behaviour specific to raditecSBG
*/

#ifndef ui_h
#define ui_h

#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// struct ui_CFG defines current ui configuration state
struct ui_CFG {
  int drawPad;
  int scrollbufY;
  int pattFileIdx;
  int gIdx;      // SPT_xfer_grooveToLine and lineToGroove
  int aIdx;      // SPT_xfer_grooveToLine and lineToGroove
  int ArIdx;
  int IlIdx;
  char IlType;
  char dispCh;
  int dispHexDec; // 0=hex, 1=dec
};

// initialise configuration state to a sensibe default
void initcfg(struct ui_CFG *cfg)
{
  cfg->drawPad = true;
  cfg->scrollbufY = 0;
  cfg->pattFileIdx = 1;
  cfg->gIdx = 0;
  cfg->aIdx = 0;
  cfg->ArIdx = 0;
  cfg->IlIdx = 0;
  cfg->IlType = 'G';
  cfg->dispCh = -1;
  cfg->dispHexDec = 0;
}

#include "ui_utils.h"
#include "uiSPT_Il.h"
#include "uiSPT_xfer.h"

void SPT_ui_help_wprint(WINDOW* w)
{
  // fill our wHi pad with helpful information
  wclear( w );
  wprintw( w,
  "Radikal Technologies Spectralis SPT editor (zenpho.co.uk)\n"
  "==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ====\n"
  "This thing kludge-tastically modifies Groove and Line Sequences\n"
  "\n"
  "Use arrow up/down and page up/down keys to scroll\n"
  "Use command keys listed in the left panel\n"
  "\n"
  "Typical workflow:\n"
  "  - 'p' setpath /Volumes/SPC_FLASH/SONG01/\n"
  "  - ',' and '.' keys to select PATT01.SPT\n"
  "  - 'r' key views drum groove + lineseq triggers\n"
  "  - 'a' and 's' keys to select +- groove\n"
  "  - 'A' and 'S' keys to select +- lineseq\n"
  "  - 'x' and 'X' key clears selected\n"
  "  - 'c' key copies groove to line, overwriting PATT01.SPT\n"
  "  - 'C' key copies line to groove, overwriting PATT01.SPT\n"
  //"  - 'm' key toggles extmidi/internal sequencer config\n"
  //"  - 'i' key imports standard midi file to selected\n"
  );
}

/** SPT_ui_reprintPadsAll - in which we reprint data in pads */
void SPT_ui_reprintPadsAll(struct ui_CFG* cfg)
{
  SPT_ui_IlVal_wprint( cfg, wIl );
  SPT_ui_xfer_wprint( cfg, wXfer );
  SPT_ui_help_wprint( wHelp );
}

// currently supported terminal size in rows,cols
// we assign actual values later
static int termRows = 0;
static int termCols = 0;

// current state of the display mode
struct DisplayMode
{
  char dispCh;
  char* displayName;
};
const int NUM_DISPLAY_MODES = 5;
struct DisplayMode displayModes[NUM_DISPLAY_MODES] = {
  {'?', "help info"},
  {'r', "rhythms"},
  {'I', "tab Ils"}, {'i', "show Ils A"}, {'u', "show Ils G"},
};

char* getDisplayModeName(int dispCh)
{
  for(int i=0; i<NUM_DISPLAY_MODES; i++)
  {
    if( dispCh == displayModes[i].dispCh )
      return displayModes[i].displayName;
  }
  return NULL;
}

void drawTopStatus(struct ui_CFG *cfg)
{
  char* msg = "----------- v0.0 - PATT%02d - mode:%s";
  char* displayMode = getDisplayModeName(cfg->dispCh);
  
  mvwprintw(stdscr, 0, 0, msg, cfg->pattFileIdx, displayMode);
}

void drawBotStatus(struct ui_CFG *cfg)
{
  char* msg = "H toggle hex/dec. Scroll=%03d";
  int width = (int)strlen(msg);
  mvwprintw(stdscr, termRows - 1, termCols-width, msg, cfg->scrollbufY);
}

void drawCommands(WINDOW* win)
{
  int y = 1;
  redrawWindow(win, "Commands");
  mvwprintw(win, y++, 1, "p setpath");
  mvwprintw(win, y++, 1, ",.  SPT+-");
  y++;
  mvwprintw(win, y++, 1, "r rhythms");
  mvwprintw(win, y++, 1, "as  grv+-");
  mvwprintw(win, y++, 1, "AS  lin+-");
  y++;
  mvwprintw(win, y++, 1, "xX  clear");
  mvwprintw(win, y++, 1, "cC   copy");
  y++;
  mvwprintw(win, y++, 1, "Ee extint");
  y++;
  mvwprintw(win, y++, 1, "Iui   Ils");
  mvwprintw(win, y++, 1, "kl   Il+-");
  y++;
  mvwprintw(win, y++, 1, "q exit");
  wrefresh(win);
}

void drawScrollable(WINDOW* win, struct ui_CFG* cfg)
{
  if( cfg->drawPad )
    redrawWinPad(win, "Scrollable", wScrollbuff, cfg->scrollbufY);
  else
    redrawWindow(win, "Scrollable");
}

void keyHandler(int ch, struct ui_CFG *cfg)
{
  int maxY   = getmaxy(wScrollbuff);
  int pageY  = 20;
  
  switch (ch)
    {
      case KEY_UP:
        if (cfg->scrollbufY > 0) cfg->scrollbufY--;
      break;
      
      case KEY_DOWN:
        if (cfg->scrollbufY < maxY) cfg->scrollbufY++;
      break;
      
      case KEY_PPAGE:
        if (cfg->scrollbufY > pageY) cfg->scrollbufY-=pageY;
      break;
      
      case KEY_NPAGE:
        if (cfg->scrollbufY < maxY-pageY) cfg->scrollbufY+=pageY;
      break;
        
      case ' ': // startup
        displayCwd( wScrollable );
        bool retryChdir = false;
        char response   = ' ';
        do
        {
          retryChdir = chdir( "/Volumes/SPC_FLASH/" );
        
          if( retryChdir ) {
            wprintw( wScrollable, "\nWAITING FOR /Volumes/SPC_FLASH"
                                  "\n --- any key to retry (except 'q' to abort)" );
            wrefresh( wScrollable );
            response = wgetch(stdscr);
          }
          
        } while( retryChdir && response != 'q' );
        
        // load a pattern
        File_idx = cfg->pattFileIdx;
        if( !File_loadFromIdx() )     // we might display an error if this fails
        { mvwprintw(stdscr, 0, 45, "ERR %s\n", File_path ); }
        
        SPT_read();
        SPT_ui_reprintPadsAll(cfg);
      
        cfg->dispCh = '?';            // display helpful info
        keyHandler(cfg->dispCh, cfg); // by invoking keyHandler
        break;
        
      case '?':
        wScrollbuff = wHelp;         // switch to helpful info
        cfg->dispCh = ch;            // mode indicator
        break;
        
      case 'p':
        askChdir( wScrollable );      // must be wScrollable
        
        wclear( wScrollable );
        wclear( wScrollbuff );
        displayCwd( wScrollbuff );    // must be wScrollbuff
        
        File_idx = cfg->pattFileIdx;  // attempt load PATT01 from cwd
        if( !File_loadFromIdx() )     // we might display an error if this fails
        { mvwprintw(stdscr, 0, 45, "ERR %s\n", File_path ); }
        
        SPT_read();                   // parse File_data
        SPT_ui_reprintPadsAll(cfg);
        break;
        
      case 'e':
        SPT_ui_xfer_seqInternal(cfg);
        cfg->dispCh = 'r';           // switch to rhythm view
        keyHandler(cfg->dispCh, cfg);
        break;
      case 'E':
        SPT_ui_xfer_seqExtMIDI(cfg);
        cfg->dispCh = 'r';           // switch to rhythm view
        keyHandler(cfg->dispCh, cfg);
        break;
        
      case 'r':                         // rhythm view
        SPT_ui_xfer_wprint(cfg, wXfer); // reprint data in wXfer pad
        wScrollbuff = wXfer;            // focus on that pad
        cfg->dispCh = ch;               // make sure our mode indicator is valid
        break;
      
      case 'x':                      // clear groove
        SPT_ui_clearGroove(cfg);     // may (if agreed) clear selected Il 'G' drum part
        SPT_ui_reprintPadsAll(cfg);
        break;
      case 'X':                      // clear line
        SPT_ui_clearLine(cfg);       // may (if agreed) clear selected Il 'A' lineseq
        SPT_ui_reprintPadsAll(cfg);
        break;
      
        
      case 'c':                      // copy Grv to Aline
        SPT_ui_xfer_GtoA(cfg);       // may (if agreed) call SPT_xfer_GtoA() with parameters
                                     // and (if agreed) will overwrite File_path with changes
                                     // and (if agreed) wiill reload File_path for display
        SPT_ui_reprintPadsAll(cfg);
        cfg->IlType = 'A';           // focus on Il 'A' types for lineseq
        cfg->IlIdx = cfg->aIdx;      // focus on target lineseq index
        cfg->dispCh = 'r';           // switch to rhythm view
        keyHandler(cfg->dispCh, cfg);
        break;
      case 'C':                      // copy Aline to Grv
        SPT_ui_xfer_AtoG(cfg);       // may (if agreed) call SPT_xfer_AtoG() with parameters
                                     // and (if agreed) will overwrite File_path with changes
                                     // and (if agreed) wiill reload File_path for display
        SPT_ui_reprintPadsAll(cfg);
        cfg->IlType = 'A';           // focus on Il 'A' types for lineseq
        cfg->IlIdx = cfg->aIdx;      // focus on target lineseq index
        cfg->dispCh = 'r';           // switch to rhythm view
        keyHandler(cfg->dispCh, cfg);
        break;
      
      // inc-dec gIdx used for SPT_grooveToLine and lineToGroove
      case 'a':
        if( --cfg->gIdx < 0 ) cfg->gIdx = 10; // wrap around
        
        SPT_ui_xfer_wprint(cfg, wXfer);       // reprint data in xfer pad
        keyHandler(cfg->dispCh, cfg);
        break;
      case 's':
        if( ++cfg->gIdx > 10 ) cfg->gIdx = 0; // wrap around
        
        SPT_ui_xfer_wprint(cfg, wXfer);       // reprint data in xfer pad
        keyHandler(cfg->dispCh, cfg);
        break;
        
      // inc-dec aIdx used for SPT_grooveToLine and lineToGroove
      case 'A':
        if( --cfg->aIdx < 0 ) cfg->aIdx = 10; // wrap around
        
        SPT_ui_xfer_wprint(cfg, wXfer);       // reprint data in xfer pad
        keyHandler(cfg->dispCh, cfg);
        break;
      case 'S':
        if( ++cfg->aIdx > 10 ) cfg->aIdx = 0; // wrap around
        
        SPT_ui_xfer_wprint(cfg, wXfer);       // reprint data in xfer pad
        keyHandler(cfg->dispCh, cfg);
        break;
      
      // inc-dec pattFileIdx
      case ',':
        if( --cfg->pattFileIdx < 0 ) cfg->pattFileIdx = 31; // wrap around
        
        File_idx = cfg->pattFileIdx;
        if( !File_loadFromIdx() )     // we might display an error if this fails
        { mvwprintw(stdscr, 0, 45, "ERR %s\n", File_path ); }
        
        SPT_read();
        SPT_ui_reprintPadsAll(cfg);
        keyHandler(cfg->dispCh, cfg);
        break;
      case '.':
        if( ++cfg->pattFileIdx > 31 ) cfg->pattFileIdx = 0; // wrap around
        
        File_idx = cfg->pattFileIdx;
        if( !File_loadFromIdx() )     // we might display an error if this fails
        { mvwprintw(stdscr, 0, 45, "ERR %s\n", File_path ); }
        
        SPT_read();
        SPT_ui_reprintPadsAll(cfg);
        keyHandler(cfg->dispCh, cfg);
        break;
        
      case 'H':
      case 'h':
        cfg->dispHexDec++;           // toggle between hex and decimal display
        cfg->dispHexDec %= 2;        // ...but only for SPT_ui_dumpBy()
        keyHandler(cfg->dispCh, cfg);
        break;
      
      // view Il data for 'G' and 'A' types
      case 'I':
        SPT_ui_IlTab_wprint( cfg, wIl );
        wScrollbuff = wIl;
        cfg->dispCh = ch;
        break;
      case 'i':
        cfg->IlType = 'A';
        SPT_ui_IlVal_wprint( cfg, wIl );
        wScrollbuff = wIl;
        cfg->dispCh = ch;
        break;
      case 'u':
        cfg->IlType = 'G';
        SPT_ui_IlVal_wprint( cfg, wIl );
        wScrollbuff = wIl;
        cfg->dispCh = ch;
        break;
        
      // inc-dec IlIdx (used for Il data view and fuzzing below)
      case 'k':
        if( --cfg->IlIdx < 0 ) cfg->IlIdx = 31; // wrap around
        keyHandler(cfg->dispCh, cfg);
        break;
      case 'l':
        if( ++cfg->IlIdx > 31 ) cfg->IlIdx = 0; // wrap around
        keyHandler(cfg->dispCh, cfg);
        break;
        
      // lineseq targetPG (and targetSD) value fuzzing
      // inc-dec targetPG of selected IlA
      case '[':
        if( IlAs[ cfg->IlIdx ] ) {
          IlAs[ cfg->IlIdx ]->targetPG[0]--;
          keyHandler(cfg->dispCh, cfg);
        }
        break;
      case ']':
        if( IlAs[ cfg->IlIdx ] ) {
          IlAs[ cfg->IlIdx ]->targetPG[0]++;
          keyHandler(cfg->dispCh, cfg);
        }
        break;
      case '{':
        if( IlAs[ cfg->IlIdx ] ) {
          IlAs[ cfg->IlIdx ]->targetPG[1]--;
          keyHandler(cfg->dispCh, cfg);
        }
        break;
      case '}':
        if( IlAs[ cfg->IlIdx ] ) {
          IlAs[ cfg->IlIdx ]->targetPG[1]++;
          keyHandler(cfg->dispCh, cfg);
        }
        break;
    }
}

void drawWindows(WINDOW* wCommands, WINDOW* wScrollable, struct ui_CFG* cfg)
{
  drawCommands(wCommands);
  drawScrollable(wScrollable, cfg);
  drawTopStatus(cfg);
  drawBotStatus(cfg);
}

void ui_exit(){ endwin(); }

void ui_init(struct ui_CFG *cfg)
{
  // set default ui config state
  initcfg( cfg );

  // initialize curses
  initscr();
  atexit(ui_exit);
  clear();
  cbreak();
  curs_set(0);
  keypad(stdscr, TRUE);

  // get terminal size
  getmaxyx(stdscr, termRows, termCols);
  
  // create pads that we'll switch to/from and scroll later
  wIl = newpad( 200, termCols );
  wHelp = newpad( 200, termCols );
  wXfer = newpad( 200, termCols );
  wScrollbuff = newpad( 200, termCols );
  
  // create windows, args are height, width, top, left
  wCommands = newwin( 22, 11, 1, 0 );
  wScrollable = newwin( 22, 69, 1, 11 );
  
  // fill our pads with loaded and parsed SPT file data
  File_idx = cfg->pattFileIdx = 1;
  if( !File_loadFromIdx() )     // we might display an error if this fails
  { mvwprintw(stdscr, 0, 45, "ERR %s\n", File_path ); }
  
  SPT_read();
  SPT_ui_reprintPadsAll(cfg);
  
  // initial loading message, ui_mainloop() and ui_keyhandler() replace this
  wprintw( wScrollable, "loading..." );
}

void ui_mainloop(struct ui_CFG* cfg)
{
  // keypress loop
  noecho();
  int ch = ' ';
  do
  {
    clear();
    refresh();

    keyHandler(ch, cfg);
    drawWindows(wCommands, wScrollable, cfg);
  }
  while((ch = wgetch(stdscr)) != 'q');
}

void ui_teardown(struct ui_CFG* cfg)
{
  // remove window
  delwin(wCommands);
  delwin(wScrollable);
  delwin(wScrollbuff);
  clear();
  refresh();
}

#endif /* ui.h */
