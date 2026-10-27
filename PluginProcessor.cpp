#include "PluginProcessor.h"

namespace
{
    // "12.5 dB" in the host, and typed values such as "-6", "-6 dB" or "+3.5db" are understood
    juce::String dbToText (float value, int)            { return juce::String (value, 1) + " dB"; }
    float        textToDb (const juce::String& text)    { return text.retainCharacters ("-+.0123456789").getFloatValue(); }
}

juce::AudioProcessorValueTreeState::ParameterLayout TanhDriveAudioProcessor::createLayout()
{
    using namespace juce;
    AudioProcessorValueTreeState::ParameterLayout layout;

    auto attrs = AudioParameterFloatAttributes()
                     .withStringFromValueFunction (dbToText)
                     .withValueFromStringFunction (textToDb);

    layout.add (std::make_unique<AudioParameterFloat> (ParameterID { "drive", 1 }, "Drive",
                                                       NormalisableRange<float> (0.0f, 36.0f, 0.1f),
                                                       12.0f, attrs));

    layout.add (std::make_unique<AudioParameterFloat> (ParameterID { "output", 1 }, "Output",
                                                       NormalisableRange<float> (-24.0f, 12.0f, 0.1f),
                                                       0.0f, attrs));
    return layout;
}

TanhDriveAudioProcessor::TanhDriveAudioProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "STATE", createLayout())
{
    driveDb  = apvts.getRawParameterValue ("drive");
    outputDb = apvts.getRawParameterValue ("output");
}

bool TanhDriveAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& out = layouts.getMainOutputChannelSet();
    if (out != juce::AudioChannelSet::mono() && out != juce::AudioChannelSet::stereo())
        return false;
    return layouts.getMainInputChannelSet() == out;
}

void TanhDriveAudioProcessor::prepareToPlay (double sampleRate, int)
{
    dsp.prepare (sampleRate);
    dsp.reset (driveDb->load(), outputDb->load());
}

void TanhDriveAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int numSamples = buffer.getNumSamples();
    for (int ch = getTotalNumInputChannels(); ch < getTotalNumOutputChannels(); ++ch)
        buffer.clear (ch, 0, numSamples);

    dsp.setDriveDb  (driveDb->load());
    dsp.setOutputDb (outputDb->load());
    dsp.process (buffer.getArrayOfWritePointers(), buffer.getNumChannels(), numSamples);
}

void TanhDriveAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}

void TanhDriveAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TanhDriveAudioProcessor();
}
