#include "neopixelSimulationDataGenerator.h"
#include "neopixelAnalyzerSettings.h"

#include <AnalyzerHelpers.h>
#include <iostream>
#include <fstream>


neopixelSimulationDataGenerator::neopixelSimulationDataGenerator() :
    mPixelIndex( 0 )
{
    std::ofstream myfile;
    myfile.open ("/tmp/input.txt");
    char number_str[128];
    for (int i = 0; i < NUM_LEDS; i++) {
        int green = data[i][0] = (i+1) * 5;
        int red = data[i][1] = (i+2) * 5;
        int blue = data[i][2] = (i+3) * 5;

        snprintf(number_str, sizeof(number_str), "(%02x,%02x,%02x)\n", 
                (int)red,
                (int)green,
                (int)blue);
        myfile << number_str;
    }
    myfile.close();

}

neopixelSimulationDataGenerator::~neopixelSimulationDataGenerator()
{
}

void neopixelSimulationDataGenerator::Initialize( 
        U32 simulation_sample_rate, 
        neopixelAnalyzerSettings* settings )
{
    mSimulationSampleRateHz = simulation_sample_rate;
    mSettings = settings;

    mSerialSimulationData.SetChannel( mSettings->mInputChannel );
    mSerialSimulationData.SetSampleRate( simulation_sample_rate );
    mSerialSimulationData.SetInitialBitState( BIT_LOW );
}

U32 neopixelSimulationDataGenerator::GenerateSimulationData( 
        U64 largest_sample_requested, 
        U32 sample_rate, 
        SimulationChannelDescriptor** simulation_channel )
{
    U64 adjusted_largest_sample_requested = 
            AnalyzerHelpers::AdjustSimulationTargetSample( 
            largest_sample_requested, sample_rate, 
            mSimulationSampleRateHz );

    while( mSerialSimulationData.GetCurrentSampleNumber() < 
            adjusted_largest_sample_requested )
    {
        CreateSerialFrame();
    }

    *simulation_channel = &mSerialSimulationData;
    return 1;
}

void neopixelSimulationDataGenerator::CreateSerialFrame()
{
    U32 samples_per_bit = mSimulationSampleRateHz / 
            mSettings->mBitRate;

    // Move forward a bibt
    mSerialSimulationData.Advance( samples_per_bit * 10 );

    U32 low_samples  = 0.2917 / 1.25 * samples_per_bit;
    U32 high_samples = 0.8333 / 1.25 * samples_per_bit;

    // Green & most significant bit first
    for (int p = 0; p < 3; p++) {
        U8 byte = data[ mPixelIndex ][p];

        U8 mask = 0x1 << 7;
        for( U32 i=0; i<8; i++ ) {
            mSerialSimulationData.Transition(); // go high
            int advance = low_samples;
            if( byte & mask ) {
                advance = high_samples;
            }
            mSerialSimulationData.Advance(advance);
            mSerialSimulationData.Transition(); // go low
            mSerialSimulationData.Advance(samples_per_bit - advance);
            mask = mask >> 1;
        }
    }

    mPixelIndex++;
    if( mPixelIndex == NUM_LEDS ) mPixelIndex = 0;
}
