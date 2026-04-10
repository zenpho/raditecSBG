/* SPT_xfer.h

   functions to transfer from grooveseq to lineseq
   plus other modifications and writing modified SPT files
*/
#ifndef SPT_xfer_h
#define SPT_xfer_h

#include "SPT_types.h"
#include "SPT_Il.h"
#include "ui.h"

/** SPT_xfer_AtoG - in which we transfer a lineseq to a groove rhythm
    currently without any pitch information and
    currently only at 48ppqn '16th' step resolution and
    currently only for a total of 16 lineseq steps
*/
void SPT_xfer_AtoG(uint8_t gIdx, uint8_t aIdx)
{
  // count number of 'active' events in source IlAVdata[]
  uint8_t numAVs = IlAs[aIdx]->h.numBytesVData / sizeof(SPT_Il_vData);
  uint8_t count = 0;
  for(int step=0; step<numAVs; step++)
  {
    if( step > 15 ) continue;                  // skip beyond range 0..15
    if( IlAVData[aIdx][step].a & 1 ) count++;  // trigstate is lsb of field 'a'
  }
  
  // we may end here if the source sequence is blank
  if( 0==count ) return;
  
  // junk existing IlGVData[] target in preparation for new
  if( IlGVData[gIdx] ) free( IlGVData[gIdx] );
  
  // we malloc() for new IlGVdata[] appropriately
  IlGs[gIdx]->h.numBytesVData = count*sizeof(SPT_Il_vData);
  IlGVData[gIdx] = malloc( IlGs[gIdx]->h.numBytesVData );
  
  // loop over the IlAVdata[] again inserting counted events
  count = 0;
  for(int aStep=0; aStep<numAVs; aStep++)
  {
    uint8_t trig = IlAVData[aIdx][aStep].a & 1; // trigstate is lsb field 'a'
    if( trig )
    {
      IlGVData[gIdx][count].a = 228;        // note 60, velo 100
      IlGVData[gIdx][count].b = aStep * 12; // at 48ppqn this represents 1/16th
      
      count++;
    }
  }
  
  // we have exactly 1 bar of 16 steps at 16th resolution for IlGs[]
  IlGs[gIdx]->h.linelen = 192; // always 192
  IlGs[gIdx]->lenbar = 192;    // always 192
  IlGs[gIdx]->numbars = 1;     // exactly one bar
}

/** SPT_xfer_GtoA - in which we transfer a groove rhythm to a lineseq
    currently without any pitch information and
    currently only at 48ppqn '16th' step resolution and
    currently only for a total of 16 lineseq destination steps and
    currently always gIdx 'dsyn' (kick,etc) lineseq target assigned
*/
void SPT_xfer_GtoA(uint8_t gIdx, uint8_t aIdx)
{
  if( 0==IlAs[aIdx] )  // lineseq not yet exists?
  {
    IlAs[aIdx] = (SPT_Il_A*)malloc( sizeof(SPT_Il_A) );
    memset( IlAs[aIdx], 0, sizeof(SPT_Il_A) ); // zero all fields
    
    IlAs[aIdx]->h.numBytesStruc = sizeof(SPT_Il_A); // must compute
    IlAs[aIdx]->h.type = 'A';
    IlAs[aIdx]->h.counter = aIdx;
    IlAs[aIdx]->h.name[0] = 'A';
    IlAs[aIdx]->h.name[1] = aIdx+1;
    IlAs[aIdx]->h.editres = 6;   // represents 1/16th
    // targetPG and targetSD set below
    IlAs[aIdx]->editgate = 16;
    IlAs[aIdx]->editveldep = 126;
    IlAs[aIdx]->configtransp = 127;
  }

  // assign IlAs[] targetPG and targetSG
  IlAs[aIdx]->targetPG[0] = 0xa4; // 0xa4, 0x028 means 'dsyn'
  IlAs[aIdx]->targetPG[1] = 0x28; // for sampled drum parts
  IlAs[aIdx]->targetSD[0] = gIdx; // 0,1,2 kick,snare,clhh etc
  IlAs[aIdx]->targetSD[1] = 0;    // always zero
  
  // junk existing IlAVData[]
  if( IlAVData[aIdx] ) free( IlAVData[aIdx] );

  // now malloc() 16 steps of IlAVData on ILAs[]
  IlAs[aIdx]->h.linelen = 16;
  IlAs[aIdx]->h.numBytesVData = 16*sizeof(SPT_Il_vData);
  IlAVData[aIdx] = malloc( IlAs[aIdx]->h.numBytesVData );
  
  // and zero all values
  for(int i=0; i<16; i++){ IlAVData[aIdx][i].a = IlAVData[aIdx][i].b = 0; }
  
  // we may end here if the source sequence is blank
  if( 0==IlGs[gIdx]->h.numBytesVData ) return;  // abort if no numBytesVData
  if( 0==IlGVData[gIdx] ) return;  // abort if no IlGVData
  
  // loop over the IlGVData[gIdx] entries
  uint8_t numgVs = IlGs[gIdx]->h.numBytesVData / sizeof(SPT_Il_vData);
  for(int gv=0; gv<numgVs; gv++)
  {
    uint16_t time = IlGVData[gIdx][gv].b; // the timecode in 'G'roove vData field 'b'
    uint8_t step = time / 12;             // at 48ppqn, this represents 1/16th steps
    if( step > 15 ) continue;             // skip over any beyond range 0..15

    // we're going to generate lineseq steps that have this profile
    uint8_t trig = 1;   // 1=active
    uint8_t prob = 0;   // 0=100%
    uint8_t gate = 3;   // 3=wholestep
    uint8_t velo = 100;
    uint8_t note = 60;
    
    SPT_Il_vData *curVDat = &IlAVData[aIdx][step];
    curVDat->a = (velo<<9) | (gate<<4) | (prob<<1) | trig;
    curVDat->b = 10240 | note<<1; // 10240 is 'normal' (4th len, unmuted, unskipped, etc)
  }
}

/** TODO move SPT_overwriteFilePath() and SPT_write_31() to SPT_filewrite */

/** SPT_overwriteFilePath - in which we write SPT data to File_path */
void SPT_overwriteFilePath()
{
  if( 0 == File_path ) return; // abort?
  
  FILE* fout = fopen(File_path, "wb");
  if( 0 == fout )              // abort?
  {
    wprintw(wScrollbuff, "ERROR opening %s\n", File_path);
    return;
  }
  
  SPT_tP_fwrite(fout);
  SPT_Tp_fwrite(fout);
  SPT_Sa_fwrite(fout);
  SPT_Pm_fwrite(fout);
  SPT_Ec_fwrite(fout);
  SPT_Im_fwrite(fout);
  SPT_IL_FWRITE_WITH_VDATA( fout, IlAs, IlAVData );
  SPT_IL_FWRITE_WITH_VDATA( fout, IlGs, IlGVData );
  SPT_IL_FWRITE( fout, IlMs );
  SPT_IL_FWRITE( fout, IlPs );
  SPT_Ar_fwrite(fout);
  
  fclose(fout);
}

/** SPT_write_31 - in which we write SPT data to PAT31.SPT */
void SPT_write_31()
{
  FILE* fout = fopen("./PATT31.SPT", "wb");
  if( 0 == fout ) return;
  
  SPT_tP_fwrite(fout);
  SPT_Tp_fwrite(fout);
  SPT_Sa_fwrite(fout);
  SPT_Pm_fwrite(fout);
  SPT_Ec_fwrite(fout);
  SPT_Im_fwrite(fout);
  SPT_IL_FWRITE_WITH_VDATA( fout, IlAs, IlAVData );
  SPT_IL_FWRITE_WITH_VDATA( fout, IlGs, IlGVData );
  SPT_IL_FWRITE( fout, IlMs );
  SPT_IL_FWRITE( fout, IlPs );
  SPT_Ar_fwrite(fout);
  
  fclose(fout);
}

/** TODO move SPT_xfer_clearG() and SPT_xfer_clearA() to SPT_clear */

/** SPT_xfer_clearA - in which groove rhythm for index is cleared */
void SPT_xfer_clearG(uint8_t gIdx)
{
  if( 0==IlGs[gIdx] ) return;     // abort if invalid
  
  // junk existing IlGVData[]
  if( IlGVData[gIdx] ) free( IlGVData[gIdx] );
  IlGs[gIdx]->h.numBytesVData = 0;
}

/** SPT_xfer_clearA - in which lineseq rhythm + target for index is cleared */
void SPT_xfer_clearA(uint8_t aIdx)
{
  if( 0==IlAs[aIdx] ) return;     // abort if invalid

  // assign IlAs[] targetPG and targetSG
  IlAs[aIdx]->targetPG[0] = 0x76; // 0x76, 0x0e, 0, 0 means 'filterbank bypass'
  IlAs[aIdx]->targetPG[1] = 0x0e; // ...
  IlAs[aIdx]->targetSD[0] = 0;    // ...
  IlAs[aIdx]->targetSD[1] = 0;    // ...
  
  // junk existing IlAVData[]
  if( IlAVData[aIdx] ) free( IlAVData[aIdx] );
  IlAs[aIdx]->h.numBytesVData = 0;
}

/** TODO move SPT_seq_GA_internal() and SPT_seq_GA_ExtMIDI() to SPT_seq */

/** SPT_seqA_extToInt - in which we set IlAs[] target to dsyn parts */
void SPT_seqA_extToInt(uint8_t aIdx)
{
  if( 0==IlAs[aIdx] ) return;          // abort if invalid
  
  if( 0xad==IlAs[aIdx]->targetPG[0] && // targetPG is extMIDI note?
      0x28==IlAs[aIdx]->targetPG[1] )  // matching both PG[0] and PG[1]
    {
      uint8_t SD0 = IlAs[aIdx]->targetSD[0]; // SD0 is extMIDI statusbyte
                                             // 0x90 for note-on ch1
                                             // 0x9F for note-on ch16
      
      if( 0x91==SD0 ) IlAs[aIdx]->targetSD[0] = 0x00; // ch02 > dsyn kick
      if( 0x92==SD0 ) IlAs[aIdx]->targetSD[0] = 0x01; // ch03 > dsyn snar
      if( 0x93==SD0 ) IlAs[aIdx]->targetSD[0] = 0x02; // ch04 > dsyn clhh
      if( 0x94==SD0 ) IlAs[aIdx]->targetSD[0] = 0x03; // ch05 > dsyn ophh
      if( 0x95==SD0 ) IlAs[aIdx]->targetSD[0] = 0x04; // ch06 > dsyn clap
      if( 0x96==SD0 ) IlAs[aIdx]->targetSD[0] = 0x05; // ch07 > dsyn htom
      if( 0x97==SD0 ) IlAs[aIdx]->targetSD[0] = 0x06; // ch08 > dsyn mtom
      if( 0x98==SD0 ) IlAs[aIdx]->targetSD[0] = 0x07; // ch09 > dsyn ltom
      if( 0x99==SD0 ) IlAs[aIdx]->targetSD[0] = 0x08; // ch10 > dsyn prc1
      if( 0x9a==SD0 ) IlAs[aIdx]->targetSD[0] = 0x09; // ch11 > dsyn prc2
      if( 0x9b==SD0 ) IlAs[aIdx]->targetSD[0] = 0x0a; // ch12 > dsyn prc3
      
      IlAs[aIdx]->targetPG[0] = 0xa4; // target now dsyn parts
      IlAs[aIdx]->targetPG[1] = 0x28; // for both PG[0] and PG[1]
      IlAs[aIdx]->targetSD[1] = 0x00; // SD1 always zero
    }
}

/** SPT_seqA_intToExt - in which we set IlAs[] target to extMIDI */
void SPT_seqA_intToExt(uint8_t aIdx)
{
  if( 0==IlAs[aIdx] ) return;          // abort if invalid
  
  if( 0xa4==IlAs[aIdx]->targetPG[0] && // current targetPG is internal dsyn?
      0x28==IlAs[aIdx]->targetPG[1] )  // matching both PG[0] and PG[1]
    {
      uint8_t SD0 = IlAs[aIdx]->targetSD[0]; // SD0 is dsyn number
      
      if( 0x00==SD0 ) IlAs[aIdx]->targetSD[0] = 0x91; // dsyn kick >ch02
      if( 0x01==SD0 ) IlAs[aIdx]->targetSD[0] = 0x92; // dsyn snar >ch03
      if( 0x02==SD0 ) IlAs[aIdx]->targetSD[0] = 0x93; // dsyn clhh >ch04
      if( 0x03==SD0 ) IlAs[aIdx]->targetSD[0] = 0x94; // dsyn ophh >ch05
      if( 0x04==SD0 ) IlAs[aIdx]->targetSD[0] = 0x95; // dsyn clap >ch06
      if( 0x05==SD0 ) IlAs[aIdx]->targetSD[0] = 0x96; // dsyn htom >ch07
      if( 0x06==SD0 ) IlAs[aIdx]->targetSD[0] = 0x97; // dsyn mtom >ch08
      if( 0x07==SD0 ) IlAs[aIdx]->targetSD[0] = 0x98; // dsyn ltom >ch09
      if( 0x08==SD0 ) IlAs[aIdx]->targetSD[0] = 0x99; // dsyn prc1 >ch10
      if( 0x09==SD0 ) IlAs[aIdx]->targetSD[0] = 0x9a; // dsyn prc2 >ch11
      if( 0x0a==SD0 ) IlAs[aIdx]->targetSD[0] = 0x9b; // dsyn prc3 >ch12
      
      IlAs[aIdx]->targetPG[0] = 0xad; // target now extMIDI note
      IlAs[aIdx]->targetPG[1] = 0x28; // for both PG[0] and PG[1]
      IlAs[aIdx]->targetSD[1] = 0x00; // SD1 always zero
    }
}

void SPT_seqGA_internal(void)
{
  for(int gIdx=0; gIdx<=10; gIdx++) {
    if( 0==IlGs[gIdx] ) continue; // skip invalid
  
    IlGs[gIdx]->mididef = 0;      // 0 means internal dsyn only
  }
  
  for(int aIdx=0; aIdx<=10; aIdx++) {
    SPT_seqA_extToInt(aIdx);      // if currently ext, now dsyn
  }
}

void SPT_seqGA_extMIDI(void)
{
  for(int gIdx=0; gIdx<=10; gIdx++) {
    if( 0==IlGs[gIdx] ) continue; // skip invalid
    
    IlGs[gIdx]->mididef = 65 + gIdx; // decimal 65..76 means ch2..ch12
  }
  
  for(int aIdx=0; aIdx<=10; aIdx++) {
    SPT_seqA_intToExt(aIdx);     // if currently dsyn, now ext
  }
}

#endif /* SPT_xfer.h */
