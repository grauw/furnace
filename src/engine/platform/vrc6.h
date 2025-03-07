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

#ifndef _VRC6_H
#define _VRC6_H

#include <queue>
#include "../dispatch.h"
#include "../macroInt.h"
#include "sound/vrcvi/vrcvi.hpp"


class DivPlatformVRC6: public DivDispatch {
  struct Channel {
    int freq, baseFreq, pitch, note;
    int dacPeriod, dacRate, dacOut;
    unsigned int dacPos;
    int dacSample;
    unsigned char ins, duty;
    bool active, insChanged, freqChanged, keyOn, keyOff, inPorta, pcm, furnaceDac;
    signed char vol, outVol;
    DivMacroInt std;
    Channel():
      freq(0),
      baseFreq(0),
      pitch(0),
      note(0),
      dacPeriod(0),
      dacRate(0),
      dacOut(0),
      dacPos(0),
      dacSample(-1),
      ins(-1),
      duty(0),
      active(false),
      insChanged(true),
      freqChanged(false),
      keyOn(false),
      keyOff(false),
      inPorta(false),
      pcm(false),
      furnaceDac(false),
      vol(15),
      outVol(15) {}
  };
  Channel chan[3];
  bool isMuted[3];
  struct QueuedWrite {
      unsigned short addr;
      unsigned char val;
      QueuedWrite(unsigned short a, unsigned char v): addr(a), val(v) {}
  };
  std::queue<QueuedWrite> writes;
  unsigned char sampleBank;
  vrcvi_intf intf;
  vrcvi_core vrc6;
  unsigned char regPool[13];

  friend void putDispatchChan(void*,int,int);

  public:
    void acquire(short* bufL, short* bufR, size_t start, size_t len);
    int dispatch(DivCommand c);
    void* getChanState(int chan);
    unsigned char* getRegisterPool();
    int getRegisterPoolSize();
    void reset();
    void forceIns();
    void tick(bool sysTick=true);
    void muteChannel(int ch, bool mute);
    bool keyOffAffectsArp(int ch);
    void setFlags(unsigned int flags);
    void notifyInsDeletion(void* ins);
    void poke(unsigned int addr, unsigned short val);
    void poke(std::vector<DivRegWrite>& wlist);
    const char** getRegisterSheet();
    const char* getEffectName(unsigned char effect);
    int init(DivEngine* parent, int channels, int sugRate, unsigned int flags);
    void quit();
    DivPlatformVRC6() : vrc6(intf) {};
    ~DivPlatformVRC6();
};

#endif
