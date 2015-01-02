#include "neopixelAnalyzer.h"
#include "neopixelAnalyzerSettings.h"
#include <AnalyzerChannelData.h>

neopixelAnalyzer::neopixelAnalyzer() :	
    Analyzer(),  
	mSettings( new neopixelAnalyzerSettings() ),
	mSimulationInitilized( false )
{
	SetAnalyzerSettings( mSettings.get() );
}

neopixelAnalyzer::~neopixelAnalyzer()
{
	KillThread();
}

void neopixelAnalyzer::WorkerThread()
{
	mResults.reset( new neopixelAnalyzerResults( 
                this, mSettings.get() ) );
	SetAnalyzerResults( mResults.get() );
	mResults->AddChannelBubblesWillAppearOn( mSettings->mInputChannel );

	mSampleRateHz = GetSampleRate();

	mSerial = GetAnalyzerChannelData( mSettings->mInputChannel );

    // Find next falling edge
	if( mSerial->GetBitState() == BIT_HIGH)
		mSerial->AdvanceToNextEdge();

	U32 samples_per_bit = mSampleRateHz / mSettings->mBitRate;

    const U32 LOW_SAMPLES  = 1.1 * (0.2917 / 1.25 * samples_per_bit);
    const U32 HIGH_SAMPLES = 1.1 * (0.8333 / 1.25 * samples_per_bit);

    U64 last_sample = mSerial->GetSampleNumber();

	for( ; ; )
	{
		U8 data[] = {0, 0, 0};
        U64 starting_sample = 0;

        // State is low
		for( U32 i=0; i<24; i++ ) {

            // Find next rising edge (beginning of clock)
            mSerial->AdvanceToNextEdge();
            U64 lapse = mSerial->GetSampleNumber() - last_sample;
            last_sample = mSerial->GetSampleNumber();

            // If it's more than a bit, start again
            if (lapse >= samples_per_bit) {
                i = 0;
            }

            U8 mod8 = i % 8;
            U8 byte = i / 8;
            U8 mask = 1 << (7-mod8);

            // First sample of frame
            if (i == 0) {
                starting_sample = last_sample;
            }

            // Start of byte
            if (mod8 == 0) {
                //let's put a dot exactly where we sample this bit
                mResults->AddMarker(last_sample, 
                        AnalyzerResults::Dot, mSettings->mInputChannel);
            }

            // Find next falling edge
            mSerial->AdvanceToNextEdge();

            lapse = mSerial->GetSampleNumber() - last_sample;
            last_sample = mSerial->GetSampleNumber();

            // Found high bit
            if (lapse > LOW_SAMPLES) {
                data[byte] |= mask;
            }
		}


		//we have a byte to save. 
		Frame frame;
		frame.mData1 = data[0] | data[1] << 8 | data[2] << 16;
		frame.mFlags = 0;
		frame.mStartingSampleInclusive = starting_sample;
		frame.mEndingSampleInclusive = mSerial->GetSampleNumber();

		mResults->AddFrame( frame );
		mResults->CommitResults();
		ReportProgress( frame.mEndingSampleInclusive );
	}
}

bool neopixelAnalyzer::NeedsRerun()
{
	return false;
}

U32 neopixelAnalyzer::GenerateSimulationData(
        U64 minimum_sample_index, 
        U32 device_sample_rate, 
        SimulationChannelDescriptor** simulation_channels )
{
	if( mSimulationInitilized == false )
	{
		mSimulationDataGenerator.Initialize( 
                GetSimulationSampleRate(), mSettings.get() );
		mSimulationInitilized = true;
	}

	return mSimulationDataGenerator.GenerateSimulationData(
           minimum_sample_index, 
           device_sample_rate, 
           simulation_channels );
}

U32 neopixelAnalyzer::GetMinimumSampleRateHz()
{
	return mSettings->mBitRate * 10;
}

const char* neopixelAnalyzer::GetAnalyzerName() const
{
	return "NeoPixel";
}

const char* GetAnalyzerName()
{
	return "NeoPixel";
}

Analyzer* CreateAnalyzer()
{
	return new neopixelAnalyzer();
}

void DestroyAnalyzer( Analyzer* analyzer )
{
	delete analyzer;
}
