#ifndef NEOPIXEL_ANALYZER_H
#define NEOPIXEL_ANALYZER_H

#include <Analyzer.h>
#include "neopixelAnalyzerResults.h"
#include "neopixelSimulationDataGenerator.h"

class neopixelAnalyzerSettings;
class ANALYZER_EXPORT neopixelAnalyzer : public Analyzer
{
public:
	neopixelAnalyzer();
	virtual ~neopixelAnalyzer();
	virtual void WorkerThread();

	virtual U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels );
	virtual U32 GetMinimumSampleRateHz();

	virtual const char* GetAnalyzerName() const;
	virtual bool NeedsRerun();

protected: //vars
	std::auto_ptr< neopixelAnalyzerSettings > mSettings;
	std::auto_ptr< neopixelAnalyzerResults > mResults;
	AnalyzerChannelData* mSerial;

	neopixelSimulationDataGenerator mSimulationDataGenerator;
	bool mSimulationInitilized;

	//Serial analysis vars:
	U32 mSampleRateHz;
	U32 mStartOfStopBitOffset;
	U32 mEndOfStopBitOffset;
};

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer( );
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );

#endif //NEOPIXEL_ANALYZER_H
