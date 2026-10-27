cmake_minimum_required(VERSION 3.22)
project(TanhDrive VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.6
    GIT_SHALLOW    ON)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(TanhDrive
    COMPANY_NAME                "TanhDrive"
    PRODUCT_NAME                "Tanh Drive"
    FORMATS                     VST3
    VST3_CATEGORIES             Fx Distortion
    PLUGIN_MANUFACTURER_CODE    Tnhd
    PLUGIN_CODE                 Tnh1
    IS_SYNTH                    FALSE
    NEEDS_MIDI_INPUT            FALSE
    NEEDS_MIDI_OUTPUT           FALSE
    IS_MIDI_EFFECT              FALSE
    NEEDS_WEB_BROWSER           FALSE
    COPY_PLUGIN_AFTER_BUILD     FALSE)

target_sources(TanhDrive PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/Dsp.h)

target_compile_definitions(TanhDrive PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(TanhDrive
    PRIVATE juce::juce_audio_utils
    PUBLIC  juce::juce_recommended_config_flags
            juce::juce_recommended_lto_flags
            juce::juce_recommended_warning_flags)
