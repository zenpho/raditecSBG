/* uiSPT_common.h

   user interface common functions - e.g. display of bytes as hex/decimal
*/

#ifndef uiSPT_common_h
#define uiSPT_common_h

/** SPT_ui_dump in which we display bytes (as hex default) */
void SPT_ui_dump(uint8_t* data, uint16_t dataSize, const char* dispFormat, WINDOW* w)
{
    if( strlen(dispFormat) == 0 ) dispFormat = "%02x ";

    uint8_t dispColl = 1;
    uint16_t remain = dataSize;
    while( remain-- )
    {
        if( dispColl % 8 == 1) wprintw(w, "%03d: ", dataSize-remain-1);
    
        wprintw(w, dispFormat, *data);
      
        if( dispColl % 4 == 0) wprintw(w, "   ");
        if( dispColl % 8 == 0) wprintw(w, "\n");
      
        dispColl++;
        data++;
    }
    wprintw(w, "\n");
}

/** SPT_ui_dumpBy in which we display bytes configured by cfg for hex/dec formatting */
void SPT_ui_dumpBy(WINDOW* w, struct ui_CFG* cfg, uint8_t* data, uint16_t dataSize)
{
  if( 0 == cfg->dispHexDec )
  {
    wprintw(w, " %d bytes HEX\n", dataSize);
    SPT_ui_dump(data, dataSize, "%02x " , w);
  }
  if( 1 == cfg->dispHexDec )
  {
    wprintw(w, " %d bytes DEC\n", dataSize);
    SPT_ui_dump(data, dataSize, "%03d " , w);
  }
}

#endif /* uiSPT_common.h */
