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
    jsonObj->setProperty("object_type", object_type);
    jsonObj->setProperty("describedBy", describedBy);
    jsonObj->setProperty("schema_version", schema_version);
    jsonObj->setProperty("subject_id", subject_id);
    
    // Handle null specimen_id
    if (specimen_id.isEmpty())
        jsonObj->setProperty("specimen_id", var());
    else
        jsonObj->setProperty("specimen_id", specimen_id);
    
    jsonObj->setProperty("acquisition_start_time", acquisition_start_time.toISO8601(true));
    jsonObj->setProperty("acquisition_end_time", acquisition_end_time.toISO8601(true));

    var experimentersArray;
    for (int i = 0; i < experimenters.size(); ++i)
    {
        experimentersArray.append(experimenters[i]);
    }
    jsonObj->setProperty("experimenters", experimentersArray);
    
    // Handle null protocol_id
    if (protocol_id.isEmpty())
        jsonObj->setProperty("protocol_id", var());
    else
        jsonObj->setProperty("protocol_id", protocol_id);
    
    var ethics_review_array;
    for (int i = 0; i < ethics_review_id.size(); ++i)
    {
        ethics_review_array.append(ethics_review_id[i]);
    }
    jsonObj->setProperty("ethics_review_id", ethics_review_array);
    
    jsonObj->setProperty("instrument_id", instrument_id);
    jsonObj->setProperty("acquisition_type", acquisition_type);
    
    // Handle null notes
    if (notes.isEmpty())
        jsonObj->setProperty("notes", var());
    else
        jsonObj->setProperty("notes", notes);
    
    // Coordinate system as complex object
    jsonObj->setProperty("coordinate_system", coordinateSystemObjectToJSON(coordinate_system));
    
    // Arrays - ensure they are empty arrays, not null
    Array<var> calibrationsArrayData;
    for (int i = 0; i < calibrations.size(); ++i)
    {
        calibrationsArrayData.add(calibrations[i]);
    }
    jsonObj->setProperty("calibrations", var(calibrationsArrayData));

    Array<var> maintenanceArrayData;
    for (int i = 0; i < maintenance.size(); ++i)
    {
        maintenanceArrayData.add(maintenance[i]);
    }
    jsonObj->setProperty("maintenance", var(maintenanceArrayData));
    
    // Data streams array - ensure it's empty array, not null
    Array<var> dataStreamsArrayData;
    for (int i = 0; i < data_streams.size(); ++i)
    {
        dataStreamsArrayData.add(dataStreamToJSON(data_streams[i]));
    }
    jsonObj->setProperty("data_streams", var(dataStreamsArrayData));
    
    // Stimulus epochs array - ensure it's empty array, not null
    Array<var> stimulusEpochsArrayData;
    for (int i = 0; i < stimulus_epochs.size(); ++i)
    {
        stimulusEpochsArrayData.add(stimulus_epochs[i]);
    }
    jsonObj->setProperty("stimulus_epochs", var(stimulusEpochsArrayData));
    jsonObj->setProperty("subject_details", subjectDetailsToJSON (subject_details));
    
    return var(jsonObj);
}

var Acquisition::dataStreamToJSON(const DataStream& stream)
{
    DynamicObject* jsonObj = new DynamicObject();
    jsonObj->setProperty("object_type", stream.object_type);
    jsonObj->setProperty("stream_start_time", stream.stream_start_time.toISO8601(true));
    jsonObj->setProperty("stream_end_time", stream.stream_end_time.toISO8601(true));
    
    // Modalities array
    var modalitiesArray;
    for (int i = 0; i < stream.modalities.size(); ++i)
    {
        modalitiesArray.append(modalityToJSON(stream.modalities[i]));
    }
    jsonObj->setProperty("modalities", modalitiesArray);
    
    // Handle null code
    jsonObj->setProperty("code", stream.code);
    
    // Handle null notes
    jsonObj->setProperty("notes", stream.notes);
    
    // Active devices array
    var activeDevicesArray;
    for (int i = 0; i < stream.active_devices.size(); ++i)
    {
        activeDevicesArray.append(stream.active_devices[i]);
    }
    jsonObj->setProperty("active_devices", activeDevicesArray);
    
    // Configurations array
    var configurationsArray;
    for (int i = 0; i < stream.configurations.size(); ++i)
    {
        configurationsArray.append(ephysAssemblyConfigToJSON(stream.configurations[i]));
    }
    jsonObj->setProperty("configurations", configurationsArray);
    
    // Connections array - ensure it's empty array, not null
    Array<var> connectionsArrayData;
    for (int i = 0; i < stream.connections.size(); ++i)
    {
        connectionsArrayData.add(stream.connections[i]);
    }
    jsonObj->setProperty("connections", var(connectionsArrayData));
    
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

var Acquisition::coordinateSystemObjectToJSON(const CoordinateSystemObject& coordSystem)
{
    DynamicObject* jsonObj = new DynamicObject();
    jsonObj->setProperty("object_type", coordSystem.object_type);
    jsonObj->setProperty("name", coordSystem.name);
    jsonObj->setProperty("origin", coordSystem.origin);
    
    var axesArray;
    for (int i = 0; i < coordSystem.axes.size(); ++i)
    {
        axesArray.append(axisToJSON(coordSystem.axes[i]));
    }
    jsonObj->setProperty("axes", axesArray);
    
    jsonObj->setProperty("axis_unit", coordSystem.axis_unit);
    
    return var(jsonObj);
}

var Acquisition::axisToJSON(const Axis& axis)
{
    DynamicObject* jsonObj = new DynamicObject();
    jsonObj->setProperty("object_type", axis.object_type);
    jsonObj->setProperty("name", axis.name);
    jsonObj->setProperty("direction", axis.direction);
    return var(jsonObj);
}

var Acquisition::subjectDetailsToJSON(const SubjectDetails& details)
{
    DynamicObject* jsonObj = new DynamicObject();
    jsonObj->setProperty("object_type", details.object_type);
    jsonObj->setProperty("animal_weight_prior", details.animal_weight_prior);
    jsonObj->setProperty("animal_weight_post", details.animal_weight_post);
    jsonObj->setProperty("weight_unit", details.weight_unit);
    jsonObj->setProperty("anaesthesia", details.anaesthesia);
    jsonObj->setProperty("mouse_platform_name", details.mouse_platform_name);
    jsonObj->setProperty("reward_consumed_total", details.reward_consumed_total);
    jsonObj->setProperty("reward_consumed_unit", details.reward_consumed_unit);
    return var(jsonObj);
}

var Acquisition::translationToJSON(const Translation& translation)
{
    DynamicObject* jsonObj = new DynamicObject();
    jsonObj->setProperty("object_type", translation.object_type);
    
    var translationArray;
    for (int i = 0; i < translation.translation.size(); ++i)
    {
        translationArray.append(translation.translation[i]);
    }
    jsonObj->setProperty("translation", translationArray);
    
    return var(jsonObj);
}

var Acquisition::rotationToJSON(const Rotation& rotation)
{
    DynamicObject* jsonObj = new DynamicObject();
    jsonObj->setProperty("object_type", rotation.object_type);
    
    var anglesArray;
    for (int i = 0; i < rotation.angles.size(); ++i)
    {
        anglesArray.append(rotation.angles[i]);
    }
    jsonObj->setProperty("angles", anglesArray);
    jsonObj->setProperty("angles_unit", rotation.angles_unit);
    
    return var(jsonObj);
}

var Acquisition::modalityToJSON(const Modality& modality)
{
    DynamicObject* jsonObj = new DynamicObject();
    jsonObj->setProperty("name", modality.name);
    jsonObj->setProperty("abbreviation", modality.abbreviation);
    return var(jsonObj);
}

var Acquisition::probeConfigToJSON(const ProbeConfig& probe)
{
    DynamicObject* jsonObj = new DynamicObject();
    jsonObj->setProperty("object_type", probe.object_type);
    jsonObj->setProperty("device_name", probe.device_name);
    
    // Primary targeted structure
    DynamicObject* targetedStructure = new DynamicObject();
    targetedStructure->setProperty ("atlas", "CCFv3");
    targetedStructure->setProperty("name", probe.primary_targeted_structure.name);
    targetedStructure->setProperty("acronym", probe.primary_targeted_structure.acronym);
    targetedStructure->setProperty("id", probe.primary_targeted_structure.id);
    jsonObj->setProperty("primary_targeted_structure", var(targetedStructure));
    
    jsonObj->setProperty("other_targeted_structure", probe.other_targeted_structure);
    jsonObj->setProperty("atlas_coordinate", probe.atlas_coordinate);
    jsonObj->setProperty("coordinate_system", coordinateSystemObjectToJSON(probe.coordinate_system));
    
    Array<var> transformArrayData;
    for (int i = 0; i < probe.transform.size(); ++i)
    {
        transformArrayData.add(probe.transform[i]);
    }
    jsonObj->setProperty("transform", var(transformArrayData));
    
    jsonObj->setProperty("dye", probe.dye);
    jsonObj->setProperty("notes", probe.notes);
    
    return var(jsonObj);
}

var Acquisition::manipulatorConfigToJSON(const ManipulatorConfig& manipulator)
{
    DynamicObject* jsonObj = new DynamicObject();
    jsonObj->setProperty("object_type", manipulator.object_type);
    jsonObj->setProperty("device_name", manipulator.device_name);
    jsonObj->setProperty("coordinate_system", coordinateSystemObjectToJSON(manipulator.coordinate_system));
    jsonObj->setProperty("local_axis_positions", translationToJSON(manipulator.local_axis_positions));
    return var(jsonObj);
}

var Acquisition::misModuleConfigToJSON(const MISModuleConfig& module)
{
    DynamicObject* jsonObj = new DynamicObject();
    jsonObj->setProperty("object_type", module.object_type);
    jsonObj->setProperty("arc_angle", module.arc_angle);
    jsonObj->setProperty("module_angle", module.module_angle);
    jsonObj->setProperty("rotation_angle", module.rotation_angle);
    jsonObj->setProperty("angle_unit", module.angle_unit);
    jsonObj->setProperty("notes", module.notes);
    return var(jsonObj);
}

var Acquisition::ephysAssemblyConfigToJSON(const EphysAssemblyConfig& assembly)
{
    DynamicObject* jsonObj = new DynamicObject();
    jsonObj->setProperty("object_type", assembly.object_type);
    jsonObj->setProperty("device_name", assembly.device_name);
    jsonObj->setProperty("manipulator", manipulatorConfigToJSON(assembly.manipulator));
    
    var probesArray;
    for (int i = 0; i < assembly.probes.size(); ++i)
    {
        probesArray.append(probeConfigToJSON(assembly.probes[i]));
    }
    jsonObj->setProperty("probes", probesArray);
    
    var modulesArray;
    for (int i = 0; i < assembly.modules.size(); ++i)
    {
        modulesArray.append(misModuleConfigToJSON(assembly.modules[i]));
    }
    jsonObj->setProperty("modules", modulesArray);
    
    return var(jsonObj);
}

} // namespace Metadata
