/*
    ------------------------------------------------------------------

    This file is part of the Open Ephys GUI
    Copyright (C) 2025 Open Ephys

    ------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "EphysMetadataGenerator.h"

#include "EphysMetadataGeneratorEditor.h"

EphysMetadataGenerator::EphysMetadataGenerator()
    : GenericProcessor ("Ephys Metadata Generator")
{
}

EphysMetadataGenerator::~EphysMetadataGenerator()
{
}

AudioProcessorEditor* EphysMetadataGenerator::createEditor()
{
    editor = std::make_unique<EphysMetadataGeneratorEditor> (this);
    return editor.get();
}

void EphysMetadataGenerator::registerParameters()
{
    // Register any parameters here if needed
    // For example: addSelectedChannelsParameter (Parameter::STREAM_SCOPE,
    //                                 channels,
    //                                 Channels,
    //                                 Selected channels to use);
}

void EphysMetadataGenerator::updateSettings()
{
}

void EphysMetadataGenerator::process (AudioBuffer<float>& buffer)
{
    checkForEvents (true);
}

void EphysMetadataGenerator::handleTTLEvent (TTLEventPtr event)
{
}

void EphysMetadataGenerator::handleSpike (SpikePtr spike)
{
}

void EphysMetadataGenerator::handleBroadcastMessage (const String& message, const int64 systemTimeMillis)
{
}

void EphysMetadataGenerator::saveCustomParametersToXml (XmlElement* parentElement)
{
}

void EphysMetadataGenerator::loadCustomParametersFromXml (XmlElement* parentElement)
{
}
