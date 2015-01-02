#include "neopixelAnalyzerSettings.h"
#include <AnalyzerHelpers.h>


neopixelAnalyzerSettings::neopixelAnalyzerSettings()
:    mInputChannel( UNDEFINED_CHANNEL ),
    mBitRate(800000)
{
    mInputChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
    mInputChannelInterface->SetTitleAndTooltip( "Data", "Standard NeoPixel" );
    mInputChannelInterface->SetChannel( mInputChannel );

    AddInterface( mInputChannelInterface.get() );

    AddExportOption( 0, "Export as text/csv file" );
    AddExportExtension( 0, "text", "txt" );
    AddExportExtension( 0, "csv", "csv" );

    ClearChannels();
    AddChannel( mInputChannel, "NeoPixel", false );
}

neopixelAnalyzerSettings::~neopixelAnalyzerSettings()
{
}

bool neopixelAnalyzerSettings::SetSettingsFromInterfaces()
{
    mInputChannel = mInputChannelInterface->GetChannel();

    ClearChannels();
    AddChannel( mInputChannel, "NeoPixel", true );

    return true;
}

void neopixelAnalyzerSettings::UpdateInterfacesFromSettings()
{
    mInputChannelInterface->SetChannel( mInputChannel );
}

void neopixelAnalyzerSettings::LoadSettings( const char* settings )
{
    SimpleArchive text_archive;
    text_archive.SetString( settings );

    text_archive >> mInputChannel;

    ClearChannels();
    AddChannel( mInputChannel, "NeoPixel", true );

    UpdateInterfacesFromSettings();
}

const char* neopixelAnalyzerSettings::SaveSettings()
{
    SimpleArchive text_archive;

    text_archive << mInputChannel;

    return SetReturnString( text_archive.GetString() );
}
