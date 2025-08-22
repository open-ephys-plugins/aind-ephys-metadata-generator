/*
    ------------------------------------------------------------------

    This file is part of a plugin for the Open Ephys GUI
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

#include "EphysMetadataGeneratorEditor.h"

#include "EphysMetadataGenerator.h"
#include "EphysMetadataGeneratorCanvas.h"

EphysMetadataGeneratorEditor::EphysMetadataGeneratorEditor (GenericProcessor* p)
    : VisualizerEditor (p, "Ephys Metadata Generator", 240)
{
}

Visualizer* EphysMetadataGeneratorEditor::createNewCanvas()
{
    return new EphysMetadataGeneratorCanvas ((EphysMetadataGenerator*) getProcessor());
    ;
}

Metadata::Acquisition EphysMetadataGeneratorEditor::getLatestMetadata()
{
    if (canvas != nullptr)
    {
        // Ensure the canvas is of the correct type
        auto ephysMetadataGeneratorCanvas = (EphysMetadataGeneratorCanvas*) canvas.get();
        return ephysMetadataGeneratorCanvas->getLatestMetadata();
    }

    return Metadata::Acquisition(); // Return an empty acquisition if no canvas is available
}