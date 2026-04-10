/* ui_utils.h

   user interface utilities for working with ncurses windows and pads
   and functions to get and store user responses to prompts
   and functions to display and change working directory
*/

#ifndef curses_utils_h
#define curses_utils_h

#include "ui.h"

// windows here represent different text "pad" buffers
// three primary ones to discuss: wCommands, wScrollable, and wScrollbuf
// wCommands shows a list of commands handled by the keyHandler() function
// wScrollable is a window into the larger dataset stored in wScrollbuf
// other pads represent views that we can switch between
WINDOW *wCommands;
WINDOW *wScrollable;
WINDOW *wScrollbuff;
WINDOW *wIl;
WINDOW *wHelp;
WINDOW *wXfer;

/** askGetN - in which we prompt, then read maxLen chars of response */
void askGetN(WINDOW* w, char* prompt, char* response, uint16_t maxlen )
{
  if( 0 == prompt ) return;
  if( 0 == maxlen ) maxlen = 1;
  
  wprintw(w, prompt);
  wrefresh(w);
  
  echo();
  curs_set(1);
  wgetnstr(w, response, maxlen);
  cbreak();
  curs_set(0);
}

/** displayCwd - display current working directory */
void displayCwd(WINDOW* w)
{
  char buff[1024];
  memset( buff, 0, 1024 );
  getcwd(buff, 1024);
 
  wclear(w);
  wprintw(w, "CWD:%s\n", buff);
}

/** askChdir - in which we call chdir() */
void askChdir(WINDOW* w)
{
  char* buff = malloc( 1024 );
  memset( buff, 0, 1024 );
  
  wprintw( w, "FILE OPERATIONS - CHDIR\n");
  askGetN( w, "path: ", buff, 1024 );
  wprintw( w, "SURE? - %s 'y' to confirm (any other to abort): ", buff);
  curs_set(1);
  char response = wgetch(w);
  curs_set(0);
  if( 'y' == response ){ chdir(buff); }
  if( 'Y' == response ){ chdir(buff); }
}

void redrawSubPad(WINDOW* sourcePad, int scrollY, int top, int left, int botm, int right)
{
  int viewHeight = botm - top;
  int viewWidth  = right - left;
  int scrollX    = 0;

  WINDOW *viewSubPad = subpad(sourcePad, viewHeight-2, viewWidth-2, scrollY, scrollX);
  prefresh(viewSubPad, 0, 0, top+1, left+1, top+viewHeight, left+viewWidth);
}

void redrawWindow(WINDOW* win, const char* label)
{
  // subpad of the sourcePad drawn into provided window
  int top    = getbegy(win);
  int left   = getbegx(win);
  int height = getmaxy(win);
  int width  = getmaxx(win);
  
  int right  = left + width;
  int botm   = top + height;

  box(win, 0 , 0);
  mvwprintw(win, 0, 1, label);
  
  const int debugPos = false;
  if( debugPos )
  {
    wmove(win, 1, 3);
    mvwprintw(win, 1, 1, "top\t left\t botm\t right");

    char buf[80];
    snprintf(buf, 80, "%d\t %d\t %d\t %d", top, left, botm, right);
    mvwprintw(win, 2, 1, buf);
  }
  wrefresh(win);
}

void redrawWinPad(WINDOW* win, const char* label, WINDOW* sourcePad, int scrollY)
{
  // subpad of the sourcePad drawn into provided window
  int top    = getbegy(win);
  int left   = getbegx(win);
  int height = getmaxy(win);
  int width  = getmaxx(win);
  
  int right  = left + width;
  int botm   = top + height;

  redrawWindow(win, label);
  redrawSubPad(sourcePad, scrollY, top, left, botm, right);
}

void fillPad(WINDOW* pad)
{
  int padSize = getmaxy(pad);

  for(int i=0; i<padSize; i++)
    {
      char temp[80];
      memset(temp, 0, 80);
      
      snprintf(temp, 80, "%d", i);
      wprintw(pad, temp);
      wprintw(pad, "\n");
    }
}

void randPad(WINDOW* pad)
{
  int padSize = getmaxy(pad);

  for(int i=0; i<padSize; i++)
    {
      char temp[80];
      memset(temp, 0, 80);
      
      int r = rand();
      
      snprintf(temp, 80, "%d", r);
      wprintw(pad, temp);
      wprintw(pad, "\n");
    }
}

void alphaPad(WINDOW* pad)
{
  int padSize = getmaxy(pad);

  for(int i=0; i<padSize; i++)
    {
      char temp[80];
      memset(temp, 0, 80);
      
      snprintf(temp, 80, "%c", (i%26)+ 33);
      wprintw(pad, temp);
      wprintw(pad, "\n");
    }
}

#endif /* curses_utils.h */
