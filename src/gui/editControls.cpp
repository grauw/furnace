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

#include "gui.h"
#include "IconsFontAwesome4.h"
#include <imgui.h>

void FurnaceGUI::drawMobileControls() {
  if (ImGui::Begin("Mobile Controls",NULL,ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoScrollWithMouse|globalWinFlags)) {
    float availX=ImGui::GetContentRegionAvail().x;
    ImVec2 buttonSize=ImVec2(availX,availX);

    if (ImGui::Button(ICON_FA_CHEVRON_RIGHT "##MobileMenu",buttonSize)) {
    }

    ImGui::Separator();

    ImGui::PushStyleColor(ImGuiCol_Button,TOGGLE_COLOR(e->isPlaying()));
    if (ImGui::Button(ICON_FA_PLAY "##Play",buttonSize)) {
      play();
    }
    ImGui::PopStyleColor();
    if (ImGui::Button(ICON_FA_STOP "##Stop",buttonSize)) {
      stop();
    }
    if (ImGui::Button(ICON_FA_ARROW_DOWN "##StepOne",buttonSize)) {
      e->stepOne(cursor.y);
      pendingStepUpdate=true;
    }

    bool repeatPattern=e->getRepeatPattern();
    ImGui::PushStyleColor(ImGuiCol_Button,TOGGLE_COLOR(repeatPattern));
    if (ImGui::Button(ICON_FA_REPEAT "##RepeatPattern",buttonSize)) {
      e->setRepeatPattern(!repeatPattern);
    }
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Button,TOGGLE_COLOR(edit));
    if (ImGui::Button(ICON_FA_CIRCLE "##Edit",buttonSize)) {
      edit=!edit;
    }
    ImGui::PopStyleColor();

    bool metro=e->getMetronome();
    ImGui::PushStyleColor(ImGuiCol_Button,TOGGLE_COLOR(metro));
    if (ImGui::Button(ICON_FA_BELL_O "##Metronome",buttonSize)) {
      e->setMetronome(!metro);
    }
    ImGui::PopStyleColor();

    if (ImGui::Button("Get me out of here")) {
      toggleMobileUI(false);
    }
  }
  if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) curWindow=GUI_WINDOW_EDIT_CONTROLS;
  ImGui::End();
}

void FurnaceGUI::drawEditControls() {
  if (nextWindow==GUI_WINDOW_EDIT_CONTROLS) {
    editControlsOpen=true;
    ImGui::SetNextWindowFocus();
    nextWindow=GUI_WINDOW_NOTHING;
  }
  if (!editControlsOpen) return;
  switch (settings.controlLayout) {
    case 0: // classic
      if (ImGui::Begin("Play/Edit Controls",&editControlsOpen,globalWinFlags)) {
        if (ImGui::BeginTable("PlayEditAlign",2)) {
          ImGui::TableSetupColumn("c0",ImGuiTableColumnFlags_WidthFixed);
          ImGui::TableSetupColumn("c1",ImGuiTableColumnFlags_WidthStretch);

          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          ImGui::Text("Octave");
          ImGui::TableNextColumn();
          ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
          if (ImGui::InputInt("##Octave",&curOctave,1,1)) {
            if (curOctave>7) curOctave=7;
            if (curOctave<-5) curOctave=-5;
            e->autoNoteOffAll();

            if (settings.insFocusesPattern && !ImGui::IsItemActive() && patternOpen) {
              nextWindow=GUI_WINDOW_PATTERN;
            }
          }

          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          ImGui::Text("Edit Step");
          ImGui::TableNextColumn();
          ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
          if (ImGui::InputInt("##EditStep",&editStep,1,1)) {
            if (editStep>=e->curSubSong->patLen) editStep=e->curSubSong->patLen-1;
            if (editStep<0) editStep=0;

            if (settings.insFocusesPattern && !ImGui::IsItemActive() && patternOpen) {
              nextWindow=GUI_WINDOW_PATTERN;
            }
          }

          ImGui::EndTable();
        }

        ImGui::PushStyleColor(ImGuiCol_Button,TOGGLE_COLOR(e->isPlaying()));
        if (ImGui::Button(ICON_FA_PLAY "##Play")) {
          play();
        }
        ImGui::PopStyleColor();
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_STOP "##Stop")) {
          stop();
        }
        ImGui::SameLine();
        ImGui::Checkbox("Edit",&edit);
        ImGui::SameLine();
        bool metro=e->getMetronome();
        if (ImGui::Checkbox("Metronome",&metro)) {
          e->setMetronome(metro);
        }

        ImGui::Text("Follow");
        ImGui::SameLine();
        unimportant(ImGui::Checkbox("Orders",&followOrders));
        ImGui::SameLine();
        unimportant(ImGui::Checkbox("Pattern",&followPattern));

        bool repeatPattern=e->getRepeatPattern();
        if (ImGui::Checkbox("Repeat pattern",&repeatPattern)) {
          e->setRepeatPattern(repeatPattern);
        }
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_ARROW_DOWN "##StepOne")) {
          e->stepOne(cursor.y);
          pendingStepUpdate=true;
        }
      }
      if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) curWindow=GUI_WINDOW_EDIT_CONTROLS;
      ImGui::End();
      break;
    case 1: // compact
      if (ImGui::Begin("Play/Edit Controls",&editControlsOpen,ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoScrollWithMouse|globalWinFlags)) {
        if (ImGui::Button(ICON_FA_STOP "##Stop")) {
          stop();
        }
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button,TOGGLE_COLOR(e->isPlaying()));
        if (ImGui::Button(ICON_FA_PLAY "##Play")) {
          play();
        }
        ImGui::PopStyleColor();
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_ARROW_DOWN "##StepOne")) {
          e->stepOne(cursor.y);
          pendingStepUpdate=true;
        }

        ImGui::SameLine();
        bool repeatPattern=e->getRepeatPattern();
        ImGui::PushStyleColor(ImGuiCol_Button,TOGGLE_COLOR(repeatPattern));
        if (ImGui::Button(ICON_FA_REPEAT "##RepeatPattern")) {
          e->setRepeatPattern(!repeatPattern);
        }
        ImGui::PopStyleColor();

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button,TOGGLE_COLOR(edit));
        if (ImGui::Button(ICON_FA_CIRCLE "##Edit")) {
          edit=!edit;
        }
        ImGui::PopStyleColor();

        ImGui::SameLine();
        bool metro=e->getMetronome();
        ImGui::PushStyleColor(ImGuiCol_Button,TOGGLE_COLOR(metro));
        if (ImGui::Button(ICON_FA_BELL_O "##Metronome")) {
          e->setMetronome(!metro);
        }
        ImGui::PopStyleColor();

        ImGui::SameLine();
        ImGui::Text("Octave");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(96.0f*dpiScale);
        if (ImGui::InputInt("##Octave",&curOctave,1,1)) {
          if (curOctave>7) curOctave=7;
          if (curOctave<-5) curOctave=-5;
          e->autoNoteOffAll();

          if (settings.insFocusesPattern && !ImGui::IsItemActive() && patternOpen) {
            nextWindow=GUI_WINDOW_PATTERN;
          }
        }

        ImGui::SameLine();
        ImGui::Text("Edit Step");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(96.0f*dpiScale);
        if (ImGui::InputInt("##EditStep",&editStep,1,1)) {
          if (editStep>=e->curSubSong->patLen) editStep=e->curSubSong->patLen-1;
          if (editStep<0) editStep=0;

          if (settings.insFocusesPattern && !ImGui::IsItemActive() && patternOpen) {
            nextWindow=GUI_WINDOW_PATTERN;
          }
        }

        ImGui::SameLine();
        ImGui::Text("Follow");
        ImGui::SameLine();
        unimportant(ImGui::Checkbox("Orders",&followOrders));
        ImGui::SameLine();
        unimportant(ImGui::Checkbox("Pattern",&followPattern));
      }
      if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) curWindow=GUI_WINDOW_EDIT_CONTROLS;
      ImGui::End();
      break;
    case 2: // compact vertical
      if (ImGui::Begin("Play/Edit Controls",&editControlsOpen,ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoScrollWithMouse|globalWinFlags)) {
        ImVec2 buttonSize=ImVec2(ImGui::GetContentRegionAvail().x,0.0f);
        ImGui::PushStyleColor(ImGuiCol_Button,TOGGLE_COLOR(e->isPlaying()));
        if (ImGui::Button(ICON_FA_PLAY "##Play",buttonSize)) {
          play();
        }
        ImGui::PopStyleColor();
        if (ImGui::Button(ICON_FA_STOP "##Stop",buttonSize)) {
          stop();
        }
        if (ImGui::Button(ICON_FA_ARROW_DOWN "##StepOne",buttonSize)) {
          e->stepOne(cursor.y);
          pendingStepUpdate=true;
        }

        bool repeatPattern=e->getRepeatPattern();
        ImGui::PushStyleColor(ImGuiCol_Button,TOGGLE_COLOR(repeatPattern));
        if (ImGui::Button(ICON_FA_REPEAT "##RepeatPattern",buttonSize)) {
          e->setRepeatPattern(!repeatPattern);
        }
        ImGui::PopStyleColor();

        ImGui::PushStyleColor(ImGuiCol_Button,TOGGLE_COLOR(edit));
        if (ImGui::Button(ICON_FA_CIRCLE "##Edit",buttonSize)) {
          edit=!edit;
        }
        ImGui::PopStyleColor();

        bool metro=e->getMetronome();
        ImGui::PushStyleColor(ImGuiCol_Button,TOGGLE_COLOR(metro));
        if (ImGui::Button(ICON_FA_BELL_O "##Metronome",buttonSize)) {
          e->setMetronome(!metro);
        }
        ImGui::PopStyleColor();

        ImGui::Text("Oct.");
        float avail=ImGui::GetContentRegionAvail().x;
        ImGui::SetNextItemWidth(avail);
        if (ImGui::InputInt("##Octave",&curOctave,0,0)) {
          if (curOctave>7) curOctave=7;
          if (curOctave<-5) curOctave=-5;
          e->autoNoteOffAll();

          if (settings.insFocusesPattern && !ImGui::IsItemActive() && patternOpen) {
            nextWindow=GUI_WINDOW_PATTERN;
          }
        }

        ImGui::Text("Step");
        ImGui::SetNextItemWidth(avail);
        if (ImGui::InputInt("##EditStep",&editStep,0,0)) {
          if (editStep>=e->curSubSong->patLen) editStep=e->curSubSong->patLen-1;
          if (editStep<0) editStep=0;

          if (settings.insFocusesPattern && !ImGui::IsItemActive() && patternOpen) {
            nextWindow=GUI_WINDOW_PATTERN;
          }
        }

        ImGui::Text("Foll.");
        ImGui::PushStyleColor(ImGuiCol_Button,TOGGLE_COLOR(followOrders));
        if (ImGui::Button("Ord##FollowOrders",buttonSize)) { handleUnimportant
          followOrders=!followOrders;
        }
        ImGui::PopStyleColor();
        ImGui::PushStyleColor(ImGuiCol_Button,TOGGLE_COLOR(followPattern));
        if (ImGui::Button("Pat##FollowPattern",buttonSize)) { handleUnimportant
          followPattern=!followPattern;
        }
        ImGui::PopStyleColor();
      }
      if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) curWindow=GUI_WINDOW_EDIT_CONTROLS;
      ImGui::End();
      break;
    case 3: // split
      if (ImGui::Begin("Play Controls",&editControlsOpen,ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoScrollWithMouse|globalWinFlags)) {
        if (e->isPlaying()) {
          ImGui::PushStyleColor(ImGuiCol_Button,uiColors[GUI_COLOR_TOGGLE_ON]);
          if (ImGui::Button(ICON_FA_STOP "##Stop")) {
            stop();
          }
          ImGui::PopStyleColor();
        } else {
          if (ImGui::Button(ICON_FA_PLAY "##Play")) {
            play(oldRow);
          }
        }
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_PLAY_CIRCLE "##PlayAgain")) {
          e->setRepeatPattern(false);
          play();
        }
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_STEP_FORWARD "##PlayRepeat")) {
          e->setRepeatPattern(true);
          play();
        }
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_ARROW_DOWN "##StepOne")) {
          e->stepOne(cursor.y);
          pendingStepUpdate=true;
        }

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button,TOGGLE_COLOR(edit));
        if (ImGui::Button(ICON_FA_CIRCLE "##Edit")) {
          edit=!edit;
        }
        ImGui::PopStyleColor();

        bool metro=e->getMetronome();
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button,TOGGLE_COLOR(metro));
        if (ImGui::Button(ICON_FA_BELL_O "##Metronome")) {
          e->setMetronome(!metro);
        }
        ImGui::PopStyleColor();

        ImGui::SameLine();
        bool repeatPattern=e->getRepeatPattern();
        ImGui::PushStyleColor(ImGuiCol_Button,TOGGLE_COLOR(repeatPattern));
        if (ImGui::Button(ICON_FA_REPEAT "##RepeatPattern")) {
          e->setRepeatPattern(!repeatPattern);
        }
        ImGui::PopStyleColor();
      }
      if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) curWindow=GUI_WINDOW_EDIT_CONTROLS;
      ImGui::End();

      if (ImGui::Begin("Edit Controls",&editControlsOpen,globalWinFlags)) {
        ImGui::Columns(2);
        ImGui::Text("Octave");
        ImGui::SameLine();
        float cursor=ImGui::GetCursorPosX();
        float avail=ImGui::GetContentRegionAvail().x;
        ImGui::SetNextItemWidth(avail);
        if (ImGui::InputInt("##Octave",&curOctave,1,1)) {
          if (curOctave>7) curOctave=7;
          if (curOctave<-5) curOctave=-5;
          e->autoNoteOffAll();

          if (settings.insFocusesPattern && !ImGui::IsItemActive() && patternOpen) {
            nextWindow=GUI_WINDOW_PATTERN;
          }
        }

        ImGui::Text("Step");
        ImGui::SameLine();
        ImGui::SetCursorPosX(cursor);
        ImGui::SetNextItemWidth(avail);
        if (ImGui::InputInt("##EditStep",&editStep,1,1)) {
          if (editStep>=e->curSubSong->patLen) editStep=e->curSubSong->patLen-1;
          if (editStep<0) editStep=0;

          if (settings.insFocusesPattern && !ImGui::IsItemActive() && patternOpen) {
            nextWindow=GUI_WINDOW_PATTERN;
          }
        }
        ImGui::NextColumn();

        unimportant(ImGui::Checkbox("Follow orders",&followOrders));
        unimportant(ImGui::Checkbox("Follow pattern",&followPattern));
      }
      if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) curWindow=GUI_WINDOW_EDIT_CONTROLS;
      ImGui::End();
      break;
  }
}
