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

#include "Acquisition.h"

namespace Metadata
{

bool Acquisition::writeToJSON(File file) const
{
    try
    {
        JSON::FormatOptions options;
        FileOutputStream outputStream(file);
        
        JSON::writeToStream(outputStream,
                            toJSON(),
                            options.withIndentLevel(4));
        
        return true;
    }
    catch (const std::exception& e)
    {
        return false;
    }
}

var Acquisition::toJSON() const
{
    DynamicObject* jsonObj = new DynamicObject();
    
    // Basic fields
    jsonObj->setProperty("subjectID", subjectID);
    jsonObj->setProperty("startTime", startTime.toISO8601(true));
    jsonObj->setProperty("endTime", endTime.toISO8601(true));
    jsonObj->setProperty("instrumentID", instrumentID);
    jsonObj->setProperty("acquisitionType", acquisitionType);
    jsonObj->setProperty("coordinateSystem", coordinateSystemToString(coordinateSystem));
    
    // Experimenters array
    var experimentersArray;
    for (int i = 0; i < experimenters.size(); ++i)
    {
        experimentersArray.append(experimenters[i]);
    }
    jsonObj->setProperty("experimenters", experimentersArray);
    
    // Data streams array
    var dataStreamsArray;
    for (int i = 0; i < dataStreams.size(); ++i)
    {
        if (dataStreams[i] != nullptr)
        {
            dataStreamsArray.append(dataStreamToJSON(*dataStreams[i]));
        }
    }
    jsonObj->setProperty("dataStreams", dataStreamsArray);
    
    return var(jsonObj);
}

var Acquisition::probeToJSON(const Probe& probe)
{
    DynamicObject* jsonObj = new DynamicObject();
    jsonObj->setProperty("name", probe.name);
    jsonObj->setProperty("primaryTargetedStructure", probe.primaryTargetedStructure);
    jsonObj->setProperty("surfaceZ", probe.surfaceZ);
    jsonObj->setProperty("finalZ", probe.finalZ);
    jsonObj->setProperty("dye", probe.dye);
    jsonObj->setProperty("notes", probe.notes);
    return var(jsonObj);
}

var Acquisition::misModuleToJSON(const MISModule& module)
{
    DynamicObject* jsonObj = new DynamicObject();
    jsonObj->setProperty("arcAngle", module.arcAngle);
    jsonObj->setProperty("moduleAngle", module.moduleAngle);
    jsonObj->setProperty("rotationAngle", module.rotationAngle);
    return var(jsonObj);
}

var Acquisition::manipulatorToJSON(const Manipulator& manipulator)
{
    DynamicObject* jsonObj = new DynamicObject();
    jsonObj->setProperty("name", manipulator.name);
    jsonObj->setProperty("coordinateSystem", coordinateSystemToString(manipulator.coordinateSystem));
    jsonObj->setProperty("xPosition", manipulator.xPosition);
    jsonObj->setProperty("yPosition", manipulator.yPosition);
    jsonObj->setProperty("zPosition", manipulator.zPosition);
    return var(jsonObj);
}

var Acquisition::ephysAssemblyToJSON(const EphysAssembly& assembly)
{
    DynamicObject* jsonObj = new DynamicObject();
    jsonObj->setProperty("name", assembly.name);
    jsonObj->setProperty("manipulator", manipulatorToJSON(assembly.manipulator));
    jsonObj->setProperty("probe", probeToJSON(assembly.probe));
    jsonObj->setProperty("misModule", misModuleToJSON(assembly.misModule));
    return var(jsonObj);
}

var Acquisition::dataStreamToJSON(const DataStream& stream)
{
    DynamicObject* jsonObj = new DynamicObject();
    jsonObj->setProperty("startTime", stream.startTime.toISO8601(true));
    jsonObj->setProperty("endTime", stream.endTime.toISO8601(true));
    
    // Active devices array
    var activeDevicesArray;
    for (int i = 0; i < stream.activeDevices.size(); ++i)
    {
        activeDevicesArray.append(stream.activeDevices[i]);
    }
    jsonObj->setProperty("activeDevices", activeDevicesArray);
    
    // Ephys assemblies array
    var ephysAssembliesArray;
    for (int i = 0; i < stream.ephysAssemblies.size(); ++i)
    {
        if (stream.ephysAssemblies[i] != nullptr)
        {
            ephysAssembliesArray.append(ephysAssemblyToJSON(*stream.ephysAssemblies[i]));
        }
    }
    jsonObj->setProperty("ephysAssemblies", ephysAssembliesArray);
    
    return var(jsonObj);
}

String Acquisition::coordinateSystemToString(CoordinateSystem system)
{
    switch (system)
    {
        case BREGMA_ARID:
            return "BREGMA_ARID";
        case MPM_MANIP_RFB:
            return "MPM_MANIP_RFB";
        default:
            return "UNKNOWN";
    }
}

} // namespace Metadata
