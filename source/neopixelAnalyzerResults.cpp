#include "neopixelAnalyzerResults.h"
#include <AnalyzerHelpers.h>
#include "neopixelAnalyzer.h"
#include "neopixelAnalyzerSettings.h"
#include <iostream>
#include <fstream>


static void decode_rgb(const U64 data, U8& red, U8& green, U8& blue) 
{
    green = data & 0xFF;
    red = (data >> 8) & 0xFF;
    blue = (data >> 16) & 0xFF;
}


neopixelAnalyzerResults::neopixelAnalyzerResults(
        neopixelAnalyzer* analyzer, 
        neopixelAnalyzerSettings* settings )
:	AnalyzerResults(),
	mSettings( settings ),
	mAnalyzer( analyzer )
{
}

neopixelAnalyzerResults::~neopixelAnalyzerResults()
{
}

void neopixelAnalyzerResults::GenerateBubbleText(
        U64 frame_index, 
        Channel& channel, 
        DisplayBase display_base )
{
	ClearResultStrings();
	Frame frame = GetFrame( frame_index );

	char number_str[128];
    U8 red, green, blue;
    decode_rgb(frame.mData1, red, green, blue);
    snprintf(number_str, sizeof(number_str), "(%d,%d,%d)", 
            (int)green,
            (int)red,
            (int)blue);
	AddResultString( number_str );
}

void neopixelAnalyzerResults::GenerateExportFile(
        const char* file, 
        DisplayBase display_base, 
        U32 export_type_user_id )
{
	std::ofstream file_stream( file, std::ios::out );

	U64 trigger_sample = mAnalyzer->GetTriggerSample();
	U32 sample_rate = mAnalyzer->GetSampleRate();

	file_stream << "Time [s], Value (r,g,b)" << std::endl;

	U64 num_frames = GetNumFrames();
	for( U32 i=0; i < num_frames; i++ )
	{
		Frame frame = GetFrame( i );
		
		char time_str[128];
		AnalyzerHelpers::GetTimeString( frame.mStartingSampleInclusive, trigger_sample, sample_rate, time_str, 128 );

		char number_str[128];

        U8 red, green, blue;
        decode_rgb(frame.mData1, red, green, blue);
        snprintf(number_str, sizeof(number_str), "(%02x,%02x,%02x)", 
                (int)red,
                (int)green,
                (int)blue);

		file_stream << time_str << "," << number_str << std::endl;

		if( UpdateExportProgressAndCheckForCancel( i, num_frames ) == true )
		{
			file_stream.close();
			return;
		}
	}

	file_stream.close();
}

void neopixelAnalyzerResults::GenerateFrameTabularText(
        U64 frame_index, DisplayBase display_base )
{
	Frame frame = GetFrame( frame_index );
	ClearResultStrings();

	char number_str[128];
    U8 red, green, blue;
    decode_rgb(frame.mData1, red, green, blue);
    snprintf(number_str, sizeof(number_str), "r=%02x,g=%02x,b=%02x", 
            (int)red,
            (int)green,
            (int)blue);

	AddResultString( number_str );
}

void neopixelAnalyzerResults::GeneratePacketTabularText(
        U64 packet_id, DisplayBase display_base )
{
	ClearResultStrings();
	AddResultString( "not supported" );
}

void neopixelAnalyzerResults::GenerateTransactionTabularText(
        U64 transaction_id, DisplayBase display_base )
{
	ClearResultStrings();
	AddResultString( "not supported" );
}
