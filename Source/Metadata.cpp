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

#include "Metadata.h"

namespace Metadata
{

EphysAssemblyConfig makeAssemblyLocation(const ProbeLocation& location)
{
    EphysAssemblyConfig assembly;
    assembly.device_name = "Ephys Assembly " + location.probe_name;
    assembly.manipulator.device_name = "Manipulator " + location.probe_name;
    assembly.manipulator.coordinate_system.name = "MIS_MANIP_RPI";
    assembly.manipulator.coordinate_system.origin = "Origin";
    assembly.manipulator.coordinate_system.axes.add({ "Axis", "X", "Left_to_right" });
    assembly.manipulator.coordinate_system.axes.add({ "Axis", "Y", "Anterior_to_posterior" });
    assembly.manipulator.coordinate_system.axes.add({ "Axis", "Z", "Superior_to_inferior" });
    assembly.manipulator.local_coordinate_system = assembly.manipulator.coordinate_system;
    assembly.manipulator.local_axis_positions.translation = { location.x, location.y, location.z };

    MISModuleConfig module;
    module.arc_angle = location.arc_angle;
    module.module_angle = location.module_angle;
    assembly.modules.add(module);

    ProbeConfig probe;
    probe.device_name = location.probe_name;
    probe.coordinate_system.name = "PROBE_RUF";
    probe.coordinate_system.origin = "Tip";
    probe.coordinate_system.axes.add({ "Axis", "X", "Left_to_right" });
    probe.coordinate_system.axes.add({ "Axis", "Y", "Down_to_up" });
    probe.coordinate_system.axes.add({ "Axis", "Z", "Back_to_front" });
    probe.local_coordinate_system = probe.coordinate_system;

    auto addRotation = [&probe](std::initializer_list<double> angles,
                                const String& frame, const String& direction)
    {
        Rotation rotation;
        rotation.angles.addArray(angles.begin(), static_cast<int>(angles.size()));
        rotation.reference_coordinate_system = frame;
        rotation.rotation_direction = direction;
        probe.transform.emplace_back(rotation);
    };
    auto addTranslation = [&probe](std::initializer_list<double> values)
    {
        Translation translation;
        translation.translation.addArray(values.begin(), static_cast<int>(values.size()));
        translation.reference_coordinate_system = "local";
        probe.transform.emplace_back(translation);
    };

    // Transform order and handedness follow Resources/probeMetadata.py.
    addRotation({ 90.0, 0.0, -90.0 }, "global", "right_hand");
    addRotation({ location.arc_angle - 14.0, 0.0, 0.0 }, "global", "left_hand");
    addRotation({ location.module_angle, 0.0, 0.0 }, "local", "left_hand");
    addRotation({ 0.0, location.spin_angle, 0.0 }, "local", "left_hand");
    addTranslation({ location.y - location.bregma_y,
                     -(location.z - location.bregma_z),
                     -(location.x - location.bregma_x) });
    addTranslation({ 0.0, -(location.z - location.surface_z), 0.0 });
    assembly.probes.add(probe);

    return assembly;
}

bool DataStream::writeToJSON(File file) const
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

var DataStream::toJSON() const
{
    DynamicObject* jsonObj = new DynamicObject();
    jsonObj->setProperty("object_type", object_type);
    jsonObj->setProperty("stream_start_time", stream_start_time.toISO8601(true));
    jsonObj->setProperty("stream_end_time", stream_end_time.toISO8601(true));
    
    // Modalities array
    var modalitiesArray;
    for (int i = 0; i < modalities.size(); ++i)
    {
        modalitiesArray.append(modalityToJSON(modalities[i]));
    }
    jsonObj->setProperty("modalities", modalitiesArray);
    
    // Handle null code
    jsonObj->setProperty("code", code);
    
    // Handle null notes
    jsonObj->setProperty("notes", notes);
    
    // Active devices array
    var activeDevicesArray;
    for (int i = 0; i < active_devices.size(); ++i)
    {
        activeDevicesArray.append(active_devices[i]);
    }
    jsonObj->setProperty("active_devices", activeDevicesArray);
    
    // Configurations array
    var configurationsArray;
    for (int i = 0; i < configurations.size(); ++i)
    {
        configurationsArray.append(ephysAssemblyConfigToJSON(configurations[i]));
    }
    jsonObj->setProperty("configurations", configurationsArray);
    
    // Connections array - ensure it's empty array, not null
    Array<var> connectionsArrayData;
    for (int i = 0; i < connections.size(); ++i)
    {
        connectionsArrayData.add(connections[i]);
    }
    jsonObj->setProperty("connections", var(connectionsArrayData));
    
    return var(jsonObj);
}

String DataStream::coordinateSystemToString(CoordinateSystem system)
{
    switch (system)
    {
        case BREGMA_RAS:
            return "BREGMA_RAS";
        case MIS_MANIP_RPI:
            return "MIS_MANIP_RPI";
        default:
            return "UNKNOWN";
    }
}

var DataStream::coordinateSystemObjectToJSON(const CoordinateSystemObject& coordSystem)
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
    jsonObj->setProperty("handedness", coordSystem.handedness);
    
    return var(jsonObj);
}

var DataStream::axisToJSON(const Axis& axis)
{
    DynamicObject* jsonObj = new DynamicObject();
    jsonObj->setProperty("object_type", axis.object_type);
    jsonObj->setProperty("name", axis.name);
    jsonObj->setProperty("direction", axis.direction);
    return var(jsonObj);
}

var DataStream::translationToJSON(const Translation& translation)
{
    DynamicObject* jsonObj = new DynamicObject();
    jsonObj->setProperty("object_type", translation.object_type);
    
    var translationArray;
    for (int i = 0; i < translation.translation.size(); ++i)
    {
        translationArray.append(translation.translation[i]);
    }
    jsonObj->setProperty("translation", translationArray);
    jsonObj->setProperty("reference_coordinate_system", translation.reference_coordinate_system);
    
    return var(jsonObj);
}

var DataStream::rotationToJSON(const Rotation& rotation)
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
    jsonObj->setProperty("axis_order", rotation.axis_order);
    jsonObj->setProperty("reference_coordinate_system", rotation.reference_coordinate_system);
    jsonObj->setProperty("rotation_direction", rotation.rotation_direction);
    jsonObj->setProperty("pivot", rotation.pivot);
    
    return var(jsonObj);
}

var DataStream::modalityToJSON(const Modality& modality)
{
    DynamicObject* jsonObj = new DynamicObject();
    jsonObj->setProperty("name", modality.name);
    jsonObj->setProperty("abbreviation", modality.abbreviation);
    return var(jsonObj);
}

var DataStream::probeConfigToJSON(const ProbeConfig& probe)
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
    jsonObj->setProperty("local_coordinate_system", coordinateSystemObjectToJSON(probe.local_coordinate_system));
    
    Array<var> transformArrayData;
    for (const auto& transform : probe.transform)
    {
        if (const auto* rotation = std::get_if<Rotation>(&transform))
            transformArrayData.add(rotationToJSON(*rotation));
        else if (const auto* translation = std::get_if<Translation>(&transform))
            transformArrayData.add(translationToJSON(*translation));
    }

    jsonObj->setProperty("transform", var(transformArrayData));
    
    jsonObj->setProperty("dye", probe.dye);

    String notesForExport = probe.notes;
    if (notesForExport.isNotEmpty())
        notesForExport += "; ";
    notesForExport += "Hemisphere: ";
    notesForExport += probe.hemisphere.isEmpty() ? "None" : probe.hemisphere;
    notesForExport += "; Hole #: ";
    notesForExport += probe.implant_hole_number.isEmpty() ? "N/A" : probe.implant_hole_number;
    notesForExport += "; Inserted: ";
    notesForExport += (probe.inserted ? "true" : "false");
    notesForExport += "; On Target: ";
    notesForExport += (probe.on_target ? "true" : "false");
    notesForExport += "; Recorded: ";
    notesForExport += (probe.recorded ? "true" : "false");
    jsonObj->setProperty("notes", notesForExport);
    
    return var(jsonObj);
}

var DataStream::manipulatorConfigToJSON(const ManipulatorConfig& manipulator)
{
    DynamicObject* jsonObj = new DynamicObject();
    jsonObj->setProperty("object_type", manipulator.object_type);
    jsonObj->setProperty("device_name", manipulator.device_name);
    jsonObj->setProperty("coordinate_system", coordinateSystemObjectToJSON(manipulator.coordinate_system));
    jsonObj->setProperty("local_coordinate_system", coordinateSystemObjectToJSON(manipulator.local_coordinate_system));
    jsonObj->setProperty("local_axis_positions", translationToJSON(manipulator.local_axis_positions));
    return var(jsonObj);
}

var DataStream::misModuleConfigToJSON(const MISModuleConfig& module)
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

var DataStream::ephysAssemblyConfigToJSON(const EphysAssemblyConfig& assembly)
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
