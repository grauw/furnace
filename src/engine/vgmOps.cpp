/**
 * Furnace Tracker - multi-system chiptune tracker
 * Copyright (C) 2021-2022 tildearrow and contributors
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "engine.h"
#include "../ta-log.h"
#include "../utfutils.h"
#include "song.h"

constexpr int MASTER_CLOCK_PREC=(sizeof(void*)==8)?8:0;

void DivEngine::performVGMWrite(SafeWriter* w, DivSystem sys, DivRegWrite& write, int streamOff, double* loopTimer, double* loopFreq, int* loopSample, bool isSecond) {
  unsigned char baseAddr1=isSecond?0xa0:0x50;
  unsigned char baseAddr2=isSecond?0x80:0;
  unsigned short baseAddr2S=isSecond?0x8000:0;
  unsigned char smsAddr=isSecond?0x30:0x50;
  if (write.addr==0xffffffff) { // Furnace fake reset
    switch (sys) {
      case DIV_SYSTEM_YM2612:
      case DIV_SYSTEM_YM2612_EXT:
        for (int i=0; i<3; i++) { // set SL and RR to highest
          w->writeC(2|baseAddr1);
          w->writeC(0x80+i);
          w->writeC(0xff);
          w->writeC(2|baseAddr1);
          w->writeC(0x84+i);
          w->writeC(0xff);
          w->writeC(2|baseAddr1);
          w->writeC(0x88+i);
          w->writeC(0xff);
          w->writeC(2|baseAddr1);
          w->writeC(0x8c+i);
          w->writeC(0xff);

          w->writeC(3|baseAddr1);
          w->writeC(0x80+i);
          w->writeC(0xff);
          w->writeC(3|baseAddr1);
          w->writeC(0x84+i);
          w->writeC(0xff);
          w->writeC(3|baseAddr1);
          w->writeC(0x88+i);
          w->writeC(0xff);
          w->writeC(3|baseAddr1);
          w->writeC(0x8c+i);
          w->writeC(0xff);
        }
        for (int i=0; i<3; i++) { // note off
          w->writeC(2|baseAddr1);
          w->writeC(0x28);
          w->writeC(i);
          w->writeC(2|baseAddr1);
          w->writeC(0x28);
          w->writeC(4+i);
        }
        w->writeC(2|baseAddr1); // disable DAC
        w->writeC(0x2b);
        w->writeC(0);
        break;
      case DIV_SYSTEM_SMS:
        for (int i=0; i<4; i++) {
          w->writeC(smsAddr);
          w->writeC(0x90|(i<<5)|15);
        }
        break;
      case DIV_SYSTEM_GB:
        // square 1
        w->writeC(0xb3);
        w->writeC(2|baseAddr2);
        w->writeC(0);
        w->writeC(0xb3);
        w->writeC(4|baseAddr2);
        w->writeC(0x80);

        // square 2
        w->writeC(0xb3);
        w->writeC(7|baseAddr2);
        w->writeC(0);
        w->writeC(0xb3);
        w->writeC(9|baseAddr2);
        w->writeC(0x80);

        // wave
        w->writeC(0xb3);
        w->writeC(0x0c|baseAddr2);
        w->writeC(0);
        w->writeC(0xb3);
        w->writeC(0x0e|baseAddr2);
        w->writeC(0x80);

        // noise
        w->writeC(0xb3);
        w->writeC(0x11|baseAddr2);
        w->writeC(0);
        w->writeC(0xb3);
        w->writeC(0x13|baseAddr2);
        w->writeC(0x80);
        break;
      case DIV_SYSTEM_PCE:
        for (int i=0; i<6; i++) {
          w->writeC(0xb9);
          w->writeC(0|baseAddr2);
          w->writeC(i);
          w->writeC(0xb9);
          w->writeC(4|baseAddr2);
          w->writeC(0);
        }
        break;
      case DIV_SYSTEM_NES:
        w->writeC(0xb4);
        w->writeC(0x15|baseAddr2);
        w->writeC(0);
        break;
      case DIV_SYSTEM_YM2151:
        for (int i=0; i<8; i++) {
          w->writeC(4|baseAddr1);
          w->writeC(0xe0+i);
          w->writeC(0xff);
          w->writeC(4|baseAddr1);
          w->writeC(0xe8+i);
          w->writeC(0xff);
          w->writeC(4|baseAddr1);
          w->writeC(0xf0+i);
          w->writeC(0xff);
          w->writeC(4|baseAddr1);
          w->writeC(0xf8+i);
          w->writeC(0xff);

          w->writeC(4|baseAddr1);
          w->writeC(0x08);
          w->writeC(i);
        }
        break;
      case DIV_SYSTEM_SEGAPCM:
      case DIV_SYSTEM_SEGAPCM_COMPAT:
        for (int i=0; i<16; i++) {
          w->writeC(0xc0);
          w->writeS((0x86|baseAddr2S)+(i<<3));
          w->writeC(3);
        }
        break;
      case DIV_SYSTEM_X1_010:
        for (int i=0; i<16; i++) {
          w->writeC(0xc8);
          w->writeS_BE(baseAddr2S+(i<<3));
          w->writeC(0);
        }
        break;
      case DIV_SYSTEM_YM2610:
      case DIV_SYSTEM_YM2610_FULL:
      case DIV_SYSTEM_YM2610B:
      case DIV_SYSTEM_YM2610_EXT:
      case DIV_SYSTEM_YM2610_FULL_EXT:
      case DIV_SYSTEM_YM2610B_EXT:
        for (int i=0; i<2; i++) { // set SL and RR to highest
          w->writeC(8|baseAddr1);
          w->writeC(0x81+i);
          w->writeC(0xff);
          w->writeC(8|baseAddr1);
          w->writeC(0x85+i);
          w->writeC(0xff);
          w->writeC(8|baseAddr1);
          w->writeC(0x89+i);
          w->writeC(0xff);
          w->writeC(8|baseAddr1);
          w->writeC(0x8d+i);
          w->writeC(0xff);

          w->writeC(9|baseAddr1);
          w->writeC(0x81+i);
          w->writeC(0xff);
          w->writeC(9|baseAddr1);
          w->writeC(0x85+i);
          w->writeC(0xff);
          w->writeC(9|baseAddr1);
          w->writeC(0x89+i);
          w->writeC(0xff);
          w->writeC(9|baseAddr1);
          w->writeC(0x8d+i);
          w->writeC(0xff);
        }
        for (int i=0; i<2; i++) { // note off
          w->writeC(8|baseAddr1);
          w->writeC(0x28);
          w->writeC(1+i);
          w->writeC(8|baseAddr1);
          w->writeC(0x28);
          w->writeC(5+i);
        }
        
        // reset AY
        w->writeC(8|baseAddr1);
        w->writeC(7);
        w->writeC(0x3f);

        w->writeC(8|baseAddr1);
        w->writeC(8);
        w->writeC(0);

        w->writeC(8|baseAddr1);
        w->writeC(9);
        w->writeC(0);

        w->writeC(8|baseAddr1);
        w->writeC(10);
        w->writeC(0);

        // reset sample
        w->writeC(9|baseAddr1);
        w->writeC(0);
        w->writeC(0xbf);
        break;
      case DIV_SYSTEM_OPLL:
      case DIV_SYSTEM_OPLL_DRUMS:
      case DIV_SYSTEM_VRC7:
        for (int i=0; i<9; i++) {
          w->writeC(1|baseAddr1);
          w->writeC(0x20+i);
          w->writeC(0);
          w->writeC(1|baseAddr1);
          w->writeC(0x30+i);
          w->writeC(0);
          w->writeC(1|baseAddr1);
          w->writeC(0x10+i);
          w->writeC(0);
        }
        break;
      case DIV_SYSTEM_AY8910:
        w->writeC(0xa0);
        w->writeC(7|baseAddr2);
        w->writeC(0x3f);

        w->writeC(0xa0);
        w->writeC(8|baseAddr2);
        w->writeC(0);

        w->writeC(0xa0);
        w->writeC(9|baseAddr2);
        w->writeC(0);

        w->writeC(0xa0);
        w->writeC(10|baseAddr2);
        w->writeC(0);
        break;
      case DIV_SYSTEM_AY8930:
        w->writeC(0xa0);
        w->writeC(0x0d|baseAddr2);
        w->writeC(0);
        w->writeC(0xa0);
        w->writeC(0x0d|baseAddr2);
        w->writeC(0xa0);
        break;
      case DIV_SYSTEM_SAA1099:
        w->writeC(0xbd);
        w->writeC(0x1c|baseAddr2);
        w->writeC(0x02);
        w->writeC(0xbd);
        w->writeC(0x14|baseAddr2);
        w->writeC(0);
        w->writeC(0xbd);
        w->writeC(0x15|baseAddr2);
        w->writeC(0);

        for (int i=0; i<6; i++) {
          w->writeC(0xbd);
          w->writeC((0|baseAddr2)+i);
          w->writeC(0);
        }
        break;
      case DIV_SYSTEM_LYNX:
        w->writeC(0x4e);
        w->writeC(0x44);
        w->writeC(0xff); //stereo attenuation select
        w->writeC(0x4e);
        w->writeC(0x50);
        w->writeC(0x00); //stereo channel disable
        for (int i=0; i<4; i++) { //stereo attenuation value
          w->writeC(0x4e);
          w->writeC(0x40+i);
          w->writeC(0xff);
        }
        break;
      case DIV_SYSTEM_QSOUND:
        for (int i=0; i<16; i++) {
          w->writeC(0xc4);
          w->writeC(0);
          w->writeC(0);
          w->writeC(2+(i*8));
          w->writeC(0xc4);
          w->writeC(0);
          w->writeC(0);
          w->writeC(6+(i*8));
        }
        for (int i=0; i<3; i++) {
          w->writeC(0xc4);
          w->writeC(0);
          w->writeC(0);
          w->writeC(0xcd+(i*4));
          w->writeC(0xc4);
          w->writeC(0x00);
          w->writeC(0x01);
          w->writeC(0xd6+i);
        }
        break;
      // TODO: it's 3:35am
      case DIV_SYSTEM_OPL:
      case DIV_SYSTEM_OPL_DRUMS:
        // disable envelope
        for (int i=0; i<6; i++) {
          w->writeC(0x0b|baseAddr1);
          w->writeC(0x80+i);
          w->writeC(0x0f);
          w->writeC(0x0b|baseAddr1);
          w->writeC(0x88+i);
          w->writeC(0x0f);
          w->writeC(0x0b|baseAddr1);
          w->writeC(0x90+i);
          w->writeC(0x0f);
        }
        // key off + freq reset
        for (int i=0; i<9; i++) {
          w->writeC(0x0b|baseAddr1);
          w->writeC(0xa0+i);
          w->writeC(0);
          w->writeC(0x0b|baseAddr1);
          w->writeC(0xb0+i);
          w->writeC(0);
        }
        break;
      case DIV_SYSTEM_OPL2:
      case DIV_SYSTEM_OPL2_DRUMS:
        // disable envelope
        for (int i=0; i<6; i++) {
          w->writeC(0x0a|baseAddr1);
          w->writeC(0x80+i);
          w->writeC(0x0f);
          w->writeC(0x0a|baseAddr1);
          w->writeC(0x88+i);
          w->writeC(0x0f);
          w->writeC(0x0a|baseAddr1);
          w->writeC(0x90+i);
          w->writeC(0x0f);
        }
        // key off + freq reset
        for (int i=0; i<9; i++) {
          w->writeC(0x0a|baseAddr1);
          w->writeC(0xa0+i);
          w->writeC(0);
          w->writeC(0x0a|baseAddr1);
          w->writeC(0xb0+i);
          w->writeC(0);
        }
        break;
      case DIV_SYSTEM_OPL3:
      case DIV_SYSTEM_OPL3_DRUMS:
        // disable envelope
        for (int i=0; i<6; i++) {
          w->writeC(0x0e|baseAddr1);
          w->writeC(0x80+i);
          w->writeC(0x0f);
          w->writeC(0x0e|baseAddr1);
          w->writeC(0x88+i);
          w->writeC(0x0f);
          w->writeC(0x0e|baseAddr1);
          w->writeC(0x90+i);
          w->writeC(0x0f);
          w->writeC(0x0f|baseAddr1);
          w->writeC(0x80+i);
          w->writeC(0x0f);
          w->writeC(0x0f|baseAddr1);
          w->writeC(0x88+i);
          w->writeC(0x0f);
          w->writeC(0x0f|baseAddr1);
          w->writeC(0x90+i);
          w->writeC(0x0f);
        }
        // key off + freq reset
        for (int i=0; i<9; i++) {
          w->writeC(0x0e|baseAddr1);
          w->writeC(0xa0+i);
          w->writeC(0);
          w->writeC(0x0e|baseAddr1);
          w->writeC(0xb0+i);
          w->writeC(0);
          w->writeC(0x0f|baseAddr1);
          w->writeC(0xa0+i);
          w->writeC(0);
          w->writeC(0x0f|baseAddr1);
          w->writeC(0xb0+i);
          w->writeC(0);
        }
        // reset 4-op
        w->writeC(0x0f|baseAddr1);
        w->writeC(0x04);
        w->writeC(0x00);
        break;
      default:
        break;
    }
  }
  if (write.addr>=0xffff0000) { // Furnace special command
    unsigned char streamID=streamOff+((write.addr&0xff00)>>8);
    logD("writing stream command %x:%x with stream ID %d",write.addr,write.val,streamID);
    switch (write.addr&0xff) {
      case 0: // play sample
        if (write.val<song.sampleLen) {
          DivSample* sample=song.sample[write.val];
          w->writeC(0x95);
          w->writeC(streamID);
          w->writeS(write.val); // sample number
          w->writeC((sample->loopStart==0)); // flags
          if (sample->loopStart>0) {
            loopTimer[streamID]=sample->length8;
            loopSample[streamID]=write.val;
          }
        }
        break;
      case 1: // set sample freq
        w->writeC(0x92);
        w->writeC(streamID);
        w->writeI(write.val);
        loopFreq[streamID]=write.val;
        break;
      case 2: // stop sample
        w->writeC(0x94);
        w->writeC(streamID);
        loopSample[streamID]=-1;
        break;
    }
    return;
  }
  switch (sys) {
    case DIV_SYSTEM_YM2612:
    case DIV_SYSTEM_YM2612_EXT:
      switch (write.addr>>8) {
        case 0: // port 0
          w->writeC(2|baseAddr1);
          w->writeC(write.addr&0xff);
          w->writeC(write.val);
          break;
        case 1: // port 1
          w->writeC(3|baseAddr1);
          w->writeC(write.addr&0xff);
          w->writeC(write.val);
          break;
        case 2: // PSG
          w->writeC(smsAddr);
          w->writeC(write.val);
          break;
      }
      break;
    case DIV_SYSTEM_SMS:
      w->writeC(smsAddr);
      w->writeC(write.val);
      break;
    case DIV_SYSTEM_GB:
      w->writeC(0xb3);
      w->writeC(baseAddr2|((write.addr-16)&0xff));
      w->writeC(write.val);
      break;
    case DIV_SYSTEM_PCE:
      w->writeC(0xb9);
      w->writeC(baseAddr2|(write.addr&0xff));
      w->writeC(write.val);
      break;
    case DIV_SYSTEM_NES:
      w->writeC(0xb4);
      w->writeC(baseAddr2|(write.addr&0xff));
      w->writeC(write.val);
      break;
    case DIV_SYSTEM_FDS: // yeah
      w->writeC(0xb4);
      if ((write.addr&0xff)==0x23) {
        w->writeC(baseAddr2|0x3f);
      } else if ((write.addr&0xff)>=0x80) {
        w->writeC(baseAddr2|(0x20+(write.addr&0x7f)));
      } else {
        w->writeC(baseAddr2|(write.addr&0xff));
      }
      
      w->writeC(write.val);
      break;
    case DIV_SYSTEM_YM2151:
      w->writeC(4|baseAddr1);
      w->writeC(write.addr&0xff);
      w->writeC(write.val);
      break;
    case DIV_SYSTEM_SEGAPCM:
    case DIV_SYSTEM_SEGAPCM_COMPAT:
      w->writeC(0xc0);
      w->writeS(baseAddr2S|(write.addr&0xffff));
      w->writeC(write.val);
      break;
    case DIV_SYSTEM_X1_010:
      w->writeC(0xc8);
      w->writeS_BE(baseAddr2S|(write.addr&0x1fff));
      w->writeC(write.val);
      break;
    case DIV_SYSTEM_YM2610:
    case DIV_SYSTEM_YM2610_FULL:
    case DIV_SYSTEM_YM2610B:
    case DIV_SYSTEM_YM2610_EXT:
    case DIV_SYSTEM_YM2610_FULL_EXT:
    case DIV_SYSTEM_YM2610B_EXT:
      switch (write.addr>>8) {
        case 0: // port 0
          w->writeC(8|baseAddr1);
          w->writeC(write.addr&0xff);
          w->writeC(write.val);
          break;
        case 1: // port 1
          w->writeC(9|baseAddr1);
          w->writeC(write.addr&0xff);
          w->writeC(write.val);
          break;
      }
      break;
    case DIV_SYSTEM_OPLL:
    case DIV_SYSTEM_OPLL_DRUMS:
    case DIV_SYSTEM_VRC7:
      w->writeC(1|baseAddr1);
      w->writeC(write.addr&0xff);
      w->writeC(write.val);
      break;
    case DIV_SYSTEM_AY8910:
    case DIV_SYSTEM_AY8930:
      w->writeC(0xa0);
      w->writeC(baseAddr2|(write.addr&0xff));
      w->writeC(write.val);
      break;
    case DIV_SYSTEM_SAA1099:
      w->writeC(0xbd);
      w->writeC(baseAddr2|(write.addr&0xff));
      w->writeC(write.val);
      break;
    case DIV_SYSTEM_LYNX:
      w->writeC(0x4e);
      w->writeC(write.addr&0xff);
      w->writeC(write.val&0xff);
      break;
    case DIV_SYSTEM_QSOUND:
      w->writeC(0xc4);
      w->writeC((write.val>>8)&0xff);
      w->writeC(write.val&0xff);
      w->writeC(write.addr&0xff);
      break;
    case DIV_SYSTEM_SWAN:
      if ((write.addr&0x7f)<0x40) {
        w->writeC(0xbc);
        w->writeC(baseAddr2|(write.addr&0x3f));
        w->writeC(write.val&0xff);
      } else {
        // (Wave) RAM write
        w->writeC(0xc6);
        w->writeS_BE(baseAddr2S|(write.addr&0x3f));
        w->writeC(write.val&0xff);
      }
      break;
    case DIV_SYSTEM_OPL:
    case DIV_SYSTEM_OPL_DRUMS:
      w->writeC(0x0b|baseAddr1);
      w->writeC(write.addr&0xff);
      w->writeC(write.val);
      break;
    case DIV_SYSTEM_OPL2:
    case DIV_SYSTEM_OPL2_DRUMS:
      w->writeC(0x0a|baseAddr1);
      w->writeC(write.addr&0xff);
      w->writeC(write.val);
      break;
    case DIV_SYSTEM_OPL3:
    case DIV_SYSTEM_OPL3_DRUMS:
      switch (write.addr>>8) {
        case 0: // port 0
          w->writeC(0x0e|baseAddr1);
          w->writeC(write.addr&0xff);
          w->writeC(write.val);
          break;
        case 1: // port 1
          w->writeC(0x0f|baseAddr1);
          w->writeC(write.addr&0xff);
          w->writeC(write.val);
          break;
      }
      break;
    default:
      logW("write not handled!");
      break;
  }
}

SafeWriter* DivEngine::saveVGM(bool* sysToExport, bool loop, int version) {
  if (version<0x150) {
    lastError="VGM version is too low";
    return NULL;
  }
  stop();
  repeatPattern=false;
  setOrder(0);
  BUSY_BEGIN_SOFT;
  double origRate=got.rate;
  got.rate=44100;
  // determine loop point
  int loopOrder=0;
  int loopRow=0;
  int loopEnd=0;
  walkSong(loopOrder,loopRow,loopEnd);
  logI("loop point: %d %d",loopOrder,loopRow);
  warnings="";

  curOrder=0;
  freelance=false;
  playing=false;
  extValuePresent=false;
  remainingLoops=-1;

  // play the song ourselves
  bool done=false;
  int writeCount=0;

  int gd3Off=0;

  int hasSN=0;
  int snNoiseConfig=9;
  int snNoiseSize=16;
  int snFlags=0;
  int hasOPLL=0;
  int hasOPN2=0;
  int hasOPM=0;
  int hasSegaPCM=0;
  int segaPCMOffset=0xf8000d;
  int hasRFC=0;
  int hasOPN=0;
  int hasOPNA=0;
  int hasOPNB=0;
  int hasOPL2=0;
  int hasOPL=0;
  int hasY8950=0;
  int hasOPL3=0;
  int hasOPL4=0;
  int hasOPX=0;
  int hasZ280=0;
  int hasRFC1=0;
  int hasPWM=0;
  int hasAY=0;
  int ayConfig=0;
  int ayFlags=0;
  int hasGB=0;
  int hasNES=0;
  int hasMultiPCM=0;
  int hasuPD7759=0;
  int hasOKIM6258=0;
  int hasK054539=0;
  int hasOKIM6295=0;
  int hasK051649=0;
  int hasPCE=0;
  int hasNamco=0;
  int hasK053260=0;
  int hasPOKEY=0;
  int hasQSound=0;
  int hasSCSP=0;
  int hasSwan=0;
  int hasVSU=0;
  int hasSAA=0;
  int hasES5503=0;
  int hasES5505=0;
  int hasX1=0;
  int hasC352=0;
  int hasGA20=0;
  int hasLynx=0;

  int howManyChips=0;

  int loopPos=-1;
  int loopTick=-1;

  SafeWriter* w=new SafeWriter;
  w->init();

  // write header
  w->write("Vgm ",4);
  w->writeI(0); // will be written later
  w->writeI(version);

  bool willExport[32];
  bool isSecond[32];
  int streamIDs[32];
  double loopTimer[DIV_MAX_CHANS];
  double loopFreq[DIV_MAX_CHANS];
  int loopSample[DIV_MAX_CHANS];

  for (int i=0; i<DIV_MAX_CHANS; i++) {
    loopTimer[i]=0;
    loopFreq[i]=0;
    loopSample[i]=-1;
  }

  bool writeDACSamples=false;
  bool writeNESSamples=false;
  bool writePCESamples=false;
  int writeADPCM=0;
  int writeSegaPCM=0;
  int writeX1010=0;
  int writeQSound=0;

  for (int i=0; i<song.systemLen; i++) {
    willExport[i]=false;
    isSecond[i]=false;
    streamIDs[i]=0;
    if (sysToExport!=NULL) {
      if (!sysToExport[i]) continue;
    }
    if (minVGMVersion(song.system[i])>version) continue;
    switch (song.system[i]) {
      case DIV_SYSTEM_SMS:
        if (!hasSN) {
          hasSN=disCont[i].dispatch->chipClock;
          willExport[i]=true;
          switch ((song.systemFlags[i]>>2)&3) {
            case 1: // real SN
              snNoiseConfig=3;
              snNoiseSize=15;
              break;
            case 2: // real SN atari bass (seemingly unsupported)
              snNoiseConfig=3;
              snNoiseSize=15;
              break;
            default: // Sega VDP
              snNoiseConfig=9;
              snNoiseSize=16;
              break;
          }
        } else if (!(hasSN&0x40000000)) {
          isSecond[i]=true;
          willExport[i]=true;
          hasSN|=0x40000000;
          howManyChips++;
        }
        break;
      case DIV_SYSTEM_GB:
        if (!hasGB) {
          hasGB=disCont[i].dispatch->chipClock;
          willExport[i]=true;
        } else if (!(hasGB&0x40000000)) {
          isSecond[i]=true;
          willExport[i]=true;
          hasGB|=0x40000000;
          howManyChips++;
        }
        break;
      case DIV_SYSTEM_PCE:
        if (!hasPCE) {
          hasPCE=disCont[i].dispatch->chipClock;
          willExport[i]=true;
          writePCESamples=true;
        } else if (!(hasPCE&0x40000000)) {
          isSecond[i]=true;
          willExport[i]=true;
          hasPCE|=0x40000000;
          howManyChips++;
        }
        break;
      case DIV_SYSTEM_NES:
        if (!hasNES) {
          hasNES=disCont[i].dispatch->chipClock;
          willExport[i]=true;
          writeNESSamples=true;
        } else if (!(hasNES&0x40000000)) {
          isSecond[i]=true;
          willExport[i]=true;
          hasNES|=0x40000000;
          howManyChips++;
        }
        break;
      case DIV_SYSTEM_SEGAPCM:
      case DIV_SYSTEM_SEGAPCM_COMPAT:
        if (!hasSegaPCM) {
          hasSegaPCM=4000000;
          willExport[i]=true;
          writeSegaPCM=1;
        } else if (!(hasSegaPCM&0x40000000)) {
          isSecond[i]=true;
          willExport[i]=true;
          writeSegaPCM=2;
          hasSegaPCM|=0x40000000;
          howManyChips++;
        }
        break;
      case DIV_SYSTEM_X1_010:
        if (!hasX1) {
          hasX1=disCont[i].dispatch->chipClock;
          willExport[i]=true;
          writeX1010=1;
        } else if (!(hasX1&0x40000000)) {
          isSecond[i]=true;
          willExport[i]=true;
          writeX1010=2;
          hasX1|=0x40000000;
          howManyChips++;
        }
        break;
      case DIV_SYSTEM_YM2610:
      case DIV_SYSTEM_YM2610_FULL:
      case DIV_SYSTEM_YM2610B:
      case DIV_SYSTEM_YM2610_EXT:
      case DIV_SYSTEM_YM2610_FULL_EXT:
      case DIV_SYSTEM_YM2610B_EXT:
        if (!hasOPNB) {
          hasOPNB=disCont[i].dispatch->chipClock;
          willExport[i]=true;
          writeADPCM=1;
        } else if (!(hasOPNB&0x40000000)) {
          isSecond[i]=true;
          willExport[i]=true;
          writeADPCM=2;
          hasOPNB|=0x40000000;
          howManyChips++;
        }
        if (((song.system[i]==DIV_SYSTEM_YM2610B) || (song.system[i]==DIV_SYSTEM_YM2610B_EXT)) && (!(hasOPNB&0x80000000))) { // YM2610B flag
          hasOPNB|=0x80000000;
        }
        break;
      case DIV_SYSTEM_AY8910:
      case DIV_SYSTEM_AY8930:
        if (!hasAY) {
          hasAY=disCont[i].dispatch->chipClock;
          ayConfig=(song.system[i]==DIV_SYSTEM_AY8930)?3:0;
          ayFlags=1;
          willExport[i]=true;
        } else if (!(hasAY&0x40000000)) {
          isSecond[i]=true;
          willExport[i]=true;
          hasAY|=0x40000000;
          howManyChips++;
        }
        break;
      case DIV_SYSTEM_SAA1099:
        if (!hasSAA) {
          hasSAA=disCont[i].dispatch->chipClock;
          willExport[i]=true;
        } else if (!(hasSAA&0x40000000)) {
          isSecond[i]=true;
          willExport[i]=true;
          hasSAA|=0x40000000;
          howManyChips++;
        }
        break;
      case DIV_SYSTEM_YM2612:
      case DIV_SYSTEM_YM2612_EXT:
        if (!hasOPN2) {
          hasOPN2=disCont[i].dispatch->chipClock;
          willExport[i]=true;
          writeDACSamples=true;
        } else if (!(hasOPN2&0x40000000)) {
          isSecond[i]=true;
          willExport[i]=true;
          hasOPN2|=0x40000000;
          howManyChips++;
        }
        break;
      case DIV_SYSTEM_YM2151:
        if (!hasOPM) {
          hasOPM=disCont[i].dispatch->chipClock;
          willExport[i]=true;
        } else if (!(hasOPM&0x40000000)) {
          isSecond[i]=true;
          willExport[i]=true;
          hasOPM|=0x40000000;
          howManyChips++;
        }
        break;
      case DIV_SYSTEM_OPLL:
      case DIV_SYSTEM_OPLL_DRUMS:
      case DIV_SYSTEM_VRC7:
        if (!hasOPLL) {
          hasOPLL=disCont[i].dispatch->chipClock;
          willExport[i]=true;
        } else if (!(hasOPLL&0x40000000)) {
          isSecond[i]=true;
          willExport[i]=true;
          hasOPLL|=0x40000000;
          howManyChips++;
        }
        break;
      case DIV_SYSTEM_FDS:
        if (!hasNES) {
          hasNES=0x80000000|disCont[i].dispatch->chipClock;
          willExport[i]=true;
        } else if (!(hasNES&0x80000000)) {
          hasNES|=0x80000000;
          willExport[i]=true;
        } else if (!(hasNES&0x40000000)) {
          isSecond[i]=true;
          willExport[i]=true;
          hasNES|=0xc0000000;
          howManyChips++;
        }
        break;
      case DIV_SYSTEM_LYNX:
        if (!hasLynx) {
          hasLynx=disCont[i].dispatch->chipClock;
          willExport[i]=true;
        } else if (!(hasLynx&0x40000000)) {
          isSecond[i]=true;
          willExport[i]=true;
          hasLynx|=0x40000000;
          howManyChips++;
        }
        break;
      case DIV_SYSTEM_QSOUND:
        if (!hasQSound) {
          // could set chipClock to 4000000 here for compatibility
          // However I think it it not necessary because old VGM players will still
          // not be able to handle the 64kb sample bank trick
          hasQSound=disCont[i].dispatch->chipClock;
          willExport[i]=true;
          writeQSound=1;
        } else if (!(hasQSound&0x40000000)) {
          isSecond[i]=true;
          willExport[i]=false;
          writeQSound=2;
          addWarning("dual QSound is not supported by the VGM format");
        }
        break;
      case DIV_SYSTEM_SWAN:
        if (!hasSwan) {
          hasSwan=disCont[i].dispatch->chipClock;
          willExport[i]=true;
          // funny enough, VGM doesn't have support for WSC's sound DMA by design
          // so DAC stream it goes
          // since WS has the same PCM format as YM2612 DAC, I can just reuse this flag
          writeDACSamples=true;
        } else if (!(hasSwan&0x40000000)) {
          isSecond[i]=true;
          willExport[i]=true;
          hasSwan|=0x40000000;
          howManyChips++;
        }
        break;
      case DIV_SYSTEM_OPL:
      case DIV_SYSTEM_OPL_DRUMS:
        if (!hasOPL) {
          hasOPL=disCont[i].dispatch->chipClock;
          willExport[i]=true;
        } else if (!(hasOPL&0x40000000)) {
          isSecond[i]=true;
          willExport[i]=true;
          hasOPL|=0x40000000;
          howManyChips++;
        }
        break;
      case DIV_SYSTEM_OPL2:
      case DIV_SYSTEM_OPL2_DRUMS:
        if (!hasOPL2) {
          hasOPL2=disCont[i].dispatch->chipClock;
          willExport[i]=true;
        } else if (!(hasOPL2&0x40000000)) {
          isSecond[i]=true;
          willExport[i]=true;
          hasOPL2|=0x40000000;
          howManyChips++;
        }
        break;
      case DIV_SYSTEM_OPL3:
      case DIV_SYSTEM_OPL3_DRUMS:
        if (!hasOPL3) {
          hasOPL3=disCont[i].dispatch->chipClock;
          willExport[i]=true;
        } else if (!(hasOPL3&0x40000000)) {
          isSecond[i]=true;
          willExport[i]=true;
          hasOPL3|=0x40000000;
          howManyChips++;
        }
        break;
      default:
        break;
    }
    if (willExport[i]) {
      disCont[i].dispatch->toggleRegisterDump(true);
    }
  }

  //bool wantsExtraHeader=false;
  /*for (int i=0; i<song.systemLen; i++) {
    if (isSecond[i]) {
      wantsExtraHeader=true;
      break;
    }
  }*/

  // write chips and stuff
  w->writeI(hasSN);
  w->writeI(hasOPLL);
  w->writeI(0);
  w->writeI(0); // length. will be written later
  w->writeI(0); // loop. will be written later
  w->writeI(0); // loop length. why is this necessary?
  w->writeI(0); // tick rate
  w->writeS(snNoiseConfig);
  w->writeC(snNoiseSize);
  if (version>=0x151) {
    w->writeC(snFlags);
  } else {
    w->writeC(0);
  }
  w->writeI(hasOPN2);
  w->writeI(hasOPM);
  w->writeI(0); // data pointer. will be written later
  if (version>=0x151) {
    w->writeI(hasSegaPCM);
    w->writeI(segaPCMOffset);
    w->writeI(hasRFC);
    w->writeI(hasOPN);
    w->writeI(hasOPNA);
    w->writeI(hasOPNB);
    w->writeI(hasOPL2);
    w->writeI(hasOPL);
    w->writeI(hasY8950);
    w->writeI(hasOPL3);
    w->writeI(hasOPL4);
    w->writeI(hasOPX);
    w->writeI(hasZ280);
    w->writeI(hasRFC1);
    w->writeI(hasPWM);
    w->writeI(hasAY);
    w->writeC(ayConfig);
    w->writeC(ayFlags);
    w->writeC(ayFlags); // OPN
    w->writeC(ayFlags); // OPNA
  } else {
    w->writeI(0);
    w->writeI(0);
    w->writeI(0);
    w->writeI(0);
    w->writeI(0);
    w->writeI(0);
    w->writeI(0);
    w->writeI(0);
    w->writeI(0);
    w->writeI(0);
    w->writeI(0);
    w->writeI(0);
    w->writeI(0);
    w->writeI(0);
    w->writeI(0);
    w->writeI(0);
    w->writeC(0);
    w->writeC(0);
    w->writeC(0); // OPN
    w->writeC(0); // OPNA
  }
  // currently not used but is part of 1.60
  w->writeC(0); // volume
  w->writeC(0); // reserved
  w->writeC(0); // loop count
  // 1.51
  w->writeC(0); // loop modifier
  
  if (version>=0x161) {
    w->writeI(hasGB);
    w->writeI(hasNES);
    w->writeI(hasMultiPCM);
    w->writeI(hasuPD7759);
    w->writeI(hasOKIM6258);
    w->writeC(0); // flags
    w->writeC(0); // K flags
    w->writeC(0); // C140 chip type
    w->writeC(0); // reserved
    w->writeI(hasOKIM6295);
    w->writeI(hasK051649);
    w->writeI(hasK054539);
    w->writeI(hasPCE);
    w->writeI(hasNamco);
    w->writeI(hasK053260);
    w->writeI(hasPOKEY);
    w->writeI(hasQSound);
  } else {
    w->writeI(0);
    w->writeI(0);
    w->writeI(0);
    w->writeI(0);
    w->writeI(0);
    w->writeC(0); // flags
    w->writeC(0); // K flags
    w->writeC(0); // C140 chip type
    w->writeC(0); // reserved
    w->writeI(0);
    w->writeI(0);
    w->writeI(0);
    w->writeI(0);
    w->writeI(0);
    w->writeI(0);
    w->writeI(0);
    w->writeI(0);
  }
  if (version>=0x171) {
    w->writeI(hasSCSP);
  } else {
    w->writeI(0);
  }
  // 1.70
  w->writeI(0); // extra header
  // 1.71
  if (version>=0x171) {
    w->writeI(hasSwan);
    w->writeI(hasVSU);
    w->writeI(hasSAA);
    w->writeI(hasES5503);
    w->writeI(hasES5505);
    w->writeC(0); // 5503 chans
    w->writeC(0); // 5505 chans
    w->writeC(0); // C352 clock divider
    w->writeC(0); // reserved
    w->writeI(hasX1);
    w->writeI(hasC352);
    w->writeI(hasGA20);
    w->writeI(hasLynx);
  } else {
    w->writeI(0);
    w->writeI(0);
    w->writeI(0);
    w->writeI(0);
    w->writeI(0);
    w->writeC(0); // 5503 chans
    w->writeC(0); // 5505 chans
    w->writeC(0); // C352 clock divider
    w->writeC(0); // reserved
    w->writeI(0);
    w->writeI(0);
    w->writeI(0);
    w->writeI(0);
  }
  for (int i=0; i<6; i++) { // reserved
    w->writeI(0);
  }

  /* TODO
  unsigned int exHeaderOff=w->tell();
  if (wantsExtraHeader) {
    w->writeI(4);
    w->writeI(4);

    // write clocks
    w->writeC(howManyChips);
  }*/

  unsigned int songOff=w->tell();

  // write samples
  unsigned int sampleSeek=0;
  for (int i=0; i<song.sampleLen; i++) {
    DivSample* sample=song.sample[i];
    logI("setting seek to %d",sampleSeek);
    sample->off8=sampleSeek;
    sampleSeek+=sample->length8;
  }

  if (writeDACSamples) for (int i=0; i<song.sampleLen; i++) {
    DivSample* sample=song.sample[i];
    w->writeC(0x67);
    w->writeC(0x66);
    w->writeC(0);
    w->writeI(sample->length8);
    for (unsigned int j=0; j<sample->length8; j++) {
      w->writeC((unsigned char)sample->data8[j]+0x80);
    }
  }

  if (writeNESSamples) for (int i=0; i<song.sampleLen; i++) {
    DivSample* sample=song.sample[i];
    w->writeC(0x67);
    w->writeC(0x66);
    w->writeC(7);
    w->writeI(sample->length8);
    for (unsigned int j=0; j<sample->length8; j++) {
      w->writeC(((unsigned char)sample->data8[j]+0x80)>>1);
    }
  }

  if (writePCESamples) for (int i=0; i<song.sampleLen; i++) {
    DivSample* sample=song.sample[i];
    w->writeC(0x67);
    w->writeC(0x66);
    w->writeC(5);
    w->writeI(sample->length8);
    for (unsigned int j=0; j<sample->length8; j++) {
      w->writeC(((unsigned char)sample->data8[j]+0x80)>>3);
    }
  }

  if (writeSegaPCM>0) {
    unsigned char* pcmMem=new unsigned char[16777216];

    size_t memPos=0;
    for (int i=0; i<song.sampleLen; i++) {
      DivSample* sample=song.sample[i];
      if ((memPos&0xff0000)!=((memPos+sample->length8)&0xff0000)) {
        memPos=(memPos+0xffff)&0xff0000;
      }
      if (memPos>=16777216) break;
      sample->offSegaPCM=memPos;
      unsigned int alignedSize=(sample->length8+0xff)&(~0xff);
      unsigned int readPos=0;
      if (alignedSize>65536) alignedSize=65536;
      for (unsigned int j=0; j<alignedSize; j++) {
        if (readPos>=sample->length8) {
          if (sample->loopStart>=0 && sample->loopStart<(int)sample->length8) {
            readPos=sample->loopStart;
            pcmMem[memPos++]=((unsigned char)sample->data8[readPos]+0x80);
          } else {
            pcmMem[memPos++]=0x80;
          }
        } else {
          pcmMem[memPos++]=((unsigned char)sample->data8[readPos]+0x80);
        }
        readPos++;
        if (memPos>=16777216) break;
      }
      sample->loopOffP=readPos-sample->loopStart;
      if (memPos>=16777216) break;
    }

    for (int i=0; i<writeSegaPCM; i++) {
      w->writeC(0x67);
      w->writeC(0x66);
      w->writeC(0x80);
      w->writeI((memPos+8)|(i*0x80000000));
      w->writeI(memPos);
      w->writeI(0);
      w->write(pcmMem,memPos);
    }

    delete[] pcmMem;
  }

  if (adpcmAMemLen>0) {
    for (int i=0; i<writeADPCM; i++) {
      w->writeC(0x67);
      w->writeC(0x66);
      w->writeC(0x82);
      w->writeI((adpcmAMemLen+8)|(i*0x80000000));
      w->writeI(adpcmAMemLen);
      w->writeI(0);
      w->write(adpcmAMem,adpcmAMemLen);
    }
  }

  if (adpcmBMemLen>0) {
    for (int i=0; i<writeADPCM; i++) {
      w->writeC(0x67);
      w->writeC(0x66);
      w->writeC(0x83);
      w->writeI((adpcmBMemLen+8)|(i*0x80000000));
      w->writeI(adpcmBMemLen);
      w->writeI(0);
      w->write(adpcmBMem,adpcmBMemLen);
    }
  }

  if (qsoundMemLen>0) {
    // always write a whole bank
    unsigned int blockSize=(qsoundMemLen+0xffff)&(~0xffff);
    if (blockSize > 0x1000000) {
      blockSize = 0x1000000;
    }
    for (int i=0; i<writeQSound; i++) {
      w->writeC(0x67);
      w->writeC(0x66);
      w->writeC(0x8F);
      w->writeI((blockSize+8)|(i*0x80000000));
      w->writeI(0x1000000);
      w->writeI(0);
      w->write(qsoundMem,blockSize);
    }
  }

  if (x1_010MemLen>0) {
    for (int i=0; i<writeX1010; i++) {
      w->writeC(0x67);
      w->writeC(0x66);
      w->writeC(0x91);
      w->writeI((x1_010MemLen+8)|(i*0x80000000));
      w->writeI(x1_010MemLen);
      w->writeI(0);
      w->write(x1_010Mem,x1_010MemLen);
    }
  }

  // initialize streams
  int streamID=0;
  for (int i=0; i<song.systemLen; i++) {
    if (!willExport[i]) continue;
    streamIDs[i]=streamID;
    switch (song.system[i]) {
      case DIV_SYSTEM_YM2612:
      case DIV_SYSTEM_YM2612_EXT:
        w->writeC(0x90);
        w->writeC(streamID);
        w->writeC(0x02);
        w->writeC(0); // port
        w->writeC(0x2a); // DAC

        w->writeC(0x91);
        w->writeC(streamID);
        w->writeC(0);
        w->writeC(1);
        w->writeC(0);

        w->writeC(0x92);
        w->writeC(streamID);
        w->writeI(32000); // default
        streamID++;
        break;
      case DIV_SYSTEM_NES:
        w->writeC(0x90);
        w->writeC(streamID);
        w->writeC(20);
        w->writeC(0); // port
        w->writeC(0x11); // DAC

        w->writeC(0x91);
        w->writeC(streamID);
        w->writeC(7);
        w->writeC(1);
        w->writeC(0);

        w->writeC(0x92);
        w->writeC(streamID);
        w->writeI(32000); // default
        streamID++;
        break;
      case DIV_SYSTEM_PCE:
        for (int j=0; j<6; j++) {
          w->writeC(0x90);
          w->writeC(streamID);
          w->writeC(27);
          w->writeC(j); // port
          w->writeC(0x06); // select+DAC

          w->writeC(0x91);
          w->writeC(streamID);
          w->writeC(5);
          w->writeC(1);
          w->writeC(0);

          w->writeC(0x92);
          w->writeC(streamID);
          w->writeI(16000); // default
          streamID++;
        }
        break;
      case DIV_SYSTEM_SWAN:
        w->writeC(0x90);
        w->writeC(streamID);
        w->writeC(isSecond[i]?0xa1:0x21);
        w->writeC(0); // port
        w->writeC(0x09); // DAC

        w->writeC(0x91);
        w->writeC(streamID);
        w->writeC(0);
        w->writeC(1);
        w->writeC(0);

        w->writeC(0x92);
        w->writeC(streamID);
        w->writeI(24000); // default
        streamID++;
        break;
      default:
        break;
    }
  }

  // write song data
  playSub(false);
  size_t tickCount=0;
  bool writeLoop=false;
  while (!done) {
    if (loopPos==-1) {
      if (loopOrder==curOrder && loopRow==curRow && ticks==1) {
        writeLoop=true;
      }
    }
    if (nextTick(false,true) || !playing) {
      done=true;
      if (!loop) {
        for (int i=0; i<song.systemLen; i++) {
          disCont[i].dispatch->getRegisterWrites().clear();
        }
        break;
      }
      // stop all streams
      for (int i=0; i<streamID; i++) {
        w->writeC(0x94);
        w->writeC(i);
        loopSample[i]=-1;
      }

      if (!playing) {
        writeLoop=false;
        loopPos=-1;
      }
    }
    // get register dumps
    for (int i=0; i<song.systemLen; i++) {
      std::vector<DivRegWrite>& writes=disCont[i].dispatch->getRegisterWrites();
      for (DivRegWrite& j: writes) {
        performVGMWrite(w,song.system[i],j,streamIDs[i],loopTimer,loopFreq,loopSample,isSecond[i]);
        writeCount++;
      }
      writes.clear();
    }
    // check whether we need to loop
    int totalWait=cycles>>MASTER_CLOCK_PREC;
    for (int i=0; i<streamID; i++) {
      if (loopSample[i]>=0) {
        loopTimer[i]-=(loopFreq[i]/44100.0)*(double)totalWait;
      }
    }
    bool haveNegatives=false;
    for (int i=0; i<streamID; i++) {
      if (loopSample[i]>=0) {
        if (loopTimer[i]<0) {
          haveNegatives=true;
        }
      }
    }
    while (haveNegatives) {
      // finish all negatives
      int nextToTouch=-1;
      for (int i=0; i<streamID; i++) {
        if (loopSample[i]>=0) {
          if (loopTimer[i]<0) {
            if (nextToTouch>=0) {
              if (loopTimer[nextToTouch]>loopTimer[i]) nextToTouch=i;
            } else {
              nextToTouch=i;
            }
          }
        }
      }
      if (nextToTouch>=0) {
        double waitTime=totalWait+(loopTimer[nextToTouch]*(44100.0/MAX(1,loopFreq[nextToTouch])));
        if (waitTime>0) {
          w->writeC(0x61);
          w->writeS(waitTime);
          logV("wait is: %f",waitTime);
          totalWait-=waitTime;
          tickCount+=waitTime;
        }
        if (loopSample[nextToTouch]<song.sampleLen) {
          DivSample* sample=song.sample[loopSample[nextToTouch]];
          // insert loop
          if (sample->loopStart<(int)sample->length8) {
            w->writeC(0x93);
            w->writeC(nextToTouch);
            w->writeI(sample->off8+sample->loopStart);
            w->writeC(0x81);
            w->writeI(sample->length8-sample->loopStart);
          }
        }
        loopSample[nextToTouch]=-1;
      } else {
        haveNegatives=false;
      }
    }
    // write wait
    if (totalWait>0) {
      if (totalWait==735) {
        w->writeC(0x62);
      } else if (totalWait==882) {
        w->writeC(0x63);
      } else {
        w->writeC(0x61);
        w->writeS(totalWait);
      }
      tickCount+=totalWait;
    }
    if (writeLoop) {
      writeLoop=false;
      loopPos=w->tell();
      loopTick=tickCount;
    }
  }
  // end of song
  w->writeC(0x66);

  got.rate=origRate;

  for (int i=0; i<song.systemLen; i++) {
    disCont[i].dispatch->toggleRegisterDump(false);
  }

  // write GD3 tag
  gd3Off=w->tell();
  w->write("Gd3 ",4);
  w->writeI(0x100);
  w->writeI(0); // length. will be written later

  WString ws;
  ws=utf8To16(song.name.c_str());
  w->writeWString(ws,false); // name
  w->writeS(0); // japanese name
  w->writeS(0); // game name
  w->writeS(0); // japanese game name
  if (song.systemLen>1) {
    ws=L"Multiple Systems";
  } else {
    ws=utf8To16(getSystemName(song.system[0]));
  }
  w->writeWString(ws,false); // system name
  if (song.systemLen>1) {
    ws=L"複数システム";
  } else {
    ws=utf8To16(getSystemNameJ(song.system[0]));
  }
  w->writeWString(ws,false); // japanese system name
  ws=utf8To16(song.author.c_str());
  w->writeWString(ws,false); // author name
  w->writeS(0); // japanese author name
  w->writeS(0); // date
  w->writeWString(L"Furnace Tracker",false); // ripper
  w->writeS(0); // notes

  int gd3Len=w->tell()-gd3Off-12;

  w->seek(gd3Off+8,SEEK_SET);
  w->writeI(gd3Len);

  // finish file
  size_t len=w->size()-4;
  w->seek(4,SEEK_SET);
  w->writeI(len);
  w->seek(0x14,SEEK_SET);
  w->writeI(gd3Off-0x14);
  w->writeI(tickCount);
  if (loop) {
    if (loopPos==-1) {
      w->writeI(0);
      w->writeI(0);
    } else {
      w->writeI(loopPos-0x1c);
      w->writeI(tickCount-loopTick-1);
    }
  } else {
    w->writeI(0);
    w->writeI(0);
  }
  w->seek(0x34,SEEK_SET);
  w->writeI(songOff-0x34);
  /*if (wantsExtraHeader) {
    w->seek(0xbc,SEEK_SET);
    w->writeI(exHeaderOff-0xbc);
  }*/

  remainingLoops=-1;
  playing=false;
  freelance=false;
  extValuePresent=false;

  logI("%d register writes total.",writeCount);

  BUSY_END;
  return w;
}
