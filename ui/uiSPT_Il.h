/* uiSPT_Il.h

   user interface for display of interpreted SPT Il chunk data
   Il chunks contain sequencer configuration and timing (see SPT_types.h)
*/
#ifndef uiSPT_Il_h
#define uiSPT_Il_h

#include "ui.h"
#include "uiSPT_common.h"
#include "SPT_parse.h"

void SPT_ui_IlHead_wprint( WINDOW* w, struct ui_CFG* cfg, SPT_Il_head* h  )
{
  if( 0 == h ) return;
  
  wprintw( w, "numBytesStruc: %d bytes\n", h->numBytesStruc );
  wprintw( w, "numBytesVData: %d bytes\n", h->numBytesVData );
  wprintw( w, "type: %c\n", h->type );
  wprintw( w, "counter: %d\n", h->counter );
  wprintw( w, "name: %s\n", h->name );
  wprintw( w, "linelen: %d\n", h->linelen );
  wprintw( w, "mystery1: %d\n", h->mystery1);
  
  if( h->type == 'A' ) wprintw( w, "editres (rate): %d\n", h->editres );
  else                 wprintw( w, "editres (zoom): %d\n", h->editres );
}

void SPT_ui_IlTab_wprint( struct ui_CFG* cfg, WINDOW* w )
{
  uint8_t numG = 0;
  uint8_t numA = 0;
  uint8_t numM = 0;
  uint8_t numP = 0;
  
  uint8_t numGVData = 0;
  uint8_t numAVData = 0;
  for(int i=0; i<32; i++)
  {
    if( IlGs[i] ) numG++;
    if( IlAs[i] ) numA++;
    if( IlMs[i] ) numM++;
    if( IlPs[i] ) numP++;
    
    if( IlGVData[i] ) numGVData++;
    if( IlAVData[i] ) numAVData++;
  }

  wclear(w);
  wprintw(w, "Ils count --- %d total\n", numG+numA+numM+numP);
  wprintw(w, "G: %d\n", numG);
  wprintw(w, "A: %d\n", numA);
  wprintw(w, "M: %d\n", numM);
  wprintw(w, "P: %d\n", numP);
  
  wprintw(w, "GVData %d\n", numGVData);
  wprintw(w, "AVData %d\n", numAVData);
}

void SPT_ui_Il_vData_wprint( WINDOW* w, SPT_Il_head* h, SPT_Il_vData** vData, uint8_t i )
{
  if( 0 == h ) return;
  // a null vData[0] is valid so 'if( 0 == *vData ) return;' is not cool
  if( 0 == vData[i] ) return;
  
  wprintw( w, "interpreted vData[%d] --- %d bytes\n", i, h->numBytesVData );
  uint8_t numVs = h->numBytesVData / sizeof(*vData[i]);
  for(int v=0; v<numVs; v++)
  {
    uint16_t a   = vData[i][v].a;
    uint16_t b   = vData[i][v].b;
    
    uint8_t  prob_percent[8] = {
       100, 88, 75, 63, 50, 38, 25, 13
    };
    
    char*    gate_text[16] = {
      "8th", "4th", "2th", "1  ", "2  ", "3  ", "4  ", "5  ",
      "6  ", "7  ", "8  ", "9  ", "10 ", "11 ", "12 ", "13 "
    };
    
    if( 'A' == h->type )
    {
      uint8_t trig = a & 1;           // range 0 - 1
      uint8_t prob = (a >> 1) & 0x7;  // range 0 - 7
      uint8_t gate = (a >> 4) & 0x0F; // range 0 - 15
      uint8_t velo = (a >> 9) & 0x7F; // range 0 - 127
      uint8_t note = (b >> 1) & 0x7F; // range 0 - 127
      wprintw( w, "[%d] trig:%01d prob#:%02d note:%03d len:%s velo:%03d\n",
                     v, trig, prob_percent[prob], note, gate_text[gate], velo );
    }
  
    if( 'G' == h->type )
    {
      uint16_t trig = a;
      uint16_t pos  = b;
      
      wprintw( w, "[%d] trig#:%02d pos:%04d\n", v, trig, pos );
    }
  }// for
  
  wprintw( w, "raw vData --- %d bytes\n", i, h->numBytesVData );
  for(int v=0; v<numVs; v++)
  {
    wprintw( w, "[%d] a %d\n", v, vData[i][v].a );
    wprintw( w, "[%d] b %d\n", v, vData[i][v].b );
  }// for
}

void SPT_ui_IlVal_wprint( struct ui_CFG* cfg, WINDOW* w )
{
  uint8_t i    = cfg->IlIdx;
  char    type = cfg->IlType;
  
  wclear(w);
  wprintw( w, "%s\n", Tp.name );
  
  if( type == 'G' )
  {
    wprintw( w, "IlGs[%d] ", i );
    if( 0 == IlGs[i] ) return;
    
    uint16_t totalBytes = IlGs[i]->h.numBytesStruc + IlGs[i]->h.numBytesVData;
    wprintw( w, "%d bytes\n", totalBytes );
    SPT_ui_IlHead_wprint( w, cfg, &IlGs[i]->h );
    if(0)
    {
     wprintw(w, "mystery1:");
     SPT_ui_dumpBy(w, cfg, IlGs[i]->mystery1, sizeof(IlGs[i]->mystery1));
    }
    wprintw( w, "pitch: %d\n", IlGs[i]->pitch );
    wprintw( w, "editplus: %d\n", IlGs[i]->editplus );
    wprintw( w, "editvelo: %d\n", IlGs[i]->editvelo );
    wprintw( w, "lenbar: %d\n", IlGs[i]->lenbar );
    wprintw( w, "numbars: %d\n", IlGs[i]->numbars );
    wprintw( w, "mididef: %d\n", IlGs[i]->mididef );
    
    //if( 0 == IlGVData[i] ) return;
    SPT_ui_Il_vData_wprint( w, &IlGs[i]->h, IlGVData, i );
    
  }
    
  if( type == 'A' )
  {
    wprintw( w, "IlAs[%d]", i );
    if( 0 == IlAs[i] ) return;
    
    wprintw( w, "%d bytes\n", sizeof(*IlAs[i]) );
    SPT_ui_IlHead_wprint( w, cfg, &IlAs[i]->h );
    wprintw(w, "targetPG:");
    SPT_ui_dumpBy(w, cfg, IlAs[i]->targetPG, sizeof(IlAs[i]->targetPG));
    wprintw( w, "glide: %d\n", IlAs[i]->glide );
    wprintw( w, "mystery2: %d\n", IlAs[i]->mystery2 );
    wprintw( w, "editgate: %d\n", IlAs[i]->editgate );
    wprintw( w, "editveldep: %d\n", IlAs[i]->editveldep );
    wprintw(w, "targetSD:");
    SPT_ui_dumpBy(w, cfg, IlAs[i]->targetSD, sizeof(IlAs[i]->targetSD));
    wprintw( w, "editpitch: %d\n", IlAs[i]->editpitch );
    wprintw( w, "mystery3: %d\n", IlAs[i]->mystery3 );
    wprintw( w, "configtransp: %d\n", IlAs[i]->configtransp );
    wprintw(w, "mystery4:");
    SPT_ui_dumpBy(w, cfg, IlAs[i]->mystery4, sizeof(IlAs[i]->mystery4));

    SPT_ui_Il_vData_wprint( w, &IlAs[i]->h, IlAVData, i );
  }
  
  if( type == 'M' )
  {
    wprintw( w, "IlMs[%d] %d bytes\n", i, sizeof(*IlMs[i]) );
  }
  
  if( type == 'P' )
  {
    wprintw( w, "IlPs[%d] %d bytes\n", i, sizeof(*IlPs[i]) );
  }
}

#endif /* uiSPT_Il.h */
