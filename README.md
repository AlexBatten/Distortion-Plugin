# Bit Collapse

A VST3 distortion and bit crush plugin built with the JUCE framework.

## Features

### Distortion
- **4 distortion types:** Hard Clip, Soft Clip (tanh saturation), Foldback, Arc Tan
- **Drive** — controls how hard the signal is pushed into the distortion algorithm
- **Tone** — post-distortion low-pass filter to shape the harshness
- **Mix** — wet/dry blend, independent from drive

### Bit Crush
- **Bit Depth** (1-16 bit) — reduces signal resolution. 1-bit gives pure square waves, lower values give classic retro digital character
- **Downsample** (1x-50x) — sample-and-hold that reduces the effective sample rate for lo-fi staircase effects

## Signal Chain

Input → Downsample → Bit Reduce → Distortion → Tone Filter → Mix → Output

## Building

Requires [JUCE](https://juce.com/) and Visual Studio with the C++ desktop development workload.

1. Open `DistortionPlugin.jucer` in Projucer
2. Save and open the generated Visual Studio solution
3. Build as Release — the VST3 is output to `C:\Program Files\Common Files\VST3\`
