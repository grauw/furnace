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

#ifndef _ARCADE_H
#define _ARCADE_H
#include "../dispatch.h"
#include "../instrument.h"
#include <queue>
#include "../../../extern/opm/opm.h"
#include "sound/ymfm/ymfm_opm.h"
#include "../macroInt.h"

class DivArcadeInterface: public ymfm::ymfm_interface {

};

class DivPlatformArcade: public DivDispatch {
  protected:
    struct Channel {
      DivInstrumentFM state;
      DivMacroInt std;
      unsigned char freqH, freqL;
      int freq, baseFreq, pitch, note;
      unsigned char ins;
      signed char konCycles;
      bool active, insChanged, freqChanged, keyOn, keyOff, inPorta, portaPause, furnacePCM, hardReset;
      int vol, outVol;
      unsigned char chVolL, chVolR;
      Channel():
        freqH(0),
        freqL(0),
        freq(0),
        baseFreq(0),
        pitch(0),
        note(0),
        ins(-1),
        active(false),
        insChanged(true),
        freqChanged(false),
        keyOn(false),
        keyOff(false),
        inPorta(false),
        portaPause(false),
        furnacePCM(false),
        hardReset(false),
        vol(0),
        outVol(0),
        chVolL(127),
        chVolR(127) {}
    };
    Channel chan[8];
    struct QueuedWrite {
      unsigned short addr;
      unsigned char val;
      bool addrOrVal;
      QueuedWrite(unsigned short a, unsigned char v): addr(a), val(v), addrOrVal(false) {}
    };
    std::queue<QueuedWrite> writes;
    opm_t fm;
    int delay, baseFreqOff;
    int pcmL, pcmR, pcmCycles;
    unsigned char lastBusy;
    unsigned char amDepth, pmDepth;

    ymfm::ym2151* fm_ymfm;
    ymfm::ym2151::output_data out_ymfm;
    DivArcadeInterface iface;

    unsigned char regPool[256];

    bool extMode, useYMFM;

    bool isMuted[8];
  
    short oldWrites[256];
    short pendingWrites[256];

    int octave(int freq);
    int toFreq(int freq);

    void acquire_nuked(short* bufL, short* bufR, size_t start, size_t len);
    void acquire_ymfm(short* bufL, short* bufR, size_t start, size_t len);
  
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
    void notifyInsChange(int ins);
    void setFlags(unsigned int flags);
    bool isStereo();
    void setYMFM(bool use);
    void poke(unsigned int addr, unsigned short val);
    void poke(std::vector<DivRegWrite>& wlist);
    const char** getRegisterSheet();
    const char* getEffectName(unsigned char effect);
    int init(DivEngine* parent, int channels, int sugRate, unsigned int flags);
    void quit();
    ~DivPlatformArcade();
};
#endif
