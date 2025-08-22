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


struct Probe
{
    /** Probe name */
    String name;
    
    /** Primary targeted structure*/
    Region primary_targeted_structure;
    
    /** Surface Z */
    double surfaceZ = 0.0;
    
    /** Final Z*/
    double finalZ = 0.0;
    
    /** Dye */
    String dye;
    
    /** Notes */
    String notes;
    
};

struct MISModule
{
    /** Arc angle*/
    double arcAngle = 0.0;
    
    /** Module angle */
    double moduleAngle = 0.0;
    
    /** Rotation angle */
    double rotationAngle = 0.0;
    
};

struct Manipulator
{
    /** Name */
    String name;
    
    /** Coordinate system*/
    CoordinateSystem coordinateSystem = MPM_MANIP_RFB;
    
    /** X position */
    double xPosition = 0.0;
    
    /** Y position */
    double yPosition = 0.0;
    
    /** Z position */
    double zPosition = 0.0;
    

};

struct EphysAssembly
{
    /** Assembly name */
    String name;

    /** Manipulator */
    Manipulator manipulator;
    
    /** Probe */
    Probe probe;
    
    /** MIS module */
    MISModule misModule;

};

struct DataStream
{
    /** Start time **/
    Time stream_start_time;
    
    /** End time */
    Time stream_end_time;
    
    /** Active devices */
    StringArray activeDevices;
    
    /** Ephys assemblies */
    OwnedArray<EphysAssembly> ephysAssemblies;
    
};


struct Acquisition
{
    /** Object type */
    String object_type = "Acquisition";

    /** Described by */
    String describedBy = "https://raw.githubusercontent.com/AllenNeuralDynamics/aind-data-schema/main/src/aind_data_schema/core/acquisition.py";

    /** Schema version */
    String schema_version = SCHEMA_VERSION;

    /** Subject ID*/
    String subject_id;

    /** Specimen ID*/
    String specimen_id;
    
    /** Start time **/
    Time acquisition_start_time;
    
    /** End time */
    Time acquisition_end_time;
    
    /** Experimenters */
    StringArray experimenters;

    /** Protocol ID*/
    String protocol_id;

    /** Ethics review ID */
    StringArray ethics_review_id;
    
    /** Instrument ID*/
    String instrument_id;
    
    /** Acquisition type */
    String acquisition_type;
    
    /** Notes */
    String notes;

    /** Coordinate system*/
    CoordinateSystem coordinate_system;
    
    /** Data streams*/
    OwnedArray<DataStream> data_streams;
    
    /** Sets end time for all streams */
    void setEndTime (const Time& endTime)
    {
        this->acquisition_end_time = endTime;
        for (auto* stream : data_streams)
        {
            stream->stream_start_time = endTime;
        }
    }

    /**
     * Write all acquisition data to a JSON file
     * @param filePath Path where the JSON file should be written
     * @return true if successful, false otherwise
     */
    bool writeToJSON(File file) const;
    
private:
    /**
     * Convert the entire acquisition to a JSON object
     * @return var object containing all acquisition data
     */
    var toJSON() const;
    
    /**
     * Convert a Probe struct to JSON
     * @param probe The probe to convert
     * @return var object containing probe data
     */
    static var probeToJSON(const Probe& probe);
    
    /**
     * Convert a MISModule struct to JSON
     * @param module The MIS module to convert
     * @return var object containing module data
     */
    static var misModuleToJSON(const MISModule& module);
    
    /**
     * Convert a Manipulator struct to JSON
     * @param manipulator The manipulator to convert
     * @return var object containing manipulator data
     */
    static var manipulatorToJSON(const Manipulator& manipulator);
    
    /**
     * Convert an EphysAssembly struct to JSON
     * @param assembly The assembly to convert
     * @return var object containing assembly data
     */
    static var ephysAssemblyToJSON(const EphysAssembly& assembly);
    
    /**
     * Convert a DataStream struct to JSON
     * @param stream The data stream to convert
     * @return var object containing stream data
     */
    static var dataStreamToJSON(const DataStream& stream);
    
    /**
     * Convert CoordinateSystem enum to string
     * @param system The coordinate system enum value
     * @return String representation of the coordinate system
     */
    static String coordinateSystemToString(CoordinateSystem system);
};

};

#endif // SESSION_H_DEFINED
