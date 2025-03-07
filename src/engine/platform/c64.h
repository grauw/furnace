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

#ifndef _C64_H
#define _C64_H

#include "../dispatch.h"
#include "../macroInt.h"
#include "sound/c64/sid.h"

class DivPlatformC64: public DivDispatch {
  struct Channel {
    int freq, baseFreq, pitch, prevFreq, testWhen, note;
    unsigned char ins, sweep, wave, attack, decay, sustain, release;
    short duty;
    bool active, insChanged, freqChanged, sweepChanged, keyOn, keyOff, inPorta, filter;
    bool resetMask, resetFilter, resetDuty, ring, sync;
    signed char vol, outVol;
    DivMacroInt std;
    Channel():
      freq(0),
      baseFreq(0),
      pitch(0),
      prevFreq(65535),
      testWhen(0),
      note(0),
      ins(-1),
      sweep(0),
      wave(0),
      attack(0),
      decay(0),
      sustain(0),
      release(0),
      duty(0),
      active(false),
      insChanged(true),
      freqChanged(false),
      sweepChanged(false),
      keyOn(false),
      keyOff(false),
      inPorta(false),
      filter(false),
      resetMask(false),
      resetFilter(false),
      resetDuty(false),
      ring(false),
      sync(false),
      vol(15) {}
  };
  Channel chan[3];
  bool isMuted[3];

  unsigned char filtControl, filtRes, vol;
  int filtCut, resetTime;

  SID sid;
  unsigned char regPool[32];

  friend void putDispatchChan(void*,int,int);

  void updateFilter();
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
    void setFlags(unsigned int flags);
    void notifyInsChange(int ins);
    bool getDCOffRequired();
    void notifyInsDeletion(void* ins);
    void poke(unsigned int addr, unsigned short val);
    void poke(std::vector<DivRegWrite>& wlist);
    const char** getRegisterSheet();
    const char* getEffectName(unsigned char effect);
    int init(DivEngine* parent, int channels, int sugRate, unsigned int flags);
    void setChipModel(bool is6581);
    void quit();
    ~DivPlatformC64();
};

#endif
