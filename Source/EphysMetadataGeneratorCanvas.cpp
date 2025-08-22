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
    addAndMakeVisible(metadataTable.get());
    
    // Create and configure the Add Row button
    addRowButton = std::make_unique<TextButton>("Add Row");
    addRowButton->setName("AddRowButton");
    addRowButton->addListener(this);
    addRowButton->setColour(TextButton::buttonColourId, Colours::lightgreen);
    addRowButton->setColour(TextButton::textColourOnId, Colours::black);
    addAndMakeVisible(addRowButton.get());
    
    // Create and configure the server address components
    serverAddressLabel = std::make_unique<Label>("Server Address Label", "HTTP Server:");
    serverAddressLabel->setJustificationType(Justification::centredLeft);
    serverAddressLabel->setColour(Label::textColourId, findColour(ThemeColours::defaultText));
    addAndMakeVisible(serverAddressLabel.get());
    
    serverAddressEditor = std::make_unique<TextEditor>("Server Address");
    serverAddressEditor->setText("10.128.49.150:8080", false);
    serverAddressEditor->setJustification(Justification::centred);
    serverAddressEditor->setColour(TextEditor::backgroundColourId, Colours::white);
    serverAddressEditor->setColour(TextEditor::textColourId, Colours::black);
    serverAddressEditor->setColour(TextEditor::outlineColourId, Colours::grey);
    addAndMakeVisible(serverAddressEditor.get());
    
    connectButton = std::make_unique<TextButton>("Connect");
    connectButton->setName("ConnectButton");
    connectButton->addListener(this);
    connectButton->setColour(TextButton::buttonColourId, Colours::lightblue);
    connectButton->setColour(TextButton::textColourOnId, Colours::black);
    addAndMakeVisible(connectButton.get());
    
    // Add columns to the table
    metadataTable->getHeader().addColumn("Probe", 1, 80);
    metadataTable->getHeader().addColumn("Manipulator", 2, 100);
    metadataTable->getHeader().addColumn("Arc Angle", 3, 80);
    metadataTable->getHeader().addColumn("Module Angle", 4, 100);
    metadataTable->getHeader().addColumn("Rotation", 5, 80);
    metadataTable->getHeader().addColumn("X", 6, 60);
    metadataTable->getHeader().addColumn("Y", 7, 60);
    metadataTable->getHeader().addColumn("Z", 8, 60);
    metadataTable->getHeader().addColumn("Surface", 9, 80);
    metadataTable->getHeader().addColumn("Target Structure", 10, 220);
    metadataTable->getHeader().addColumn("Dye", 11, 80);
    metadataTable->getHeader().addColumn("Notes", 12, 150);
    metadataTable->getHeader().addColumn("Delete", 13, 60);
    
    // Create some initial values
    initializeTable();
    
    // Create and start the stage listener
    stageListener = std::make_unique<StageListener>(this);
    stageListener->startListening();
}


EphysMetadataGeneratorCanvas::~EphysMetadataGeneratorCanvas()
{

}


void EphysMetadataGeneratorCanvas::resized()
{
    const int buttonHeight = 30;
    const int margin = 10;
    const int bottomControlsHeight = 40; // Height for bottom controls (Add Row + Server)
    
    if (metadataTable != nullptr && addRowButton != nullptr)
    {
        // Calculate available space for table (leave room for bottom controls)
        int tableHeight = getHeight() - (bottomControlsHeight + 2 * margin + margin); // Extra margin for spacing
        
        // Position the table at the top
        metadataTable->setBounds(margin, margin, getWidth() - 2 * margin, tableHeight);
        
        // Position controls at the bottom
        int bottomY = margin + tableHeight + margin;
        
        // Server address controls dimensions
        int labelWidth = 80;
        int editorWidth = 200;
        int connectButtonWidth = 200;
        int addButtonWidth = 200;
        
        // Calculate total width needed for all bottom controls
        int totalBottomControlsWidth = labelWidth + editorWidth + connectButtonWidth + addButtonWidth + 4 * margin;
        
        // Center all bottom controls horizontally
        int startX = (getWidth() - totalBottomControlsWidth) / 2;
        
        // Position server address controls on the left side of bottom area
        if (serverAddressLabel != nullptr && serverAddressEditor != nullptr && connectButton != nullptr)
        {
            serverAddressLabel->setBounds(startX, bottomY, labelWidth, buttonHeight);
            serverAddressEditor->setBounds(startX + labelWidth + margin, bottomY, editorWidth, buttonHeight);
            connectButton->setBounds(startX + labelWidth + editorWidth + 2 * margin, bottomY, connectButtonWidth, buttonHeight);
        }
        
        // Position the Add Row button on the right side of bottom area
        int addButtonX = startX + labelWidth + editorWidth + connectButtonWidth + 3 * margin;
        addRowButton->setBounds(addButtonX, bottomY, addButtonWidth, buttonHeight);
    }
    else if (metadataTable != nullptr)
    {
        // If no add button, use most of height for table, but leave room for server controls
        int tableHeight = getHeight() - (bottomControlsHeight + 2 * margin);
        metadataTable->setBounds(margin, margin, getWidth() - 2 * margin, tableHeight);
        
        // Position server controls at bottom center
        if (serverAddressLabel != nullptr && serverAddressEditor != nullptr && connectButton != nullptr)
        {
            int labelWidth = 80;
            int editorWidth = 150;
            int connectButtonWidth = 80;
            int totalServerControlsWidth = labelWidth + editorWidth + connectButtonWidth + 2 * margin;
            int serverX = (getWidth() - totalServerControlsWidth) / 2;
            int bottomY = margin + tableHeight + margin;
            
            serverAddressLabel->setBounds(serverX, bottomY, labelWidth, buttonHeight);
            serverAddressEditor->setBounds(serverX + labelWidth + margin, bottomY, editorWidth, buttonHeight);
            connectButton->setBounds(serverX + labelWidth + editorWidth + 2 * margin, bottomY, connectButtonWidth, buttonHeight);
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
        serverAddressLabel->setColour(Label::textColourId, findColour(ThemeColours::defaultText));
    }
    
    if (serverAddressEditor != nullptr)
    {
        serverAddressEditor->setColour(TextEditor::backgroundColourId, Colours::white);
        serverAddressEditor->setColour(TextEditor::textColourId, Colours::black);
        serverAddressEditor->setColour(TextEditor::outlineColourId, Colours::grey);
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
    
    
}


void EphysMetadataGeneratorCanvas::paint(Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour (ThemeColours::windowBackground));
}

// TableListBoxModel methods
int EphysMetadataGeneratorCanvas::getNumRows()
{
    return probeMetadata.size();
}

void EphysMetadataGeneratorCanvas::paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll(Colour(0x44000000));
    else if (rowNumber % 2)
        g.fillAll(Colour(0x22000000));
}

void EphysMetadataGeneratorCanvas::paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    g.setColour(getLookAndFeel().findColour(Label::textColourId));
    g.setFont(14.0f);
    
    if (rowNumber < probeMetadata.size())
    {
        // Do not draw text for Surface, Dye, Notes, and Delete columns, handled by buttons
        if (columnId == 9 || columnId == 10 || columnId == 11 || columnId == 12 || columnId == 13)
            return;
        String text;
        const ProbeMetadata& row = probeMetadata[rowNumber];
        switch (columnId)
        {
            case 1: text = row.probe; break;
            case 2: text = row.manipulator; break;
            case 3: text = row.arcAngle; break;
            case 4: text = row.moduleAngle; break;
            case 5: text = row.rotation; break;
            case 6: text = row.x; break;
            case 7: text = row.y; break;
            case 8: text = row.z; break;
            case 9: text = row.surface; break;
        }
        g.drawText(text, 2, 0, width - 4, height, Justification::centredLeft, true);
    }
}

void EphysMetadataGeneratorCanvas::initializeTable()
{
    
    
    // Add some sample data
    ProbeMetadata sample1;
    sample1.probe = "Probe A";
    sample1.manipulator = "Manipulator 1";
    sample1.arcAngle = "45";
    sample1.moduleAngle = "30";
    sample1.rotation = "0";
    sample1.x = "1.5";
    sample1.y = "2.0";
    sample1.z = "3.5";
    sample1.surface = "0.0";
    sample1.targetStructure = "";
    sample1.dye = "CM-DiI";
    sample1.notes = "";
    
    ProbeMetadata sample2;
    sample2.probe = "Probe B";
    sample2.manipulator = "Manipulator 2";
    sample2.arcAngle = "60";
    sample2.moduleAngle = "45";
    sample2.rotation = "90";
    sample2.x = "2.0";
    sample2.y = "1.5";
    sample2.z = "4.0";
    sample2.surface = "0.0";
    sample2.targetStructure = "";
    sample2.dye = "CM-DiI";
    sample2.notes = "";
    
    ProbeMetadata sample3;
    sample3.probe = "Probe C";
    sample3.manipulator = "Manipulator 3";
    sample3.arcAngle = "60";
    sample3.moduleAngle = "45";
    sample3.rotation = "90";
    sample3.x = "2.0";
    sample3.y = "1.5";
    sample3.z = "900.0";
    sample3.surface = "0.0";
    sample3.targetStructure = "";
    sample3.dye = "CM-DiI";
    sample3.notes = "";
    
    probeMetadata.add(sample1);
    probeMetadata.add(sample2);
    probeMetadata.add(sample3);
    
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
            label->setColour(Label::backgroundColourId, Colours::white.withAlpha(0.0f));
            label->setColour(Label::textColourId, findColour(ThemeColours::defaultText));
            label->setColour(Label::outlineColourId, Colours::white.withAlpha(0.0f));
            
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
        else
        {
            label->setColour (Label::textColourId, findColour (ThemeColours::defaultText));
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
                button->setColour(TextButton::buttonColourId, Colours::lightgrey);
                button->setColour(TextButton::textColourOnId, Colours::black);
            }
        }
        
        return button;
    }
    
    // AutoCompleteTextEditor for the Target Structure column (column 10)
    if (columnId == 10)
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
    
    // Button for the Dye column (column 11)
    if (columnId == 11)
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
            if (currentDye == "CM-DiI")
            {
                button->setButtonText("CM-DiI");
                button->setColour(TextButton::buttonColourId, Colours::darkred);
                button->setColour(TextButton::textColourOnId, Colours::white);
            }
            else if (currentDye == "DS-DiD")
            {
                button->setButtonText("DS-DiD");
                button->setColour(TextButton::buttonColourId, Colours::darkblue);
                button->setColour(TextButton::textColourOnId, Colours::white);
            }
            else if (currentDye == "None")
            {
                button->setButtonText("None");
                button->setColour(TextButton::buttonColourId, Colours::darkgrey);
                button->setColour(TextButton::textColourOnId, Colours::white);
            }
            else
            {
                // Default to CM-DiI
                button->setButtonText("CM-DiI");
                button->setColour(TextButton::buttonColourId, Colours::darkred);
                button->setColour(TextButton::textColourOnId, Colours::white);
                probeMetadata.getReference(rowNumber).dye = "CM-DiI";
            }
        }
        
        return button;
    }
    
    // Button for the Notes column (column 12)
    if (columnId == 12)
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
                button->setColour(TextButton::textColourOnId, findColour(ThemeColours::defaultText));
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
                button->setColour(TextButton::textColourOnId, findColour(ThemeColours::defaultText));
            }
            
            button->setButtonText(displayText);
        }
        
        return button;
    }
    
    // Button for the Delete column (column 13)
    if (columnId == 13)
    {
        TextButton* button = dynamic_cast<TextButton*>(existingComponentToUpdate);
        
        if (button == nullptr)
        {
            button = new TextButton();
            button->setName("DeleteButton_" + String(rowNumber));
            button->addListener(this);
        }
        
        // Set the button appearance for delete action
        button->setButtonText("X");
        button->setColour(TextButton::buttonColourId, Colours::darkred);
        button->setColour(TextButton::textColourOnId, Colours::white);
        
        // Make the button slightly smaller and centered
        button->setSize(40, 20);
        
        return button;
    }
    
    return nullptr;
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
        // Create a new probe metadata entry with default values
        ProbeMetadata newProbe;
        newProbe.probe = "Probe " + String(char('A' + probeMetadata.size())); // Generate next letter
        newProbe.manipulator = "62219"; // Default manipulator
        newProbe.arcAngle = "0";
        newProbe.moduleAngle = "0";
        newProbe.rotation = "0";
        newProbe.x = "0.0";
        newProbe.y = "0.0";
        newProbe.z = "0.0";
        newProbe.surface = "0.0";
        newProbe.targetStructure = "";
        newProbe.dye = "CM-DiI"; // Default dye
        newProbe.notes = "";
        
        // Add the new probe to the data array
        probeMetadata.add(newProbe);
        
        // Update the table to show the new row
        metadataTable->updateContent();
        metadataTable->repaint();
        
        LOGD("Added new probe row: ", newProbe.probe, " (total rows: ", probeMetadata.size(), ")");
        
        return; // Early return to avoid checking other button types
    }
    
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
                surfaceButton->setColour(TextButton::buttonColourId, Colours::lightgrey);
                surfaceButton->setColour(TextButton::textColourOnId, Colours::black);
                
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
    // Check if it's a dye button
    if (buttonName.startsWith("DyeButton_"))
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
                LOGD("Dye for row ", rowNumber, " changed to DS-DiD");
            }
            else if (currentDye == "DS-DiD")
            {
                // Switch to None
                probeMetadata.getReference(rowNumber).dye = "None";
                dyeButton->setButtonText("None");
                dyeButton->setColour(TextButton::buttonColourId, Colours::darkgrey);
                dyeButton->setColour(TextButton::textColourOnId, Colours::white);
                LOGD("Dye for row ", rowNumber, " changed to None");
            }
            else {
                // Switch back to CM-DiI
                probeMetadata.getReference(rowNumber).dye = "CM-DiI";
                dyeButton->setButtonText("CM-DiI");
                dyeButton->setColour(TextButton::buttonColourId, Colours::darkred);
                dyeButton->setColour(TextButton::textColourOnId, Colours::white);
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
        
        if (rowNumber < probeMetadata.size())
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
                probe.x = stagePos.x;
                probe.y = stagePos.y;
                probe.z = stagePos.z;
                
                LOGD("Updated probe row ", probeIndex, " (manipulator: ", probe.manipulator, 
                     ") with stage position from serial ", stagePos.serialNumber, 
                     ": X=", stagePos.x, " Y=", stagePos.y, " Z=", stagePos.z);
                
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
    metadataTable->updateContent();
    metadataTable->repaint();
}

void EphysMetadataGeneratorCanvas::stagePositionError(const String& errorMessage)
{
    LOGE("Stage position fetch error: ", errorMessage);
    
    // You could show an error message to the user here, for example:
    // AlertWindow::showMessageBox(AlertWindow::WarningIcon, "Stage Position Error", errorMessage);
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
        probeElement->setAttribute("manipulator", probe.manipulator);
        probeElement->setAttribute("arcAngle", probe.arcAngle);
        probeElement->setAttribute("moduleAngle", probe.moduleAngle);
        probeElement->setAttribute("rotation", probe.rotation);
        probeElement->setAttribute("x", probe.x);
        probeElement->setAttribute("y", probe.y);
        probeElement->setAttribute("z", probe.z);
        probeElement->setAttribute("surface", probe.surface);
        probeElement->setAttribute("targetStructure", probe.targetStructure);
        probeElement->setAttribute("dye", probe.dye);
        probeElement->setAttribute("notes", probe.notes);
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
                probe.manipulator = probeElement->getStringAttribute("manipulator", "62219");
                probe.arcAngle = probeElement->getStringAttribute("arcAngle", "0");
                probe.moduleAngle = probeElement->getStringAttribute("moduleAngle", "0");
                probe.rotation = probeElement->getStringAttribute("rotation", "0");
                probe.x = probeElement->getStringAttribute("x", "0.0");
                probe.y = probeElement->getStringAttribute("y", "0.0");
                probe.z = probeElement->getStringAttribute("z", "0.0");
                probe.surface = probeElement->getStringAttribute("surface", "0.0");
                probe.targetStructure = probeElement->getStringAttribute("targetStructure", "");
                probe.dye = probeElement->getStringAttribute("dye", "CM-DiI");
                probe.notes = probeElement->getStringAttribute("notes", "");
                
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
        
        // Update the connect button to show it's connected
        if (connectButton != nullptr)
        {
            connectButton->setButtonText("Connected");
            connectButton->setColour(TextButton::buttonColourId, Colours::green);
            connectButton->setColour(TextButton::textColourOnId, Colours::white);
        }
    }
    else
    {
        // Start listening with the new endpoint
        stageListener->startListening();
        LOGD("Started stage listener with new endpoint: ", newEndpoint);
        
        // Update the connect button to show it's connected
        if (connectButton != nullptr)
        {
            connectButton->setButtonText("Connected");
            connectButton->setColour(TextButton::buttonColourId, Colours::green);
            connectButton->setColour(TextButton::textColourOnId, Colours::white);
        }
    }
}

Metadata::Acquisition EphysMetadataGeneratorCanvas::getLatestMetadata()
{

    Metadata::Acquisition acquisition;

    acquisition.subject_id = "Subject ID";
    acquisition.acquisition_start_time = Time::getCurrentTime();

    acquisition.acquisition_type = "Extracellular ephys experiment";

    return acquisition;


}