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

/**
    The editor for the EphysMetadataGenerator

    Includes buttons for opening the canvas in a tab or window
*/

#define schemaVersion "2.0.34"

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
    String primaryTargetedStructure;
    
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
    Time startTime;
    
    /** End time */
    Time endTime;
    
    /** Active devices */
    StringArray activeDevices;
    
    /** Ephys assemblies */
    OwnedArray<EphysAssembly> ephysAssemblies;
    
};


struct Acquisition
{
    /** Subject ID*/
    String subjectID;
    
    /** Start time **/
    Time startTime;
    
    /** End time */
    Time endTime;
    
    /** Experimenters */
    StringArray experimenters;
    
    /** Instrument ID*/
    String instrumentID;
    
    /** Acquisition  type */
    String acquisitionType;
    
    /** Coordinate system*/
    CoordinateSystem coordinateSystem;
    
    /** Data streams*/
    OwnedArray<DataStream> dataStreams;
};

};

#endif // SESSION_H_DEFINED
