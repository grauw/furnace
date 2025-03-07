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

#ifndef _FDS_H
#define _FDS_H

#include "../dispatch.h"
#include "../macroInt.h"
#include "../waveSynth.h"

class DivPlatformFDS: public DivDispatch {
  struct Channel {
    int freq, baseFreq, pitch, prevFreq, note, modFreq;
    unsigned char ins, duty, sweep, modDepth, modPos;
    bool active, insChanged, freqChanged, sweepChanged, keyOn, keyOff, inPorta, modOn;
    signed char vol, outVol, wave;
    signed char modTable[32];
    DivMacroInt std;
    Channel():
      freq(0),
      baseFreq(0),
      pitch(0),
      prevFreq(65535),
      note(0),
      modFreq(0),
      ins(-1),
      duty(0),
      sweep(8),
      modDepth(0),
      modPos(0),
      active(false),
      insChanged(true),
      freqChanged(false),
      sweepChanged(false),
      keyOn(false),
      keyOff(false),
      inPorta(false),
      modOn(false),
      vol(32),
      outVol(32),
      wave(-1) {
      memset(modTable,0,32);
    }
  };
  Channel chan[1];
  bool isMuted[1];
  DivWaveSynth ws;
  unsigned char apuType;
  struct _fds* fds;
  unsigned char regPool[128];

  void updateWave();

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
    ~DivPlatformFDS();
};

#endif
