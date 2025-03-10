#include "gui.h"
#include "imgui.h"
#include "audio.h"
#include "imgui-knobs.h"
#include <mutex>

// knob speeds for coarse and fine tuning
#define FAST (0.0f)
#define SLOW (0.0003f)

// Oscillator bank window
void renderGUI(AudioEngine& audioEngine) {
    ImGui::Begin("Oscillator Control");

    static int oscToRemove = -1;

    // Toggle fine tuning knobs
    float knobSpeed = FAST;
    ImGui::GetIO();
    if(ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl)) {
        knobSpeed = SLOW;
    } else {
        knobSpeed = FAST;
    }

    // Render oscillator bank
    for (size_t i = 0; i < audioEngine.oscillators.size(); i++) {
        ImGui::PushID(i);
        ImGui::Text("Oscillator %d", (int)i+1);
        float freq = audioEngine.oscillators[i]->frequency.load();
        float pan = audioEngine.oscillators[i]->pan.load();
        float vol = audioEngine.oscillators[i]->volume.load();
        int* wf = &(audioEngine.oscillators[i]->waveform);
        ImGui::RadioButton("Sine", wf, SINEW);   ImGui::SameLine();
        ImGui::RadioButton("Square", wf, SQUAREW); ImGui::SameLine();
        ImGui::RadioButton("Saw", wf, SAWW);
        // Frequency knob
        if (ImGuiKnobs::Knob("Frequency", &freq, 20.0f, 5000.0f, knobSpeed * 5000, "%.2f Hz", ImGuiKnobVariant_WiperDot, 0, ImGuiKnobFlags_Logarithmic)) {
            audioEngine.oscillators[i]->frequency.store(freq);
        }
        if (ImGui::IsItemActive() && ImGui::IsMouseDoubleClicked(0)) { //double click to reset
            freq = 440;
            audioEngine.oscillators[i]->frequency.store(freq);
        }
        ImGui::SameLine();
        // Pan knob
        if (ImGuiKnobs::Knob("Pan", &pan, 0.0f, 1.0f, knobSpeed, "%.2f", ImGuiKnobVariant_WiperDot)) {
            audioEngine.oscillators[i]->pan.store(pan);
        }
        if (ImGui::IsItemActive() && ImGui::IsMouseDoubleClicked(0)) { //double click to reset
            pan = 0.5;
            audioEngine.oscillators[i]->pan.store(pan);
        }
        ImGui::SameLine();
        // Volume knob
        if (ImGuiKnobs::Knob("Volume", &vol, 0.0f, 1.0f, knobSpeed, "%.2f", ImGuiKnobVariant_WiperDot)) {
            audioEngine.oscillators[i]->volume.store(vol);
        }
        if (ImGui::IsItemActive() && ImGui::IsMouseDoubleClicked(0)) { //double click to reset
            vol = 0.5;
            audioEngine.oscillators[i]->volume.store(vol);
        }
        if (ImGui::Button("Remove")) 
            oscToRemove = i;
        ImGui::PopID();
    }
    
    if (ImGui::Button("Add Oscillator")) {
        audioEngine.addOscillator();
    }
    
    if (oscToRemove >= 0) {
        audioEngine.removeOscillator(oscToRemove);
        oscToRemove = -1;
    }

    // Filter
    bool passthrough = audioEngine.filter.passthrough;
    if(ImGui::Checkbox("Filter passthrough", &passthrough)) {
        audioEngine.filter.passthrough = passthrough;
    }
    float cutoff = audioEngine.filter.frequency.load();
    if (ImGuiKnobs::Knob("Cutoff", &cutoff, 20.0f, 20000.0f, knobSpeed * 20000, "%.2f", ImGuiKnobVariant_WiperDot, 0, ImGuiKnobFlags_Logarithmic)) {
        audioEngine.filter.frequency.store(cutoff);
        audioEngine.filter.updateAlpha();
    }
    ImGui::SameLine();
    // if you change the max n of poles please change the last 3 variables here:
    ImGuiKnobs::KnobInt("# Poles", &audioEngine.filter.nPoles, 1, 4, 0.1, "%i", ImGuiKnobVariant_Stepped,0,0,4,3.5,6);

    ImGui::SameLine();

    // Volume knob
    float mVol = audioEngine.masterVolume.load();
    if (ImGuiKnobs::Knob("Master Volume", &mVol, 0.0f, 1.0f, knobSpeed, "%.2f", ImGuiKnobVariant_WiperDot)) {
        audioEngine.masterVolume.store(mVol);
    }
    if (ImGui::IsItemActive() && ImGui::IsMouseDoubleClicked(0)) { //double click to reset
        mVol = 1;
        audioEngine.masterVolume.store(mVol);
    }

    // scope
    std::lock_guard<std::mutex> lock(audioMutex); // Ensure safe access
    ImGui::PlotLines("Waveform", audioBuffer.data(), audioBuffer.size(), 0, nullptr, -1.0f, 1.0f, ImVec2(400, 100));

    ImGui::End();
}