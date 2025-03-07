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

// guiConst: constants used in the GUI like arrays, strings and other stuff

struct FurnaceGUIActionDef {
  const char* name;
  const char* friendlyName;
  int defaultBind;
  FurnaceGUIActionDef(const char* n, const char* fn, int db):
    name(n), friendlyName(fn), defaultBind(db) {}
};

struct FurnaceGUIColorDef {
  const char* name;
  const char* friendlyName;
  unsigned int defaultColor;
  FurnaceGUIColorDef(const char* n, const char* fn, unsigned int dc):
    name(n), friendlyName(fn), defaultColor(dc) {}
};

extern const int opOrder[4];
extern const char* noteNames[180];
extern const char* noteNamesG[180];
extern const char* pitchLabel[11];
extern const char* insTypes[];
extern const char* sampleDepths[17];
extern const char* resampleStrats[];
extern const int availableSystems[];
extern const FurnaceGUIActionDef guiActions[];
extern const FurnaceGUIColorDef guiColors[];
extern const int altValues[24];
extern const int vgmVersions[6];
extern const FurnaceGUIColors fxColors[16];
extern const FurnaceGUIColors extFxColors[32];