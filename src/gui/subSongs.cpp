#include "gui.h"
#include "imgui.h"
#include "IconsFontAwesome4.h"
#include "misc/cpp/imgui_stdlib.h"

void FurnaceGUI::drawSubSongs() {
  if (nextWindow==GUI_WINDOW_SUBSONGS) {
    subSongsOpen=true;
    ImGui::SetNextWindowFocus();
    nextWindow=GUI_WINDOW_NOTHING;
  }
  if (!subSongsOpen) return;
  ImGui::SetNextWindowSizeConstraints(ImVec2(64.0f*dpiScale,32.0f*dpiScale),ImVec2(scrW*dpiScale,scrH*dpiScale));
  if (ImGui::Begin("Subsongs",&subSongsOpen,ImGuiWindowFlags_NoScrollWithMouse|ImGuiWindowFlags_NoScrollbar|globalWinFlags)) {
    char id[1024];
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x-ImGui::GetFrameHeightWithSpacing()*2.0f-ImGui::GetStyle().ItemSpacing.x);
    if (e->curSubSong->name.empty()) {
      snprintf(id,1023,"%d. <no name>",(int)e->getCurrentSubSong()+1);
    } else {
      snprintf(id,1023,"%d. %s",(int)e->getCurrentSubSong()+1,e->curSubSong->name.c_str());
    }
    if (ImGui::BeginCombo("##SubSong",id)) {
      for (size_t i=0; i<e->song.subsong.size(); i++) {
        if (e->song.subsong[i]->name.empty()) {
          snprintf(id,1023,"%d. <no name>",(int)i+1);
        } else {
          snprintf(id,1023,"%d. %s",(int)i+1,e->song.subsong[i]->name.c_str());
        }
        if (ImGui::Selectable(id,i==e->getCurrentSubSong())) {
          e->changeSongP(i);
          updateScroll(0);
          oldOrder=0;
          oldOrder1=0;
          oldRow=0;
          cursor.xCoarse=0;
          cursor.xFine=0;
          cursor.y=0;
          selStart=cursor;
          selEnd=cursor;
          curOrder=0;
        }
      }
      ImGui::EndCombo();
    }
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_PLUS "##SubSongAdd")) {
      if (!e->addSubSong()) {
        showError("too many subsongs!");
      } else {
        e->changeSongP(e->song.subsong.size()-1);
        updateScroll(0);
        oldOrder=0;
        oldOrder1=0;
        oldRow=0;
        cursor.xCoarse=0;
        cursor.xFine=0;
        cursor.y=0;
        selStart=cursor;
        selEnd=cursor;
        curOrder=0;
        MARK_MODIFIED;
      }
    }
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_MINUS "##SubSongDel")) {
      if (e->song.subsong.size()<=1) {
        showError("this is the only subsong!");
      } else {
        showWarning("are you sure you want to remove this subsong?",GUI_WARN_SUBSONG_DEL);
      }
    }

    ImGui::Text("Name");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    if (ImGui::InputText("##SubSongName",&e->curSubSong->name)) {
      MARK_MODIFIED;
    }
  }
  if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) curWindow=GUI_WINDOW_SUBSONGS;
  ImGui::End();
}
