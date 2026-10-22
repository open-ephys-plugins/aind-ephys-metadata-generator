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

#ifndef ACQUISITION_H_DEFINED
#define ACQUISITION_H_DEFINED

#include <VisualizerEditorHeaders.h>
#include "CCFRegions.h"

/**
    The editor for the EphysMetadataGenerator

    Includes buttons for opening the canvas in a tab or window
*/

#define SCHEMA_VERSION "2.0.34"

namespace Metadata
{

enum CoordinateSystem
{
    BREGMA_ARID = 0,
    MPM_MANIP_RFB = 1
};

struct Axis
{
    /** Object type */
    String object_type = "Axis";
    
    /** Name */
    String name;
    
    /** Direction */
    String direction;
};

struct CoordinateSystemObject
{
    /** Object type */
    String object_type = "Coordinate system";
    
    /** Name */
    String name;
    
    /** Origin */
    String origin;
    
    /** Axes */
    Array<Axis> axes;
    
    /** Axis unit */
    String axis_unit = "millimeter";
};

struct Translation
{
    /** Object type */
    String object_type = "Translation";
    
    /** Translation values */
    Array<double> translation;
};

struct Rotation
{
    /** Object type */
    String object_type = "Rotation";
    
    /** Angles */
    Array<double> angles;
    
    /** Angles unit */
    String angles_unit = "degrees";
};

struct Modality
{
    /** Name */
    String name;
    
    /** Abbreviation */
    String abbreviation;
};

struct ProbeConfig
{
    /** Object type */
    String object_type = "Probe config";
    
    /** Device name */
    String device_name;
    
    /** Primary targeted structure */
    Region primary_targeted_structure;
    
    /** Other targeted structure */
    var other_targeted_structure; // can be null
    
    /** Atlas coordinate */
    var atlas_coordinate; // can be null
    
    /** Coordinate system */
    CoordinateSystemObject coordinate_system;
    
    /** Transform */
    Translation transform_translation; // Translation part of the transform
    Rotation transform_rotation; // Rotation part of the transform
    
    /** Dye */
    String dye;
    
    /** Notes */
    String notes;
    
    /** Hemisphere */
    String hemisphere;
    
    /** Implant hole number */
    String implant_hole_number;
    
    /** Inserted */
    bool inserted;
    
    /** On target */
    bool on_target;
    
    /** Recorded */
    bool recorded;
};

struct ManipulatorConfig
{
    /** Object type */
    String object_type = "Manipulator config";
    
    /** Device name */
    String device_name;
    
    /** Coordinate system */
    CoordinateSystemObject coordinate_system;
    
    /** Local axis positions */
    Translation local_axis_positions;
};

struct MISModuleConfig
{
    /** Object type */
    String object_type = "MIS module config";
    
    /** Arc angle */
    double arc_angle = 0.0;
    
    /** Module angle */
    double module_angle = 0.0;
    
    /** Rotation angle */
    double rotation_angle = 0.0;
    
    /** Angle unit */
    String angle_unit = "degrees";
    
    /** Notes */
    var notes; // can be null
};

struct EphysAssemblyConfig
{
    /** Object type */
    String object_type = "Ephys assembly config";
    
    /** Device name */
    String device_name;
    
    /** Manipulator */
    ManipulatorConfig manipulator;
    
    /** Probes */
    Array<ProbeConfig> probes;
    
    /** Modules */
    Array<MISModuleConfig> modules;
};

struct DataStream
{
    /** Object type */
    String object_type = "Data stream";
    
    /** Start time */
    Time stream_start_time;
    
    /** End time */
    Time stream_end_time;
    
    /** Modalities */
    Array<Modality> modalities;
    
    /** Code */
    var code; // can be null
    
    /** Notes */
    var notes; // can be null
    
    /** Active devices */
    StringArray active_devices;
    
    /** Configurations */
    Array<EphysAssemblyConfig> configurations;
    
    /** Connections */
    Array<var> connections; // Empty array in example

    /**
     * Write all acquisition data to a JSON file
     * @param filePath Path where the JSON file should be written
     * @return true if successful, false otherwise
     */
    bool writeToJSON(File file) const;
    
    /**
     * Convert the entire acquisition to a JSON object
     * @return var object containing all acquisition data
     */
    var toJSON() const;
    
private:
    /**
     * Convert CoordinateSystem enum to string
     * @param system The coordinate system enum value
     * @return String representation of the coordinate system
     */
    static String coordinateSystemToString(CoordinateSystem system);
    
    /**
     * Convert a CoordinateSystemObject struct to JSON
     * @param coordSystem The coordinate system to convert
     * @return var object containing coordinate system data
     */
    static var coordinateSystemObjectToJSON(const CoordinateSystemObject& coordSystem);
    
    /**
     * Convert an Axis struct to JSON
     * @param axis The axis to convert
     * @return var object containing axis data
     */
    static var axisToJSON(const Axis& axis);

    /**
     * Convert a Translation struct to JSON
     * @param translation The translation to convert
     * @return var object containing translation data
     */
    static var translationToJSON(const Translation& translation);
    
    /**
     * Convert a Rotation struct to JSON
     * @param rotation The rotation to convert
     * @return var object containing rotation data
     */
    static var rotationToJSON(const Rotation& rotation);
    
    /**
     * Convert a Modality struct to JSON
     * @param modality The modality to convert
     * @return var object containing modality data
     */
    static var modalityToJSON(const Modality& modality);
    
    /**
     * Convert a ProbeConfig struct to JSON
     * @param probe The probe config to convert
     * @return var object containing probe config data
     */
    static var probeConfigToJSON(const ProbeConfig& probe);
    
    /**
     * Convert a ManipulatorConfig struct to JSON
     * @param manipulator The manipulator config to convert
     * @return var object containing manipulator config data
     */
    static var manipulatorConfigToJSON(const ManipulatorConfig& manipulator);
    
    /**
     * Convert a MISModuleConfig struct to JSON
     * @param module The MIS module config to convert
     * @return var object containing MIS module config data
     */
    static var misModuleConfigToJSON(const MISModuleConfig& module);
    
    /**
     * Convert an EphysAssemblyConfig struct to JSON
     * @param assembly The ephys assembly config to convert
     * @return var object containing ephys assembly config data
     */
    static var ephysAssemblyConfigToJSON(const EphysAssemblyConfig& assembly);
};

};

#endif // SESSION_H_DEFINED
