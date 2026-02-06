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

AudioProcessorEditor* EphysMetadataGenerator::createEditor()
{
    editor = std::make_unique<EphysMetadataGeneratorEditor> (this);
    return editor.get();
}

void EphysMetadataGenerator::updateSettings()
{
    // Needs to count the number of probes and use this to
    // update the table model in the canvas
}

void EphysMetadataGenerator::startRecording()
{
    EphysMetadataGeneratorEditor* thisEditor = dynamic_cast<EphysMetadataGeneratorEditor*> (getEditor());
    dataStreamMetadata = thisEditor->getLatestMetadata();
}

void EphysMetadataGenerator::stopRecording()
{
    
    // This function is called when recording stops
    
    File parentDirectory = CoreServices::getRecordingParentDirectory().getChildFile(CoreServices::getRecordingDirectoryName());
    
    File recordingDirectory = parentDirectory.getChildFile (getName() + " " + String(getNodeId()));
    
    if (!recordingDirectory.exists())
    {
        recordingDirectory.createDirectory();
    }

    // Get the experiment number

    int firstRecNodeId = CoreServices::getAvailableRecordNodeIds().getFirst();
    int experimentNum = CoreServices::RecordNode::getExperimentNumber (firstRecNodeId);
    int recordingNum = CoreServices::RecordNode::getRecordingNumber (firstRecNodeId);

    String filename = String ("ephys_datastream_" + String (experimentNum) + "_" + String (recordingNum) + ".json");
    
    // Save the data streams file in the recording directory
    File metadataFile = recordingDirectory.getChildFile (filename);
    
    // Set end time for all streams and write to JSON
    Time endTime = Time::getCurrentTime();
    dataStreamMetadata.stream_end_time = endTime;
    
    dataStreamMetadata.writeToJSON (metadataFile);
    
}
