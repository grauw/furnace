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

#ifndef _SMS_H
#define _SMS_H

#include "../dispatch.h"
#include "../macroInt.h"
#include "sound/sn76496.h"

class DivPlatformSMS: public DivDispatch {
  struct Channel {
    int freq, baseFreq, pitch, note, actualNote;
    unsigned char ins;
    bool active, insChanged, freqChanged, keyOn, keyOff, inPorta;
    signed char vol, outVol;
    DivMacroInt std;
    Channel():
      freq(0),
      baseFreq(0),
      pitch(0),
      note(0),
      actualNote(0),
      ins(-1),
      active(false),
      insChanged(true),
      freqChanged(false),
      keyOn(false),
      keyOff(false),
      inPorta(false),
      vol(15),
      outVol(15) {}
  };
  Channel chan[4];
  bool isMuted[4];
  unsigned char oldValue; 
  unsigned char snNoiseMode;
  bool updateSNMode;
  bool resetPhase;
  bool isRealSN;
  sn76496_base_device* sn;
  friend void putDispatchChan(void*,int,int);
  public:
    int acquireOne();
    void acquire(short* bufL, short* bufR, size_t start, size_t len);
    int dispatch(DivCommand c);
    void* getChanState(int chan);
    void reset();
    void forceIns();
    void tick(bool sysTick=true);
    void muteChannel(int ch, bool mute);
    bool keyOffAffectsArp(int ch);
    bool keyOffAffectsPorta(int ch);
    int getPortaFloor(int ch);
    void setFlags(unsigned int flags);
    void notifyInsDeletion(void* ins);
    void poke(unsigned int addr, unsigned short val);
    void poke(std::vector<DivRegWrite>& wlist);
    const char** getRegisterSheet();
    const char* getEffectName(unsigned char effect);
    int init(DivEngine* parent, int channels, int sugRate, unsigned int flags);
    void quit();
    ~DivPlatformSMS();
};

#endif
