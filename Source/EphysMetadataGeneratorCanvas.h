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

#ifndef EPHYSMETADATAGENERATORCANVAS_H_INCLUDED
#define EPHYSMETADATAGENERATORCANVAS_H_INCLUDED

#include <VisualizerWindowHeaders.h>
#include "TableComponents.h"
#include "StageListener.h"

class EphysMetadataGenerator;

/**
*
    Draws data in real time

*/
class EphysMetadataGeneratorCanvas : public Visualizer,
                                     public TableListBoxModel,
                                     public ComboBox::Listener,
                                     public Label::Listener,
                                     public Button::Listener,
                                     public StagePositionListener
{
public:
    /** Constructor */
    EphysMetadataGeneratorCanvas (EphysMetadataGenerator* processor);

    /** Destructor */
    ~EphysMetadataGeneratorCanvas();

    /** Updates boundaries of sub-components whenever the canvas size changes */
    void resized() override;

    /** Called by the update() method to allow the visualizer to update its custom settings.*/
    void updateSettings() override;

    /** Called when the visualizer's tab becomes visible again */
    void refreshState() override;

    /** Called instead of "repaint()" to avoid re-painting sub-components*/
    void refresh() override;

    /** Draws the canvas background */
    void paint (Graphics& g) override;

    /** TableListBoxModel methods */
    int getNumRows() override;
    void paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
    void paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate) override;

private:
    /** Pointer to the processor class */
    EphysMetadataGenerator* processor;

    /** Table component for displaying metadata */
    std::unique_ptr<TableListBox> metadataTable;

    /** Sample data structure for table rows */
    struct ProbeMetadata {
        String probe;
        String manipulator;
        String arcAngle;
        String moduleAngle;
        String rotation;
        String x;
        String y;
        String z;
        String surface;
        String targetStructure;
        String dye;
        String notes;
    };

    /** Array to hold the probe metadata */
    Array<ProbeMetadata> probeData;

    /** Initialize the table with columns and sample data */
    void initializeTable();

    /** Handle ComboBox changes */
    void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;

    /** Handle Label text changes */
    void labelTextChanged(Label* labelThatHasChanged) override;

    /** Validate numeric input for angle columns */
    bool isValidAngleInput(const String& text);

    /** Handle Button clicks */
    void buttonClicked(Button* buttonThatWasClicked) override;
    
    // StagePositionListener interface
    /** Called when new stage positions are received from the HTTP endpoint */
    void stagePositionsUpdated(const Array<StagePosition>& positions) override;
    
    /** Called when there's an error fetching stage positions */
    void stagePositionError(const String& errorMessage) override;
    
    /** Stage listener for fetching X, Y, Z coordinates */
    std::unique_ptr<StageListener> stageListener;

    /** Generates an assertion if this class leaks */
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EphysMetadataGeneratorCanvas);
};

#endif // EPHYSMETADATAGENERATORCANVAS_H_INCLUDED
