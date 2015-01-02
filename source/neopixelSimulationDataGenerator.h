#ifndef NEOPIXEL_SIMULATION_DATA_GENERATOR
#define NEOPIXEL_SIMULATION_DATA_GENERATOR

#include <SimulationChannelDescriptor.h>
#include <string>
class neopixelAnalyzerSettings;

class neopixelSimulationDataGenerator
{
    public:
        neopixelSimulationDataGenerator();
        ~neopixelSimulationDataGenerator();

        void Initialize( U32 simulation_sample_rate, 
                neopixelAnalyzerSettings* settings );
        U32 GenerateSimulationData( U64 newest_sample_requested, 
                U32 sample_rate, 
                SimulationChannelDescriptor** simulation_channel );

    protected:
        neopixelAnalyzerSettings* mSettings;
        U32 mSimulationSampleRateHz;

    protected:
        void CreateSerialFrame();
        static const unsigned int NUM_LEDS = 10;
        U8 data[NUM_LEDS][3];
        U32 mPixelIndex;

        SimulationChannelDescriptor mSerialSimulationData;

};
#endif //NEOPIXEL_SIMULATION_DATA_GENERATOR
