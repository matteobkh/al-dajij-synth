// gui.cpp
#include "gui.h"
#include "imgui.h"
#include "audio.h"
#include "imgui-knobs.h"
#include <mutex>

// Oscillator bank window
void renderGUI(AudioEngine& audioEngine) {
    ImGui::Begin("Oscillator Control");

    static int oscToRemove = -1;
    for (size_t i = 0; i < audioEngine.oscillators.size(); i++) {
        ImGui::PushID(i);
        ImGui::Text("Oscillator %d", (int)i+1);
        float freq = audioEngine.oscillators[i]->frequency.load();
        float pan = audioEngine.oscillators[i]->pan.load();
        float vol = audioEngine.oscillators[i]->volume.load();
        int* wf = &(audioEngine.oscillators[i]->waveform);
        ImGui::RadioButton("Sine", wf, 0);   ImGui::SameLine();
        ImGui::RadioButton("Square", wf, 1); ImGui::SameLine();
        ImGui::RadioButton("Saw", wf, 2);
        // Frequency knob
        if (ImGuiKnobs::Knob("Frequency", &freq, 20.0f, 5000.0f, 0.0f, "%.2f Hz", ImGuiKnobVariant_WiperDot, 0, ImGuiKnobFlags_Logarithmic)) {
            audioEngine.oscillators[i]->frequency.store(freq);
        }
        if (ImGui::IsItemActive() && ImGui::IsMouseDoubleClicked(0)) { //double click to reset
            freq = 440;
            audioEngine.oscillators[i]->frequency.store(freq);
        }
        ImGui::SameLine();
        // Pan knob
        if (ImGuiKnobs::Knob("Pan", &pan, 0.0f, 1.0f, 0.0f, "%.2f", ImGuiKnobVariant_WiperDot)) {
            audioEngine.oscillators[i]->pan.store(pan);
        }
        if (ImGui::IsItemActive() && ImGui::IsMouseDoubleClicked(0)) { //double click to reset
            pan = 0.5;
            audioEngine.oscillators[i]->pan.store(pan);
        }
        ImGui::SameLine();
        // Volume knob
        if (ImGuiKnobs::Knob("Volume", &vol, 0.0f, 1.0f, 0.0f, "%.2f", ImGuiKnobVariant_WiperDot)) {
            audioEngine.oscillators[i]->volume.store(vol);
        }
        if (ImGui::IsItemActive() && ImGui::IsMouseDoubleClicked(0)) { //double click to reset
            vol = 0.5;
            audioEngine.oscillators[i]->volume.store(vol);
        }
        if (ImGui::Button("Remove")) oscToRemove = i;
        ImGui::PopID();
    }
    
    if (ImGui::Button("Add Oscillator")) {
        audioEngine.addOscillator();
    }
    
    if (oscToRemove >= 0) {
        audioEngine.removeOscillator(oscToRemove);
        oscToRemove = -1;
    }

    ImGui::Checkbox("Filter passthrough", &audioEngine.filterL.passthrough);
    float cutoff = audioEngine.filterL.frequency.load();
    if (ImGui::SliderFloat("Filter cutoff", &cutoff, 20.0f, 20000.0f, NULL, ImGuiSliderFlags_Logarithmic)) {
        audioEngine.filterL.frequency.store(cutoff);
        audioEngine.filterL.updateAlpha();
        audioEngine.filterR.frequency.store(cutoff);
        audioEngine.filterR.updateAlpha();
    }

    //ImGui::Text("Master Volume");
    float mVol = audioEngine.masterVolume.load();
    if (ImGui::SliderFloat("Master Volume", &mVol, 0.0f, 1.0f)) {
        audioEngine.masterVolume.store(mVol);
    }

    // scope
    std::lock_guard<std::mutex> lock(audioMutex); // Ensure safe access
    ImGui::PlotLines("Waveform", audioBuffer.data(), audioBuffer.size(), 0, nullptr, -1.0f, 1.0f, ImVec2(400, 100));

    ImGui::End();
}