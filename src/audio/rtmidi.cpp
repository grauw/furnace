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

#include "rtmidi.h"
#include "../ta-log.h"
#include "taAudio.h"

// --- IN ---

bool TAMidiInRtMidi::gather() {
  std::vector<unsigned char> msg;
  if (port==NULL) return false;
  while (true) {
    TAMidiMessage m;
    double t=port->getMessage(&msg);
    if (msg.empty()) break;

    // parse message
    m.time=t;
    m.type=msg[0];
    if (m.type!=TA_MIDI_SYSEX && msg.size()>1) {
      memcpy(m.data,msg.data()+1,MIN(msg.size()-1,7));
    }
    queue.push(m);
  }
  return true;
}

std::vector<String> TAMidiInRtMidi::listDevices() {
  std::vector<String> ret;
  logD("listing devices.");
  if (port==NULL) return ret;

  try {
    unsigned int count=port->getPortCount();
    logD("got port count.");
    for (unsigned int i=0; i<count; i++) {
      String name=port->getPortName(i);
      if (name!="") ret.push_back(name);
    }
  } catch (RtMidiError& e) {
    logW("could not get MIDI inputs! %s",e.what());
  }
  return ret;
}

bool TAMidiInRtMidi::isDeviceOpen() {
  return isOpen;
}

bool TAMidiInRtMidi::openDevice(String name) {
  if (port==NULL) return false;
  if (isOpen) return false;
  try {
    bool portOpen=false;
    unsigned int count=port->getPortCount();
    for (unsigned int i=0; i<count; i++) {
      if (port->getPortName(i)==name) {
        port->openPort(i);
        portOpen=true;
        break;
      }
    }
    isOpen=portOpen;
    if (!portOpen) logW("could not find MIDI in device...");
    return portOpen;
  } catch (RtMidiError& e) {
    logW("could not open MIDI in device! %s",e.what());
    return false;
  }
  return true;
}

bool TAMidiInRtMidi::closeDevice() {
  if (port==NULL) return false;
  if (!isOpen) return false;
  try {
    port->closePort();
  } catch (RtMidiError& e) {
    logW("could not close MIDI in device! %s",e.what());
    isOpen=false; // still
    return false;
  }
  isOpen=false;
  return true;
}

bool TAMidiInRtMidi::init() {
  if (port!=NULL) return true;
  try {
    port=new RtMidiIn;
  } catch (RtMidiError& e) {
    logW("could not initialize RtMidi in! %s",e.what());
    return false;
  }
  return true;
}

bool TAMidiInRtMidi::quit() {
  if (port!=NULL) {
    delete port;
    port=NULL;
  }
  return true;
}

// --- OUT ---

bool TAMidiOutRtMidi::send(const TAMidiMessage& what) {
  if (!isOpen) return false;
  if (what.type<0x80) return false;
  size_t len=0;
  switch (what.type&0xf0) {
    case TA_MIDI_NOTE_OFF:
    case TA_MIDI_NOTE_ON:
    case TA_MIDI_AFTERTOUCH:
    case TA_MIDI_CONTROL:
    case TA_MIDI_PITCH_BEND:
      len=3;
      break;
    case TA_MIDI_PROGRAM:
    case TA_MIDI_CHANNEL_AFTERTOUCH:
      len=2;
      break;
  }
  if (len==0) switch (what.type) {
    case TA_MIDI_SYSEX: // currently not supported :<
      return false;
      break;
    case TA_MIDI_MTC_FRAME:
    case TA_MIDI_SONG_SELECT:
      len=2;
      break;
    case TA_MIDI_POSITION:
      len=3;
      break;
    default:
      len=1;
      break;
  }
  port->sendMessage((const unsigned char*)&what.type,len);
  return true;
}

bool TAMidiOutRtMidi::isDeviceOpen() {
  return isOpen;
}

bool TAMidiOutRtMidi::openDevice(String name) {
  if (port==NULL) return false;
  if (isOpen) return false;
  try {
    bool portOpen=false;
    unsigned int count=port->getPortCount();
    for (unsigned int i=0; i<count; i++) {
      if (port->getPortName(i)==name) {
        port->openPort(i);
        portOpen=true;
        break;
      }
    }
    isOpen=portOpen;
    if (!portOpen) logW("could not find MIDI out device...");
    return portOpen;
  } catch (RtMidiError& e) {
    logW("could not open MIDI out device! %s",e.what());
    return false;
  }
  return true;
}

bool TAMidiOutRtMidi::closeDevice() {
  if (port==NULL) return false;
  if (!isOpen) return false;
  try {
    port->closePort();
  } catch (RtMidiError& e) {
    logW("could not close MIDI out device! %s",e.what());
    isOpen=false; // still
    return false;
  }
  isOpen=false;
  return true;
}

std::vector<String> TAMidiOutRtMidi::listDevices() {
  std::vector<String> ret;
  if (port==NULL) return ret;

  try {
    unsigned int count=port->getPortCount();
    for (unsigned int i=0; i<count; i++) {
      String name=port->getPortName(i);
      if (name!="") ret.push_back(name);
    }
  } catch (RtMidiError& e) {
    logW("could not get MIDI outputs! %s",e.what());
  }
  return ret;
}

bool TAMidiOutRtMidi::init() {
  if (port!=NULL) return true;
  try {
    port=new RtMidiOut;
  } catch (RtMidiError& e) {
    logW("could not initialize RtMidi out! %s",e.what());
    return false;
  }
  return true;
}

bool TAMidiOutRtMidi::quit() {
  if (port!=NULL) {
    delete port;
    port=NULL;
  }
  return true;
}