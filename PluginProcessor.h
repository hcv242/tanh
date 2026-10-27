#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "Dsp.h"

class TanhDriveAudioProcessor : public juce::AudioProcessor
{
public:
    TanhDriveAudioProcessor();
    ~TanhDriveAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    // GUI-less: the host shows its own generic parameter list
    juce::AudioProcessorEditor* createEditor() override { return nullptr; }
    bool hasEditor() const override { return false; }

    const juce::String getName() const override { return "Tanh Drive"; }
    bool acceptsMidi() const override  { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return "Default"; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();

    juce::AudioProcessorValueTreeState apvts;
    std::atomic<float>* driveDb  = nullptr;
    std::atomic<float>* outputDb = nullptr;

    tanhdrive::Processor dsp;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TanhDriveAudioProcessor)
};
