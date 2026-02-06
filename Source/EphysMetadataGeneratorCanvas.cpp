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

#include "EphysMetadataGeneratorCanvas.h"

#include "EphysMetadataGenerator.h"
#include "CCFRegions.h"


EphysMetadataGeneratorCanvas::EphysMetadataGeneratorCanvas(EphysMetadataGenerator* processor_)
    : processor(processor_)
{
    // Create and configure the metadata table
    metadataTable = std::make_unique<TableListBox>();
    metadataTable->setModel(this);
    metadataTable->setColour(ListBox::outlineColourId, Colours::grey);
    metadataTable->setOutlineThickness(1);
    metadataTable->setHeaderHeight (30);
    metadataTable->setRowHeight (26);
    metadataTable->setRowSelectedOnMouseDown (false);
    metadataTable->setAutoSizeMenuOptionShown (false);
    metadataTable->getViewport()->setScrollBarThickness (12);
    metadataTable->setInterceptsMouseClicks (false, true); // Allow clicks to pass through to cells, but not the header
    addAndMakeVisible(metadataTable.get());

    tableCellFont = FontOptions("Inter", "Medium", 16.0f);
    
    // Create and configure the Add Row button
    addRowButton = std::make_unique<TextButton>("Add Row");
    addRowButton->setName("AddRowButton");
    addRowButton->addListener(this);
    addAndMakeVisible(addRowButton.get());
    
    // Create and configure the server address components
    serverAddressLabel = std::make_unique<Label>("Server Address Label", "HTTP Server:");
    serverAddressLabel->setFont (FontOptions ("Inter", "Semi Bold", 16.0f));
    serverAddressLabel->setJustificationType(Justification::centredLeft);
    serverAddressLabel->setColour(Label::textColourId, findColour(ThemeColours::controlPanelText));
    addAndMakeVisible(serverAddressLabel.get());
    
    serverAddressEditor = std::make_unique<TextEditor>("Server Address");
    serverAddressEditor->setText("10.128.49.150:8080", false);
    serverAddressEditor->setJustification(Justification::centred);
    serverAddressEditor->setInputRestrictions (0, "0123456789.:");
    serverAddressEditor->onTextChange = [this]()
    {
        if (connectButton != nullptr)
        {
            connectButton->setButtonText("Connect");
            connectButton->setColour(TextButton::buttonColourId, findColour (ThemeColours::widgetBackground));
            connectButton->setColour(TextButton::textColourOnId, findColour (ThemeColours::defaultText));
            connectButton->setColour(TextButton::textColourOffId, findColour (ThemeColours::defaultText));
        }
    };

    addAndMakeVisible(serverAddressEditor.get());
    
    connectButton = std::make_unique<TextButton>("Connect");
    connectButton->setName("ConnectButton");
    connectButton->addListener(this);
    connectButton->setColour(TextButton::buttonColourId, findColour (ThemeColours::widgetBackground));
    connectButton->setColour(TextButton::textColourOnId, findColour (ThemeColours::defaultText));
    connectButton->setColour(TextButton::textColourOffId, findColour (ThemeColours::defaultText));
    addAndMakeVisible(connectButton.get());
    
    // Add columns to the table
    metadataTable->getHeader().addColumn("Probe", 1, 80, 80, 300, TableHeaderComponent::visible | TableHeaderComponent::resizable);
    metadataTable->getHeader().addColumn("Manipulator", 2, 100, 100, 300, TableHeaderComponent::visible | TableHeaderComponent::resizable);
    metadataTable->getHeader().addColumn("Arc Angle", 3, 80, 80, 80, TableHeaderComponent::visible);
    metadataTable->getHeader().addColumn("Module Angle", 4, 100, 100, 100, TableHeaderComponent::visible);
    metadataTable->getHeader().addColumn("Rotation", 5, 80, 80, 80, TableHeaderComponent::visible);
    metadataTable->getHeader().addColumn("X", 6, 60, 60, 60, TableHeaderComponent::visible);
    metadataTable->getHeader().addColumn("Y", 7, 60, 60, 60, TableHeaderComponent::visible);
    metadataTable->getHeader().addColumn("Z", 8, 60, 60, 60, TableHeaderComponent::visible);
    metadataTable->getHeader().addColumn("Surface", 9, 100, 100, 100, TableHeaderComponent::visible);
    metadataTable->getHeader().addColumn("Bregma X", 10, 70, 70, 70, TableHeaderComponent::visible);
    metadataTable->getHeader().addColumn("Bregma Y", 11, 70, 70, 70, TableHeaderComponent::visible);
    metadataTable->getHeader().addColumn("Bregma Z", 12, 70, 70, 70, TableHeaderComponent::visible);
    metadataTable->getHeader().addColumn("Target Structure", 13, 120, 120, 120, TableHeaderComponent::visible);
    metadataTable->getHeader().addColumn("Dye", 14, 80, 80, 80, TableHeaderComponent::visible);
    metadataTable->getHeader().addColumn("Notes", 15, 150, 100, 400, TableHeaderComponent::visible | TableHeaderComponent::resizable);
    metadataTable->getHeader().addColumn("Hemisphere", 16, 80, 80, 80, TableHeaderComponent::visible);
    metadataTable->getHeader().addColumn("Implant Hole #", 17, 90, 90, 90, TableHeaderComponent::visible);
    metadataTable->getHeader().addColumn("Inserted?", 18, 70, 70, 70, TableHeaderComponent::visible);
    metadataTable->getHeader().addColumn("On Target?", 19, 80, 80, 80, TableHeaderComponent::visible);
    metadataTable->getHeader().addColumn("Recorded?", 20, 80, 80, 80, TableHeaderComponent::visible);
    metadataTable->getHeader().addColumn("Delete", 21, 50, 50, 50, TableHeaderComponent::visible);

    metadataTable->getHeader().setPopupMenuActive(false); // Disable default column visibility menu since we have a custom one
    metadataTable->getHeader().addMouseListener (this, true); // Listen for mouse events on the header to show custom context menu

    // Create some initial values
    initializeTable();
    
    // Create and start the stage listener
    stageListener = std::make_unique<StageListener>(this);
    stageListener->startListening();

    connectButton->setButtonText("Connecting...");
    connectButton->setColour(TextButton::buttonColourId, Colours::lightgreen);
    connectButton->setColour(TextButton::textColourOnId, Colours::black);
    connectButton->setColour(TextButton::textColourOffId, Colours::black);
}


EphysMetadataGeneratorCanvas::~EphysMetadataGeneratorCanvas()
{

}


void EphysMetadataGeneratorCanvas::resized()
{
    const int buttonHeight = 30;
    const int margin = 10;
    const int bottomControlsHeight = 40; // Height reserved for server controls and Add Row button

    if (metadataTable != nullptr)
    {
        // Calculate available space for table (leave room for bottom controls)
        int tableHeight = getHeight() - (bottomControlsHeight + 2 * margin);

        // Position the table at the top
        metadataTable->setBounds(margin, margin, getWidth() - 2 * margin, tableHeight);

        // Position server controls and Add Row button at bottom
        if (serverAddressLabel != nullptr && serverAddressEditor != nullptr && connectButton != nullptr && addRowButton != nullptr)
        {
            int labelWidth = 100;
            int editorWidth = 200;
            int connectButtonWidth = 200;
            int addButtonWidth = 150;
            int totalControlsWidth = labelWidth + editorWidth + connectButtonWidth + addButtonWidth + 3 * margin;
            int startX = (getWidth() - totalControlsWidth) / 2;
            int bottomY = margin + tableHeight + margin;

            serverAddressLabel->setBounds(startX, bottomY, labelWidth, buttonHeight);
            serverAddressEditor->setBounds(startX + labelWidth + margin, bottomY, editorWidth, buttonHeight);
            connectButton->setBounds(startX + labelWidth + editorWidth + 2 * margin, bottomY, connectButtonWidth, buttonHeight);
            addRowButton->setBounds(startX + labelWidth + editorWidth + connectButtonWidth + 3 * margin, bottomY, addButtonWidth, buttonHeight);
        }
    }
}

void EphysMetadataGeneratorCanvas::lookAndFeelChanged()
{
    // Call the base class implementation first
    Visualizer::lookAndFeelChanged();
    
    // Update colors for components that use theme colors
    if (serverAddressLabel != nullptr)
    {
        serverAddressLabel->setColour(Label::textColourId, findColour(ThemeColours::controlPanelText));
    }
    
    if (serverAddressEditor != nullptr)
    {
        serverAddressEditor->applyColourToAllText (findColour (ThemeColours::defaultText));
    }

    if (connectButton != nullptr && connectButton->getButtonText() == "Connect")
    {
        connectButton->setColour(TextButton::buttonColourId, findColour (ThemeColours::widgetBackground));
        connectButton->setColour(TextButton::textColourOnId, findColour (ThemeColours::defaultText));
        connectButton->setColour(TextButton::textColourOffId, findColour (ThemeColours::defaultText));
    }
    
    // Force table to update its components by refreshing the content
    // This will cause refreshComponentForCell to be called for all cells,
    // which will update colors for labels and other components
    if (metadataTable != nullptr)
    {
        metadataTable->updateContent();
        metadataTable->repaint();
    }
    
    // Repaint the entire canvas to ensure all colors are updated
    repaint();
}

void EphysMetadataGeneratorCanvas::updateSettings()
{
    // Get all data streams from the processor
    Array<const DataStream*> dataStreams = processor->getDataStreams();
    
    // Create a list of Neuropixels probe names
    Array<String> neuropixelsProbeNames;

    // Keep track of probe serial numbers already added
    Array<String> addedSerialNumbers;
    
    for (auto stream : dataStreams)
    {
        String streamName = "Unknown";
        
        if (stream->device != nullptr)
        {
            // Check if this stream has neuropixels metadata
            int adcMetadataIndex = stream->device->findMetadata(
                MetadataDescriptor::MetadataType::UINT16,
                1,
                "neuropixels.adcs");

            const String serialNumber = stream->device->serial_number;
            
            if (adcMetadataIndex > -1 && !addedSerialNumbers.contains(serialNumber))
            {
                // This is a Neuropixels stream with a unique serial number
                streamName = stream->getName();
                if (streamName.contains("-AP"))
                    streamName = streamName.dropLastCharacters (3);
                else if (streamName.contains("-LFP"))
                    streamName = streamName.dropLastCharacters (4);

                neuropixelsProbeNames.add(streamName);
                
                LOGD("Found Neuropixels stream: ", streamName);
                addedSerialNumbers.add(serialNumber);
            }
        }
    }
    
    // Update the probe metadata array to match the number of Neuropixels probes
    // Keep existing metadata if probe serial numbers match, otherwise create new entries
    // Always preserve manually-created probes
    Array<ProbeMetadata> updatedMetadata;
    
    for (int i = 0; i < addedSerialNumbers.size(); ++i)
    {
        String serialNumber = addedSerialNumbers[i];
        String probeName = neuropixelsProbeNames[i];
        
        // Check if we already have metadata for this probe (non-manual only)
        int existingIndex = -1;
        for (int j = 0; j < probeMetadata.size(); ++j)
        {
            if (!probeMetadata[j].isManuallyCreated && probeMetadata[j].serialNumber == serialNumber)
            {
                existingIndex = j;
                break;
            }
        }
        
        if (existingIndex >= 0)
        {
            // Keep existing metadata
            updatedMetadata.add(probeMetadata[existingIndex]);
        }
        else
        {
            // Create new metadata with defaults
            ProbeMetadata newProbe;
            newProbe.probe = probeName;
            newProbe.serialNumber = serialNumber;
            newProbe.manipulator = "";
            newProbe.arcAngle = "0";
            newProbe.moduleAngle = "0";
            newProbe.rotation = "0";
            newProbe.x = "0.0";
            newProbe.y = "0.0";
            newProbe.z = "0.0";
            newProbe.surface = "0.0";
            newProbe.bregmaX = "0.0";
            newProbe.bregmaY = "0.0";
            newProbe.bregmaZ = "0.0";
            newProbe.targetStructure = "";
            newProbe.dye = "CM-DiI";
            newProbe.notes = "";
            newProbe.hemisphere = "";
            newProbe.implantHoleNumber = "0";
            newProbe.inserted = true;
            newProbe.onTarget = true;
            newProbe.recorded = true;
            newProbe.isManuallyCreated = false;
            
            updatedMetadata.add(newProbe);
            LOGD("Created new metadata for probe: ", probeName);
        }

        maxStreamNameWidth = jmax(maxStreamNameWidth, (int)TextLayout::getStringWidth (tableCellFont, probeName) + 20);
    }

    // Add all manually-created probes (they persist regardless of streams)
    for (int j = 0; j < probeMetadata.size(); ++j)
    {
        if (probeMetadata[j].isManuallyCreated)
        {
            updatedMetadata.add(probeMetadata[j]);
        }
    }
    
    // Replace the probe metadata array
    probeMetadata = updatedMetadata;
    
    // Update the table display
    if (metadataTable != nullptr)
    {
        metadataTable->updateContent();
        metadataTable->repaint();
        metadataTable->autoSizeColumn (1); // Auto-size the Probe column
    }
    
    LOGD("Updated probe metadata table with ", probeMetadata.size(), " probes");
}


void EphysMetadataGeneratorCanvas::paint(Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour (ThemeColours::componentParentBackground));
}

// TableListBoxModel methods
int EphysMetadataGeneratorCanvas::getNumRows()
{
    return probeMetadata.size();
}

void EphysMetadataGeneratorCanvas::paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    Colour rowColour = findColour (ThemeColours::componentBackground);

    if (rowNumber % 2 == 0)
        g.fillAll (rowColour);
    else
        g.fillAll (rowColour.darker (0.15f));
}

void EphysMetadataGeneratorCanvas::paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    g.setColour(getLookAndFeel().findColour(Label::textColourId));
    g.setFont(tableCellFont);
    
    if (rowNumber < probeMetadata.size())
    {
        // Draw text for non-editable columns
        if (columnId == 1 || columnId == 6 || columnId == 7 || columnId == 8)
        {
            String text;
            const ProbeMetadata& row = probeMetadata[rowNumber];
            switch (columnId)
            {
                case 1: text = row.probe; break;
                case 6: text = row.x; break;
                case 7: text = row.y; break;
                case 8: text = row.z; break;
                default: break;
            }
            g.drawText(text, 5, 0, width - 10, height, Justification::centredLeft, true);
        }
    }
}

void EphysMetadataGeneratorCanvas::initializeTable()
{
    // Table will be populated by updateSettings() when data streams are available
    // No sample data needed - probes will be detected automatically from Neuropixels streams
    
    metadataTable->updateContent();
}

Component* EphysMetadataGeneratorCanvas::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate)
{
    // ComboBox for the Manipulator column (column 2)
    if (columnId == 2)
    {
        ComboBox* comboBox = dynamic_cast<ComboBox*>(existingComponentToUpdate);
        
        if (comboBox == nullptr)
        {
            comboBox = new ComboBox();
            comboBox->setName("Manipulator_" + String(rowNumber));
            comboBox->addListener(this);
        }
        
        // Clear existing items and repopulate with available serial numbers
        comboBox->clear();
        
        // Add an empty/null option first
        comboBox->addItem("<Select Manipulator>", 1);
        
        // Add available serial numbers from the stage server
        for (int i = 0; i < availableSerialNumbers.size(); ++i)
        {
            comboBox->addItem(availableSerialNumbers[i], i + 2); // Start from ID 2
        }
        
        // If no serial numbers are available, add fallback options
        if (availableSerialNumbers.isEmpty())
        {
            comboBox->addItem("SN00000", 2);
        }
        
        // Set the current selection based on the data
        if (rowNumber < probeMetadata.size())
        {
            String currentValue = probeMetadata[rowNumber].manipulator;
            
            if (currentValue.isEmpty())
            {
                // Select the null option
                comboBox->setSelectedId(1);
            }
            else
            {
                // Find the matching serial number
                bool found = false;
                for (int i = 0; i < availableSerialNumbers.size(); ++i)
                {
                    if (availableSerialNumbers[i] == currentValue)
                    {
                        comboBox->setSelectedId(i + 2); // +2 because we start from ID 2
                        found = true;
                        break;
                    }
                }
                
                // If not found in available serial numbers but we have saved serial numbers,
                // add the current value as an option (marked as potentially invalid)
                if (!found)
                {
                    if (!availableSerialNumbers.isEmpty())
                    {
                        // Add the current value as a special option to preserve it
                        comboBox->addItem(currentValue + " (saved)", availableSerialNumbers.size() + 2);
                        comboBox->setSelectedId(availableSerialNumbers.size() + 2);
                    }
                }
            }
        }
        
        return comboBox;
    }
    
    // Editable Labels for angle columns (Arc Angle=3, Module Angle=4, Rotation=5)
    if (columnId == 3 || columnId == 4 || columnId == 5)
    {
        Label* label = dynamic_cast<Label*>(existingComponentToUpdate);
        
        if (label == nullptr)
        {
            label = new Label();
            label->setEditable(true);
            label->setJustificationType(Justification::centred);
            
            // Set name to identify which column this label belongs to
            if (columnId == 3) label->setName("ArcAngle_" + String(rowNumber));
            else if (columnId == 4) label->setName("ModuleAngle_" + String(rowNumber));
            else if (columnId == 5) label->setName("Rotation_" + String(rowNumber));
            
            // Add validation on text change
            label->onTextChange = [this, label, rowNumber, columnId]()
            {
                String newText = label->getText();
                
                // Validate the input
                if (!isValidAngleInput(newText))
                {
                    // Revert to previous valid value
                    if (rowNumber < probeMetadata.size())
                    {
                        String validValue;
                        if (columnId == 3) validValue = probeMetadata[rowNumber].arcAngle;
                        else if (columnId == 4) validValue = probeMetadata[rowNumber].moduleAngle;
                        else if (columnId == 5) validValue = probeMetadata[rowNumber].rotation;
                        
                        label->setText(validValue, dontSendNotification);
                        return;
                    }
                }
                
                // Update the data if valid
                if (rowNumber < probeMetadata.size())
                {
                    if (columnId == 3) probeMetadata.getReference(rowNumber).arcAngle = newText;
                    else if (columnId == 4) probeMetadata.getReference(rowNumber).moduleAngle = newText;
                    else if (columnId == 5) probeMetadata.getReference(rowNumber).rotation = newText;
                    
                    LOGD("Angle value for row ", rowNumber, " column ", columnId, " changed to: ", newText);
                }
            };
        }
        
        // Set the current text based on the data
        if (rowNumber < probeMetadata.size())
        {
            String currentValue;
            if (columnId == 3) currentValue = probeMetadata[rowNumber].arcAngle;
            else if (columnId == 4) currentValue = probeMetadata[rowNumber].moduleAngle;
            else if (columnId == 5) currentValue = probeMetadata[rowNumber].rotation;
            
            label->setText(currentValue, dontSendNotification);
        }
        
        return label;
    }
    
    // Toggle button for the Surface column (column 9)
    if (columnId == 9)
    {
        TextButton* button = dynamic_cast<TextButton*>(existingComponentToUpdate);
        
        if (button == nullptr)
        {
            button = new TextButton();
            button->setName("SurfaceButton_" + String(rowNumber));
            button->addListener(this);
        }
        
        // Set the button appearance based on whether surface has been set from Z
        if (rowNumber < probeMetadata.size())
        {
            String currentSurface = probeMetadata[rowNumber].surface;
            String currentZ = probeMetadata[rowNumber].z;
            
            // Check if surface value matches Z value (indicating it was copied)
            bool isSurfaceSetFromZ = (currentSurface == currentZ && currentSurface != "0.0" && !currentSurface.isEmpty());
            
            if (isSurfaceSetFromZ)
            {
                // Button is "active" - show green and display the surface value
                button->setButtonText(currentSurface);
                button->setColour(TextButton::buttonColourId, Colours::green);
                button->setColour(TextButton::textColourOnId, Colours::white);
            }
            else
            {
                // Button is "inactive" - show default appearance
                if (currentSurface.isEmpty() || currentSurface == "0.0")
                {
                    button->setButtonText("Set Surface");
                }
                else
                {
                    button->setButtonText(currentSurface);
                }
                button->setColour(TextButton::buttonColourId, findColour (ThemeColours::widgetBackground));
                button->setColour(TextButton::textColourOnId, findColour (ThemeColours::defaultText));
            }
        }
        
        return button;
    }
    
    // TextEditor for the Bregma coordinate columns (Bregma X=10, Bregma Y=11, Bregma Z=12)
    if (columnId >= 10 && columnId <= 12)
    {
        Label* bregmaEditor = dynamic_cast<Label*>(existingComponentToUpdate);
        
        if (bregmaEditor == nullptr)
        {
            bregmaEditor = new Label();
            bregmaEditor->setEditable(true);
            bregmaEditor->setJustificationType(Justification::centred);

            String fieldName;
            if (columnId == 10)
                fieldName = "BregmaXEditor_" + String(rowNumber);
            else if (columnId == 11)
                fieldName = "BregmaYEditor_" + String(rowNumber);
            else
                fieldName = "BregmaZEditor_" + String(rowNumber);

            bregmaEditor->setName(fieldName);

            bregmaEditor->onTextChange = [this, bregmaEditor, rowNumber, columnId]()
            {
                if (rowNumber < probeMetadata.size())
                {
                    String newText = bregmaEditor->getText();

                    if (!isValidCoordinateInput(newText))
                    {
                        // Revert to previous valid value
                        String validValue;
                        if (columnId == 10) validValue = probeMetadata[rowNumber].bregmaX;
                        else if (columnId == 11) validValue = probeMetadata[rowNumber].bregmaY;
                        else if (columnId == 12) validValue = probeMetadata[rowNumber].bregmaZ;

                        bregmaEditor->setText(validValue, dontSendNotification);
                        return;
                    }

                    ProbeMetadata& metadata = probeMetadata.getReference(rowNumber);

                    if (columnId == 10)
                        metadata.bregmaX = newText;
                    else if (columnId == 11)
                        metadata.bregmaY = newText;
                    else
                        metadata.bregmaZ = newText;
                }
            };
        }

        if (rowNumber < probeMetadata.size())
        {
            String currentValue;
            if (columnId == 10)
                currentValue = probeMetadata[rowNumber].bregmaX;
            else if (columnId == 11)
                currentValue = probeMetadata[rowNumber].bregmaY;
            else
                currentValue = probeMetadata[rowNumber].bregmaZ;

            bregmaEditor->setText(currentValue, dontSendNotification);
        }

        return bregmaEditor;
    }

    // AutoCompleteTextEditor for the Target Structure column (column 13)
    if (columnId == 13)
    {
        AutoCompleteTextEditor* autoCompleteEditor = dynamic_cast<AutoCompleteTextEditor*>(existingComponentToUpdate);
        
        if (autoCompleteEditor == nullptr)
        {
            // Create common brain region suggestions
            CCFRegionList regionList;
            
            autoCompleteEditor = new AutoCompleteTextEditor(regionList.getRegionAcronyms());
            autoCompleteEditor->setName("TargetStructure_" + String(rowNumber));
            
            // Add listener to update data when text changes
            autoCompleteEditor->onTextChange = [this, autoCompleteEditor, rowNumber]()
            {
                if (rowNumber < probeMetadata.size())
                {
                    probeMetadata.getReference(rowNumber).targetStructure = autoCompleteEditor->getText();
                    //LOGD("Target structure for row ", rowNumber, " changed to: ", autoCompleteEditor->getText());
                }
            };
        }
        else
        {
            autoCompleteEditor->updateColours();
        }
        
        // Set the current text based on the data
        if (rowNumber < probeMetadata.size())
        {
            String currentValue = probeMetadata[rowNumber].targetStructure;
            autoCompleteEditor->setText(currentValue, dontSendNotification);
        }
        
        return autoCompleteEditor;
    }
    
    // Button for the Dye column (column 14)
    if (columnId == 14)
    {
        TextButton* button = dynamic_cast<TextButton*>(existingComponentToUpdate);
        
        if (button == nullptr)
        {
            button = new TextButton();
            button->setName("DyeButton_" + String(rowNumber));
            button->addListener(this);
        }
        
        // Set the button text and color based on the current dye value
        if (rowNumber < probeMetadata.size())
        {
            String currentDye = probeMetadata[rowNumber].dye;
            if (currentDye == "DS-DiD")
            {
                button->setButtonText("DS-DiD");
                button->setColour(TextButton::buttonColourId, Colours::darkblue);
                button->setColour(TextButton::textColourOnId, Colours::white);
                button->setColour(TextButton::textColourOffId, Colours::white);
            }
            else if (currentDye == "None")
            {
                button->setButtonText("None");
                button->setColour(TextButton::buttonColourId, findColour (ThemeColours::widgetBackground));
                button->setColour(TextButton::textColourOnId, findColour (ThemeColours::defaultText));
                button->setColour(TextButton::textColourOffId, findColour (ThemeColours::defaultText));
            }
            else
            {
                // Default to CM-DiI
                button->setButtonText("CM-DiI");
                button->setColour(TextButton::buttonColourId, Colours::darkred);
                button->setColour(TextButton::textColourOnId, Colours::white);
                button->setColour(TextButton::textColourOffId, Colours::white);
                probeMetadata.getReference(rowNumber).dye = "CM-DiI";
            }
        }
        
        return button;
    }
    
    // Button for the Notes column (column 15)
    if (columnId == 15)
    {
        TextButton* button = dynamic_cast<TextButton*>(existingComponentToUpdate);
        
        if (button == nullptr)
        {
            button = new TextButton();
            button->setName("NotesButton_" + String(rowNumber));
            button->addListener(this);
        }
        
        // Set the button text to a truncated version of the notes
        if (rowNumber < probeMetadata.size())
        {
            String fullNotes = probeMetadata[rowNumber].notes;
            String displayText;
            
            if (fullNotes.isEmpty())
            {
                displayText = "Add notes...";
                button->setColour(TextButton::buttonColourId, Colours::white.withAlpha(0.0f));
            }
            else
            {
                // Truncate long text for display
                const int maxDisplayLength = 20;
                if (fullNotes.length() > maxDisplayLength)
                {
                    displayText = fullNotes.substring(0, maxDisplayLength) + "...";
                }
                else
                {
                    displayText = fullNotes;
                }
                button->setColour(TextButton::buttonColourId, Colours::white.withAlpha(0.0f));
            }
            
            button->setButtonText(displayText);
        }
        
        return button;
    }
    
    // Button for the Hemisphere column (column 16)
    if (columnId == 16)
    {
        TextButton* button = dynamic_cast<TextButton*>(existingComponentToUpdate);
        
        if (button == nullptr)
        {
            button = new TextButton();
            button->setName("HemisphereButton_" + String(rowNumber));
            button->addListener(this);
        }
        
        // Set button text and color based on current hemisphere value
        if (rowNumber < probeMetadata.size())
        {
            String currentHemisphere = probeMetadata[rowNumber].hemisphere;
            if (currentHemisphere == "L")
            {
                button->setButtonText("L");
                button->setColour(TextButton::buttonColourId, Colours::lightblue);
                button->setColour(TextButton::textColourOnId, Colours::black);
                button->setColour(TextButton::textColourOffId, Colours::black);
            }
            else if (currentHemisphere == "R")
            {
                button->setButtonText("R");
                button->setColour(TextButton::buttonColourId, Colours::lightcoral);
                button->setColour(TextButton::textColourOnId, Colours::black);
                button->setColour(TextButton::textColourOffId, Colours::black);
            }
            else
            {
                button->setButtonText("None");
                button->setColour(TextButton::buttonColourId, findColour(ThemeColours::widgetBackground));
                button->setColour(TextButton::textColourOnId, findColour(ThemeColours::defaultText));
                button->setColour(TextButton::textColourOffId, findColour(ThemeColours::defaultText));
            }
        }
        
        return button;
    }
    
    // Label for the Implant Hole Number column (column 17)
    if (columnId == 17)
    {
        Label* label = dynamic_cast<Label*>(existingComponentToUpdate);
        
        if (label == nullptr)
        {
            label = new Label();
            label->setEditable(true);
            label->setJustificationType(Justification::centred);
            label->setName("ImplantHole_" + String(rowNumber));

            label->onEditorShow = [this, label]()
            {
                if (label != nullptr)
                {
                    label->getCurrentTextEditor()->setInputRestrictions(3, "0123456789");
                }
            };
            
            label->onTextChange = [this, label, rowNumber]()
            {
                String newText = label->getText();
                
                // Validate numeric input 0-100
                if (!newText.isEmpty())
                {
                    int value = newText.getIntValue();
                    if (value < 0 || value > 100 || !newText.containsOnly("0123456789"))
                    {
                        // Revert to previous value
                        if (rowNumber < probeMetadata.size())
                        {
                            label->setText(probeMetadata[rowNumber].implantHoleNumber, dontSendNotification);
                            return;
                        }
                    }
                }
                
                // Update the data
                if (rowNumber < probeMetadata.size())
                {
                    probeMetadata.getReference(rowNumber).implantHoleNumber = newText;
                    LOGD("Implant hole for row ", rowNumber, " changed to: ", newText);
                }
            };
        }
        
        // Set current text
        if (rowNumber < probeMetadata.size())
        {
            label->setText(probeMetadata[rowNumber].implantHoleNumber, dontSendNotification);
        }
        
        return label;
    }
    
    // Checkbox for Inserted column (column 18)
    if (columnId == 18)
    {
        ToggleButton* checkbox = dynamic_cast<ToggleButton*>(existingComponentToUpdate);
        
        if (checkbox == nullptr)
        {
            checkbox = new ToggleButton();
            checkbox->setName("InsertedCheckbox_" + String(rowNumber));
            checkbox->onClick = [this, checkbox, rowNumber]()
            {
                if (rowNumber < probeMetadata.size())
                {
                    probeMetadata.getReference(rowNumber).inserted = checkbox->getToggleState();
                    LOGD("Inserted for row ", rowNumber, " changed to: ", checkbox->getToggleState());
                }
            };
        }
        
        // Set current state
        if (rowNumber < probeMetadata.size())
        {
            checkbox->setToggleState(probeMetadata[rowNumber].inserted, dontSendNotification);
        }
        
        return checkbox;
    }
    
    // Checkbox for On Target column (column 19)
    if (columnId == 19)
    {
        ToggleButton* checkbox = dynamic_cast<ToggleButton*>(existingComponentToUpdate);
        
        if (checkbox == nullptr)
        {
            checkbox = new ToggleButton();
            checkbox->setName("OnTargetCheckbox_" + String(rowNumber));
            checkbox->onClick = [this, checkbox, rowNumber]()
            {
                if (rowNumber < probeMetadata.size())
                {
                    probeMetadata.getReference(rowNumber).onTarget = checkbox->getToggleState();
                    LOGD("On Target for row ", rowNumber, " changed to: ", checkbox->getToggleState());
                }
            };
        }
        
        // Set current state
        if (rowNumber < probeMetadata.size())
        {
            checkbox->setToggleState(probeMetadata[rowNumber].onTarget, dontSendNotification);
        }
        
        return checkbox;
    }
    
    // Checkbox for Recorded column (column 20)
    if (columnId == 20)
    {
        ToggleButton* checkbox = dynamic_cast<ToggleButton*>(existingComponentToUpdate);
        
        if (checkbox == nullptr)
        {
            checkbox = new ToggleButton();
            checkbox->setName("RecordedCheckbox_" + String(rowNumber));
            checkbox->onClick = [this, checkbox, rowNumber]()
            {
                if (rowNumber < probeMetadata.size())
                {
                    probeMetadata.getReference(rowNumber).recorded = checkbox->getToggleState();
                    LOGD("Recorded for row ", rowNumber, " changed to: ", checkbox->getToggleState());
                }
            };
        }
        
        // Set current state
        if (rowNumber < probeMetadata.size())
        {
            checkbox->setToggleState(probeMetadata[rowNumber].recorded, dontSendNotification);
        }
        
        return checkbox;
    }
    
    // Button for the Delete column (column 21)
    if (columnId == 21)
    {
        TextButton* button = dynamic_cast<TextButton*>(existingComponentToUpdate);
        
        if (button == nullptr)
        {
            button = new TextButton();
            button->setName("DeleteButton_" + String(rowNumber));
            button->addListener(this);
        }
        
        // Set the button appearance for delete action - only enable for manually-created probes
        if (rowNumber < probeMetadata.size() && probeMetadata[rowNumber].isManuallyCreated)
        {
            button->setButtonText("X");
            button->setColour(TextButton::textColourOnId, Colours::red.withAlpha(0.8f));
            button->setColour(TextButton::textColourOffId, Colours::red.withAlpha(0.8f));
            button->setEnabled(true);
        }
        else
        {
            // Disable delete button for auto-detected probes
            button->setButtonText("X");
            button->setEnabled(false);
            button->setColour(TextButton::textColourOnId, Colours::red.withAlpha(0.8f));
            button->setColour(TextButton::textColourOffId, Colours::red.withAlpha(0.8f));
        }
        
        button->setSize(40, 20);
        
        return button;
    }
    
    return nullptr;
}

int EphysMetadataGeneratorCanvas::getColumnAutoSizeWidth(int columnId)
{
    // Auto-size only the Probe column (column 1)
    if (columnId == 1)
    {
        return 100; // Minimum width
    }
    
    return -1; // Default behavior for other columns
}

bool EphysMetadataGeneratorCanvas::isValidAngleInput(const String& text)
{
    // Attempt to convert the string to a float
    auto result = text.getFloatValue();

    // Check if the input actually represents a valid float
    // and is within the range -180 to +180
    // We also check for edge cases where the input is not a valid float string
    if (!text.containsOnly("0123456789.-+eE")) return false;

    // Additional strict check: make sure the parsed float produces the same string
    // (optional, but avoids inputs like "12abc" which get parsed as 12.0)
    juce::String trimmed = text.trim();
    double parsed;
    if (!trimmed.retainCharacters("0123456789.-+eE").equalsIgnoreCase(trimmed))
        return false;

    // Range check
    return result >= -180.0f && result <= 180.0f;
    
}


bool EphysMetadataGeneratorCanvas::isValidCoordinateInput(const String& text)
{
    // Attempt to convert the string to a float
    auto result = text.getFloatValue();

    // Check if the input actually represents a valid float
    if (!text.containsOnly("0123456789.-+eE")) return false;

    // Additional strict check: make sure the parsed float produces the same string
    juce::String trimmed = text.trim();
    double parsed;
    if (!trimmed.retainCharacters("0123456789.-+eE").equalsIgnoreCase(trimmed))
        return false;

    return true;
}


void EphysMetadataGeneratorCanvas::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
    String comboBoxName = comboBoxThatHasChanged->getName();
    LOGD("ComboBox: ", comboBoxName, " changed to ", comboBoxThatHasChanged->getText());
    
    // Handle manipulator ComboBox changes
    if (comboBoxName.startsWith("Manipulator_"))
    {
        // Extract the row number from the ComboBox name
        String rowString = comboBoxName.substring(12); // Remove "Manipulator_" prefix
        int rowNumber = rowString.getIntValue();
        
        if (rowNumber < probeMetadata.size())
        {
            String selectedText = comboBoxThatHasChanged->getText();
            
            // Update the probe data with the selected manipulator
            if (selectedText == "<Select Manipulator>" || selectedText.isEmpty())
            {
                // User selected the null option
                probeMetadata.getReference(rowNumber).manipulator = "";
            }
            else
            {
                // User selected a valid serial number
                probeMetadata.getReference(rowNumber).manipulator = selectedText;
            }
            
            LOGD("Manipulator for row ", rowNumber, " changed to: ", probeMetadata[rowNumber].manipulator);
        }
    }
}


void EphysMetadataGeneratorCanvas::labelTextChanged(Label* labelThatHasChanged)
{
    LOGD("Label: ", labelThatHasChanged->getName(), " changed to ", labelThatHasChanged->getText());
}

void EphysMetadataGeneratorCanvas::buttonClicked(Button* buttonThatWasClicked)
{
    String buttonName = buttonThatWasClicked->getName();
    
    // Check if it's the Connect button
    if (buttonName == "ConnectButton")
    {
        updateStageListenerEndpoint();
        return; // Early return to avoid checking other button types
    }
    
    // Check if it's the Add Row button
    if (buttonName == "AddRowButton")
    {
        // Create a component with a text editor for the user to enter the device name
        auto inputComponent = std::make_unique<CustomDeviceInputComponent>();
        inputComponent->setSize(360, 80);
        
        auto label = std::make_unique<Label>("DeviceNameLabel", "Enter device name:");
        label->setBounds(10, 10, 130, 25);
        label->setFont (FontOptions ("Inter", "Medium", 16.0f));
        label->setJustificationType(Justification::centredLeft);
        inputComponent->addAndMakeVisible(*label);
        label.release();  // Transfer ownership to parent
        
        auto textEditor = std::make_unique<TextEditor>();
        textEditor->setName("DeviceNameEditor");
        textEditor->setBounds(140, 10, 210, 25);
        textEditor->setReturnKeyStartsNewLine(false);
        textEditor->setMultiLine(false);
        inputComponent->addAndMakeVisible(*textEditor);
        auto rawTextEditor = textEditor.get();
        textEditor.release();  // Transfer ownership to parent
        
        auto okButton = std::make_unique<TextButton>("OK");
        okButton->setBounds(115, 45, 60, 25);
        inputComponent->addAndMakeVisible(*okButton);
        auto rawOkButton = okButton.get();
        okButton.release();  // Transfer ownership to parent
        
        auto cancelButton = std::make_unique<TextButton>("Cancel");
        cancelButton->setBounds(185, 45, 60, 25);
        inputComponent->addAndMakeVisible(*cancelButton);
        auto rawCancelButton = cancelButton.get();
        cancelButton.release();  // Transfer ownership to parent
        
        // Launch the CallOutBox asynchronously (takes ownership of inputComponent)
        auto& calloutBox = CallOutBox::launchAsynchronously(
            std::move(inputComponent),
            buttonThatWasClicked->getScreenBounds(),
            nullptr
        );
        calloutBox.setDismissalMouseClicksAreAlwaysConsumed(true);


        // Capture 'this' and raw pointers in the lambdas
        rawOkButton->onClick = [this, rawTextEditor, &calloutBox]()
        {
            String deviceName = rawTextEditor->getText().trim();
            
            // Validate the device name
            if (deviceName.isEmpty())
            {
                AlertWindow::showMessageBoxAsync(
                    AlertWindow::WarningIcon,
                    "Invalid Input",
                    "Please enter a device name.",
                    "OK"
                );
                return;
            }
            
            // Check for duplicate names
            for (const auto& existingProbe : probeMetadata)
            {
                if (existingProbe.probe == deviceName)
                {
                    AlertWindow::showMessageBoxAsync(
                        AlertWindow::WarningIcon,
                        "Duplicate Name",
                        "A device with the name \"" + deviceName + "\" already exists.",
                        "OK"
                    );
                    return;
                }
            }
            
            // Create a new probe metadata entry with default values
            ProbeMetadata newProbe;
            newProbe.probe = deviceName;
            newProbe.serialNumber = deviceName;  // Use device name as serial number for manual devices
            newProbe.manipulator = "";
            newProbe.arcAngle = "0";
            newProbe.moduleAngle = "0";
            newProbe.rotation = "0";
            newProbe.x = "0.0";
            newProbe.y = "0.0";
            newProbe.z = "0.0";
            newProbe.surface = "0.0";
            newProbe.bregmaX = "0.0";
            newProbe.bregmaY = "0.0";
            newProbe.bregmaZ = "0.0";
            newProbe.targetStructure = "";
            newProbe.dye = "CM-DiI";
            newProbe.notes = "";
            newProbe.hemisphere = "";
            newProbe.implantHoleNumber = "";
            newProbe.inserted = true;
            newProbe.onTarget = true;
            newProbe.recorded = true;
            newProbe.isManuallyCreated = true;  // Mark as manually created
            
            // Add the new probe to the data array
            probeMetadata.add(newProbe);
            
            // Update the table to show the new row
            metadataTable->updateContent();
            metadataTable->repaint();
            
            LOGD("Added new device row: ", newProbe.probe, " (total rows: ", probeMetadata.size(), ")");
            
            // Close the callout box
            calloutBox.dismiss();
        };
        
        rawCancelButton->onClick = [&calloutBox]()
        {
            calloutBox.dismiss();
        };
        
        // Handle Enter key to confirm
        rawTextEditor->onReturnKey = [rawOkButton]()
        {
            rawOkButton->triggerClick();
        };
        
        return; // Early return to avoid checking other button types
    }
    
    // (Add Row button removed)
    
    // Check if it's a surface button
    if (buttonName.startsWith("SurfaceButton_"))
    {
        // Extract the row number from the button name
        String rowString = buttonName.substring(14); // Remove "SurfaceButton_" prefix
        int rowNumber = rowString.getIntValue();
        
        if (rowNumber < probeMetadata.size())
        {
            TextButton* surfaceButton = static_cast<TextButton*>(buttonThatWasClicked);
            String currentSurface = probeMetadata[rowNumber].surface;
            String currentZ = probeMetadata[rowNumber].z;
            
            // Check if surface is currently set from Z (button is active/green)
            bool isSurfaceSetFromZ = (currentSurface == currentZ && currentSurface != "0.0" && !currentSurface.isEmpty());
            
            if (isSurfaceSetFromZ)
            {
                // Button is currently active - revert to inactive state
                probeMetadata.getReference(rowNumber).surface = "0.0";
                
                // Update button appearance to inactive state
                surfaceButton->setButtonText("Set Surface");
                surfaceButton->setColour(TextButton::buttonColourId, findColour (ThemeColours::widgetBackground));
                surfaceButton->setColour(TextButton::textColourOnId, findColour (ThemeColours::defaultText));
                
                LOGD("Surface for row ", rowNumber, " cleared (reverted to inactive)");
            }
            else
            {
                // Button is currently inactive - set surface from Z value
                probeMetadata.getReference(rowNumber).surface = currentZ;
                
                // Update button appearance to active state
                surfaceButton->setButtonText(currentZ);
                surfaceButton->setColour(TextButton::buttonColourId, Colours::green);
                surfaceButton->setColour(TextButton::textColourOnId, Colours::white);
                
                LOGD("Surface for row ", rowNumber, " set to Z value: ", currentZ);
            }
            
            // Force a repaint of the button to show the color change
            surfaceButton->repaint();
        }
    }
    // Check if it's a hemisphere button
    else if (buttonName.startsWith("HemisphereButton_"))
    {
        // Extract the row number from the button name
        String rowString = buttonName.substring(17); // Remove "HemisphereButton_" prefix
        int rowNumber = rowString.getIntValue();
        
        if (rowNumber < probeMetadata.size())
        {
            TextButton* hemisphereButton = static_cast<TextButton*>(buttonThatWasClicked);
            String currentHemisphere = probeMetadata[rowNumber].hemisphere;
            
            // Cycle through None -> L -> R -> None
            if (currentHemisphere.isEmpty() || currentHemisphere == "")
            {
                // Switch to L
                probeMetadata.getReference(rowNumber).hemisphere = "L";
                hemisphereButton->setButtonText("L");
                hemisphereButton->setColour(TextButton::buttonColourId, Colours::lightblue);
                hemisphereButton->setColour(TextButton::textColourOnId, Colours::black);
                LOGD("Hemisphere for row ", rowNumber, " changed to L");
            }
            else if (currentHemisphere == "L")
            {
                // Switch to R
                probeMetadata.getReference(rowNumber).hemisphere = "R";
                hemisphereButton->setButtonText("R");
                hemisphereButton->setColour(TextButton::buttonColourId, Colours::lightcoral);
                hemisphereButton->setColour(TextButton::textColourOnId, Colours::black);
                LOGD("Hemisphere for row ", rowNumber, " changed to R");
            }
            else
            {
                // Switch back to None
                probeMetadata.getReference(rowNumber).hemisphere = "";
                hemisphereButton->setButtonText("None");
                hemisphereButton->setColour(TextButton::buttonColourId, findColour(ThemeColours::widgetBackground));
                hemisphereButton->setColour(TextButton::textColourOnId, findColour(ThemeColours::defaultText));
                LOGD("Hemisphere for row ", rowNumber, " changed to None");
            }
            
            hemisphereButton->repaint();
        }
    }
    // Check if it's a dye button
    else if (buttonName.startsWith("DyeButton_"))
    {
        // Extract the row number from the button name
        String rowString = buttonName.substring(10); // Remove "DyeButton_" prefix
        int rowNumber = rowString.getIntValue();
        
        if (rowNumber < probeMetadata.size())
        {
            TextButton* dyeButton = static_cast<TextButton*>(buttonThatWasClicked);
            String currentDye = probeMetadata[rowNumber].dye;
            
            // Toggle between the three dye options
            if (currentDye == "CM-DiI")
            {
                // Switch to DS-DiD
                probeMetadata.getReference(rowNumber).dye = "DS-DiD";
                dyeButton->setButtonText("DS-DiD");
                dyeButton->setColour(TextButton::buttonColourId, Colours::darkblue);
                dyeButton->setColour(TextButton::textColourOnId, Colours::white);
                dyeButton->setColour(TextButton::textColourOffId, Colours::white);
                LOGD("Dye for row ", rowNumber, " changed to DS-DiD");
            }
            else if (currentDye == "DS-DiD")
            {
                // Switch to None
                probeMetadata.getReference(rowNumber).dye = "None";
                dyeButton->setButtonText("None");
                dyeButton->setColour(TextButton::buttonColourId, findColour(ThemeColours::widgetBackground));
                dyeButton->setColour(TextButton::textColourOnId, findColour(ThemeColours::defaultText));
                dyeButton->setColour(TextButton::textColourOffId, findColour(ThemeColours::defaultText));
                LOGD("Dye for row ", rowNumber, " changed to None");
            }
            else {
                // Switch back to CM-DiI
                probeMetadata.getReference(rowNumber).dye = "CM-DiI";
                dyeButton->setButtonText("CM-DiI");
                dyeButton->setColour(TextButton::buttonColourId, Colours::darkred);
                dyeButton->setColour(TextButton::textColourOnId, Colours::white);
                dyeButton->setColour(TextButton::textColourOffId, Colours::white);
                LOGD("Dye for row ", rowNumber, " changed to CM-DiI");
            }
            
            // Force a repaint of the button to show the color change
            dyeButton->repaint();
        }
    }
    // Check if it's a delete button
    else if (buttonName.startsWith("DeleteButton_"))
    {
        // Extract the row number from the button name
        String rowString = buttonName.substring(13); // Remove "DeleteButton_" prefix
        int rowNumber = rowString.getIntValue();
        
        if (rowNumber < probeMetadata.size() && probeMetadata[rowNumber].isManuallyCreated)
        {
            // Show confirmation dialog before deleting
            String probeIdentifier = probeMetadata[rowNumber].probe;
            
            // Create a confirmation dialog
            bool shouldDelete = AlertWindow::showOkCancelBox(
                AlertWindow::QuestionIcon,
                "Delete Probe Row",
                "Are you sure you want to delete \"" + probeIdentifier + "\"?\n\nThis action cannot be undone.",
                "Delete",
                "Cancel"
            );
            
            if (shouldDelete)
            {
                // Remove the probe from the data array
                probeMetadata.remove(rowNumber);
                
                // Update the table to reflect the deletion
                metadataTable->updateContent();
                metadataTable->repaint();
                
                LOGD("Deleted probe row: ", probeIdentifier, " (remaining rows: ", probeMetadata.size(), ")");
            }
            else
            {
                LOGD("Delete cancelled for probe: ", probeIdentifier);
            }
        }
    }
    // Check if it's a notes button
    else if (buttonName.startsWith("NotesButton_"))
    {
        // Extract the row number from the button name
        String rowString = buttonName.substring(12); // Remove "NotesButton_" prefix
        int rowNumber = rowString.getIntValue();
        
        if (rowNumber < probeMetadata.size())
        {
            // Create and show the popup editor
            String currentNotes = probeMetadata[rowNumber].notes;
            
            auto popup = new NotesEditorPopup(currentNotes, [this, rowNumber](const String& newText)
            {
                // Update the notes data
                probeMetadata.getReference(rowNumber).notes = newText;
                LOGD("Notes for row ", rowNumber, " changed to: ", newText);
                
                // Update the table to reflect the change
                metadataTable->updateContent();
            });
            
            // Position the popup near the button
            Rectangle<int> buttonBounds = buttonThatWasClicked->getBounds();
            Point<int> popupPosition = getLocalPoint(buttonThatWasClicked->getParentComponent(), buttonBounds.getBottomLeft());
            popup->setTopLeftPosition(popupPosition.x, popupPosition.y + 5);
            
            // Add the popup to the canvas and give it focus
            addAndMakeVisible(popup);
            popup->grabKeyboardFocus();
        }
    }
}

// StagePositionListener interface implementation
void EphysMetadataGeneratorCanvas::stagePositionsUpdated(const Array<StagePosition>& positions)
{
    LOGD("Received ", positions.size(), " stage positions from HTTP endpoint");

    if (connectButton != nullptr && connectButton->getButtonText() == "Connecting...")
    {
        // Indicate successful connection on the Connect button
        connectButton->setButtonText("Connected");
        connectButton->setColour(TextButton::buttonColourId, Colours::green);
        connectButton->setColour(TextButton::textColourOnId, Colours::black);
        connectButton->setColour(TextButton::textColourOffId, Colours::black);
    }

    // Store the available serial numbers in the global array (merge with existing)
    for (const auto& stagePos : positions)
    {
        if (!availableSerialNumbers.contains(stagePos.serialNumber))
        {
            availableSerialNumbers.add(stagePos.serialNumber);
        }
    }

    // Only validate and clear manipulators if we have valid serial numbers available
    // AND this is not the first time we've received data (to preserve saved assignments)
    bool needsManipulatorUpdate = false;
    if (!availableSerialNumbers.isEmpty())
    {
        for (int probeIndex = 0; probeIndex < probeMetadata.size(); ++probeIndex)
        {
            ProbeMetadata& probe = probeMetadata.getReference(probeIndex);
            
            // Only clear manipulators if they don't match available serial numbers
            // AND the manipulator is not empty (to preserve user selections)
            if (!probe.manipulator.isEmpty() && !availableSerialNumbers.contains(probe.manipulator))
            {
                LOGD("Probe ", probeIndex, " manipulator '", probe.manipulator, "' does not match any available serial numbers - clearing");
                
                // Set manipulator to null (empty string)
                probe.manipulator = "";
                needsManipulatorUpdate = true;
            }
        }
    }
    
    // If we need to update manipulators, refresh the table to update ComboBoxes
    if (needsManipulatorUpdate)
    {
        LOGD("Updating table to reflect new serial numbers in ComboBoxes");
        metadataTable->updateContent();
    }
    
    int updatedCount = 0;
    
    // Update the X, Y, Z values only for probes whose manipulator matches the stage serial number
    for (const auto& stagePos : positions)
    {
        // Find matching probe metadata by comparing manipulator string with stage serial number
        for (int probeIndex = 0; probeIndex < probeMetadata.size(); ++probeIndex)
        {
            ProbeMetadata& probe = probeMetadata.getReference(probeIndex);
            
            // Check if the probe's manipulator string matches the stage's serial number
            if (probe.manipulator == stagePos.serialNumber)
            {
                // Update the probe position data
                probe.x = String (stagePos.x.getFloatValue(), 2);
                probe.y = String (stagePos.y.getFloatValue(), 2);
                probe.z = String (stagePos.z.getFloatValue(), 2);
                
                // LOGD("Updated probe row ", probeIndex, " (manipulator: ", probe.manipulator, 
                //      ") with stage position from serial ", stagePos.serialNumber, 
                //      ": X=", stagePos.x, " Y=", stagePos.y, " Z=", stagePos.z);
                
                updatedCount++;
            }
        }
    }
    
    // Log unmatched stages for debugging
    if (updatedCount < positions.size())
    {
        for (const auto& stagePos : positions)
        {
            bool found = false;
            for (int probeIndex = 0; probeIndex < probeMetadata.size(); ++probeIndex)
            {
                if (probeMetadata[probeIndex].manipulator == stagePos.serialNumber)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                LOGD("No matching probe found for stage serial number: ", stagePos.serialNumber);
            }
        }
    }
    
    LOGD("Updated ", updatedCount, " out of ", positions.size(), " stage positions");
    
    // Refresh the table to show updated values
    metadataTable->repaint();
}

void EphysMetadataGeneratorCanvas::stagePositionError(const String& errorMessage)
{
    LOGE("Stage position fetch error: ", errorMessage);
    
    if (connectButton != nullptr && connectButton->getButtonText() != "Connect")
    {
        // Indicate error state on the Connect button
        connectButton->setButtonText("Error");
        connectButton->setColour(TextButton::buttonColourId, Colours::darkred);
        connectButton->setColour(TextButton::textColourOnId, Colours::white);
        connectButton->setColour(TextButton::textColourOffId, Colours::white);
    }
}

void EphysMetadataGeneratorCanvas::mouseDown(const MouseEvent& event)
{   
    if (metadataTable->getHeader().getScreenBounds().contains (event.getScreenPosition())
        && event.mods.isPopupMenu())
    {
        bool currentlyVisible = metadataTable->getHeader().isColumnVisible(10);
        PopupMenu headerMenu;
        headerMenu.addItem("Show Bregma Coordinates", true, currentlyVisible, [this, currentlyVisible]()
        {
            metadataTable->getHeader().setColumnVisible(10, !currentlyVisible);
            metadataTable->getHeader().setColumnVisible(11, !currentlyVisible);
            metadataTable->getHeader().setColumnVisible(12, !currentlyVisible);
        });
        headerMenu.showMenuAsync (PopupMenu::Options().withPreferredPopupDirection(PopupMenu::Options::PopupDirection::downwards));
    }
}

// XML serialization methods
void EphysMetadataGeneratorCanvas::saveCustomParametersToXml(XmlElement* xml)
{
    XmlElement* probeMetadataElement = xml->createNewChildElement("PROBE_METADATA");
    
    for (int i = 0; i < probeMetadata.size(); ++i)
    {
        const ProbeMetadata& probe = probeMetadata[i];
        
        XmlElement* probeElement = probeMetadataElement->createNewChildElement("PROBE");
        probeElement->setAttribute("index", i);
        
        // Save all probe metadata fields
        probeElement->setAttribute("probe", probe.probe);
        probeElement->setAttribute("serialNumber", probe.serialNumber);
        probeElement->setAttribute("manipulator", probe.manipulator);
        probeElement->setAttribute("arcAngle", probe.arcAngle);
        probeElement->setAttribute("moduleAngle", probe.moduleAngle);
        probeElement->setAttribute("rotation", probe.rotation);
        probeElement->setAttribute("x", probe.x);
        probeElement->setAttribute("y", probe.y);
        probeElement->setAttribute("z", probe.z);
        probeElement->setAttribute("surface", probe.surface);
        probeElement->setAttribute("bregmaX", probe.bregmaX);
        probeElement->setAttribute("bregmaY", probe.bregmaY);
        probeElement->setAttribute("bregmaZ", probe.bregmaZ);
        probeElement->setAttribute("targetStructure", probe.targetStructure);
        probeElement->setAttribute("dye", probe.dye);
        probeElement->setAttribute("notes", probe.notes);
        probeElement->setAttribute("hemisphere", probe.hemisphere);
        probeElement->setAttribute("implantHoleNumber", probe.implantHoleNumber);
        probeElement->setAttribute("inserted", probe.inserted);
        probeElement->setAttribute("onTarget", probe.onTarget);
        probeElement->setAttribute("recorded", probe.recorded);
        probeElement->setAttribute("isManuallyCreated", probe.isManuallyCreated);
    }
    
    // Save server address setting
    if (serverAddressEditor != nullptr)
    {
        xml->setAttribute("serverAddress", serverAddressEditor->getText());
    }
    
    // Save available serial numbers
    if (!availableSerialNumbers.isEmpty())
    {
        XmlElement* serialNumbersElement = xml->createNewChildElement("AVAILABLE_SERIAL_NUMBERS");
        for (int i = 0; i < availableSerialNumbers.size(); ++i)
        {
            XmlElement* serialElement = serialNumbersElement->createNewChildElement("SERIAL");
            serialElement->setAttribute("number", availableSerialNumbers[i]);
        }
        LOGD("Saved ", availableSerialNumbers.size(), " available serial numbers to XML");
    }
    
    LOGD("Saved ", probeMetadata.size(), " probe metadata entries to XML");
}

void EphysMetadataGeneratorCanvas::loadCustomParametersFromXml(XmlElement* xml)
{
    // Clear existing probe data
    probeMetadata.clear();
    
    XmlElement* probeMetadataElement = xml->getChildByName("PROBE_METADATA");
    
    if (probeMetadataElement != nullptr)
    {
        // Load each probe entry
        forEachXmlChildElement(*probeMetadataElement, probeElement)
        {
            if (probeElement->hasTagName("PROBE"))
            {
                ProbeMetadata probe;
                
                // Load all probe metadata fields
                probe.probe = probeElement->getStringAttribute("probe", "");
                probe.serialNumber = probeElement->getStringAttribute("serialNumber", "");
                probe.manipulator = probeElement->getStringAttribute("manipulator", "62219");
                probe.arcAngle = probeElement->getStringAttribute("arcAngle", "0");
                probe.moduleAngle = probeElement->getStringAttribute("moduleAngle", "0");
                probe.rotation = probeElement->getStringAttribute("rotation", "0");
                probe.x = probeElement->getStringAttribute("x", "0.0");
                probe.y = probeElement->getStringAttribute("y", "0.0");
                probe.z = probeElement->getStringAttribute("z", "0.0");
                probe.surface = probeElement->getStringAttribute("surface", "0.0");
                probe.bregmaX = probeElement->getStringAttribute("bregmaX", "0.0");
                probe.bregmaY = probeElement->getStringAttribute("bregmaY", "0.0");
                probe.bregmaZ = probeElement->getStringAttribute("bregmaZ", "0.0");
                probe.targetStructure = probeElement->getStringAttribute("targetStructure", "");
                probe.dye = probeElement->getStringAttribute("dye", "CM-DiI");
                probe.notes = probeElement->getStringAttribute("notes", "");
                probe.hemisphere = probeElement->getStringAttribute("hemisphere", "");
                probe.implantHoleNumber = probeElement->getStringAttribute("implantHoleNumber", "");
                probe.inserted = probeElement->getBoolAttribute("inserted", true);
                probe.onTarget = probeElement->getBoolAttribute("onTarget", true);
                probe.recorded = probeElement->getBoolAttribute("recorded", true);
                probe.isManuallyCreated = probeElement->getBoolAttribute("isManuallyCreated", false);
                
                probeMetadata.add(probe);
            }
        }
        
        LOGD("Loaded ", probeMetadata.size(), " probe metadata entries from XML");
    }
    else
    {
        // If no saved data found, initialize with default sample data
        LOGD("No probe metadata found in XML, using default sample data");
        initializeTable(); // This will create the default sample data
        return; // Exit early since initializeTable() already updates the table
    }
    
    // Load available serial numbers
    availableSerialNumbers.clear();
    XmlElement* serialNumbersElement = xml->getChildByName("AVAILABLE_SERIAL_NUMBERS");
    if (serialNumbersElement != nullptr)
    {
        forEachXmlChildElement(*serialNumbersElement, serialElement)
        {
            if (serialElement->hasTagName("SERIAL"))
            {
                String serialNumber = serialElement->getStringAttribute("number", "");
                if (serialNumber.isNotEmpty())
                {
                    availableSerialNumbers.add(serialNumber);
                }
            }
        }
    }
    else
    {
        LOGD("No available serial numbers found in XML");
    }
    
    // Load server address setting
    if (serverAddressEditor != nullptr)
    {
        String savedServerAddress = xml->getStringAttribute("serverAddress", "10.128.49.150:8080");
        serverAddressEditor->setText(savedServerAddress, false);
        LOGD("Loaded server address: ", savedServerAddress);
        updateStageListenerEndpoint();
    }
    
    // Update the table to reflect the loaded data
    if (metadataTable != nullptr)
    {
        metadataTable->updateContent();
        metadataTable->repaint();
    }
}

void EphysMetadataGeneratorCanvas::updateStageListenerEndpoint()
{
    if (serverAddressEditor == nullptr || stageListener == nullptr)
        return;
    
    String serverAddress = serverAddressEditor->getText().trim();
    
    // Basic validation of the server address
    if (serverAddress.isEmpty())
    {
        LOGE("Server address cannot be empty");
        return;
    }
    
    // Parse the server address to extract IP/hostname and port
    String host = "10.128.49.150";
    int port = 8080;
    
    if (serverAddress.contains(":"))
    {
        StringArray parts = StringArray::fromTokens(serverAddress, ":", "");
        if (parts.size() == 2)
        {
            host = parts[0].trim();
            port = parts[1].trim().getIntValue();
            
            // Basic validation
            if (host.isEmpty() || port <= 0 || port > 65535)
            {
                LOGE("Invalid server address format. Expected format: hostname:port (e.g., localhost:8080)");
                return;
            }
        }
        else
        {
            LOGE("Invalid server address format. Expected format: hostname:port (e.g., localhost:8080)");
            return;
        }
    }
    else
    {
        // If no port specified, assume it's just a hostname/IP, keep default port
        host = serverAddress;
    }
    
    // Construct the full endpoint URL
    String newEndpoint = "http://" + host + ":" + String(port) + "/stage/positions";
    
    // Stop the current listener if it's running
    bool wasListening = stageListener->isListening();
    if (wasListening)
    {
        stageListener->stopListening();
        LOGD("Stopped stage listener for endpoint update");
    }
    
    // Update the endpoint
    stageListener->setEndpoint(newEndpoint);
    LOGD("Updated stage listener endpoint to: ", newEndpoint);
    
    // Restart the listener if it was previously running
    if (wasListening)
    {
        stageListener->startListening();
        LOGD("Restarted stage listener with new endpoint");
    }
    else
    {
        // Start listening with the new endpoint
        stageListener->startListening();
        LOGD("Started stage listener with new endpoint: ", newEndpoint);
    }

    // Update the connect button to show it's connecting
    if (connectButton != nullptr)
    {
        connectButton->setButtonText("Connecting...");
        connectButton->setColour(TextButton::buttonColourId, Colours::lightgreen);
        connectButton->setColour(TextButton::textColourOnId, Colours::black);
        connectButton->setColour(TextButton::textColourOffId, Colours::black);
    }
}

Metadata::DataStream EphysMetadataGeneratorCanvas::getLatestMetadata()
{
    Time start_time = Time::getCurrentTime();

    Metadata::DataStream stream;
    stream.stream_start_time = start_time;
    stream.modalities.add (Metadata::Modality ({ "Extracellular electrophysiology", "ecephys" }));

    for (int i = 0; i < probeMetadata.size(); ++i)
    {

        Metadata::ProbeConfig probeConfig;
        Metadata::EphysAssemblyConfig ephysAssemblyConfig;
        Metadata::MISModuleConfig misModuleConfig;
        Metadata::ManipulatorConfig manipulatorConfig;

        String device_name = "Ephys_assembly" + probeMetadata[i].probe;

        stream.active_devices.add (device_name);
        ephysAssemblyConfig.device_name = device_name;

        manipulatorConfig.device_name = probeMetadata[i].manipulator;
        manipulatorConfig.coordinate_system.name = "MPM_MANIP_RFB";
        manipulatorConfig.coordinate_system.origin = "Tip";
        manipulatorConfig.coordinate_system.axes.add (Metadata::Axis ({ "Axis", "X", "Left_to_right" }));
        manipulatorConfig.coordinate_system.axes.add (Metadata::Axis ({ "Axis", "Y", "Back_to_front" }));
        manipulatorConfig.coordinate_system.axes.add (Metadata::Axis ({ "Axis", "Z", "Up_to_down" }));
        
        manipulatorConfig.local_axis_positions.translation = { probeMetadata[i].x.getFloatValue(),
                                                               probeMetadata[i].y.getFloatValue(),
                                                               probeMetadata[i].z.getFloatValue() };


        misModuleConfig.arc_angle = probeMetadata[i].arcAngle.getFloatValue();
        misModuleConfig.module_angle = probeMetadata[i].moduleAngle.getFloatValue();
        misModuleConfig.rotation_angle = probeMetadata[i].rotation.getFloatValue();

        probeConfig.device_name = probeMetadata[i].probe;
        
        // Convert target structure string to Region struct using CCFRegionList
        CCFRegionList regionList;
        probeConfig.primary_targeted_structure = regionList.findRegionByAcronym(probeMetadata[i].targetStructure);
        
        probeConfig.coordinate_system.name = "MPM_MANIP_RFB";
        probeConfig.coordinate_system.origin = "Tip";
        probeConfig.coordinate_system.axes.add (Metadata::Axis ({ "Axis", "X", "Left_to_right" }));
        probeConfig.coordinate_system.axes.add (Metadata::Axis ({ "Axis", "Y", "Back_to_front" }));
        probeConfig.coordinate_system.axes.add (Metadata::Axis ({ "Axis", "Z", "Up_to_down" }));
        
        probeConfig.transform_translation.translation = { probeMetadata[i].bregmaX.getFloatValue(),
                                                         probeMetadata[i].bregmaY.getFloatValue(),
                                                         probeMetadata[i].bregmaZ.getFloatValue(),
                                                         probeMetadata[i].surface.getFloatValue() - probeMetadata[i].z.getFloatValue() };

        probeConfig.dye = probeMetadata[i].dye;
        probeConfig.notes = probeMetadata[i].notes;
        probeConfig.hemisphere = probeMetadata[i].hemisphere;
        probeConfig.implant_hole_number = probeMetadata[i].implantHoleNumber;
        probeConfig.inserted = probeMetadata[i].inserted;
        probeConfig.on_target = probeMetadata[i].onTarget;
        probeConfig.recorded = probeMetadata[i].recorded;
        
        ephysAssemblyConfig.modules.add (misModuleConfig);
        ephysAssemblyConfig.manipulator = manipulatorConfig;
        ephysAssemblyConfig.probes.add (probeConfig);

        stream.configurations.add (ephysAssemblyConfig);
    }

    return stream;
}