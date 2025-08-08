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
    //stageListener->startListening();
}


EphysMetadataGeneratorCanvas::~EphysMetadataGeneratorCanvas()
{

}


void EphysMetadataGeneratorCanvas::resized()
{
    const int buttonHeight = 30;
    const int margin = 10;
    
    if (metadataTable != nullptr && addRowButton != nullptr)
    {
        // Calculate available space for table (leave room for button at bottom)
        int tableHeight = getHeight() - (buttonHeight + 2 * margin + margin); // Extra margin for spacing
        
        // Position the table at the top
        metadataTable->setBounds(margin, margin, getWidth() - 2 * margin, tableHeight);
        
        // Position the Add Row button at the bottom, centered
        int buttonY = margin + tableHeight + margin;
        int buttonX = (getWidth() - 100) / 2; // Center the button horizontally
        addRowButton->setBounds(buttonX, buttonY, 100, buttonHeight);
    }
    else if (metadataTable != nullptr)
    {
        // If no button, use full height for table
        metadataTable->setBounds(margin, margin, getWidth() - 2 * margin, getHeight() - 2 * margin);
    }
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
    return probeData.size();
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
    
    if (rowNumber < probeData.size())
    {
        // Do not draw text for Surface, Dye, Notes, and Delete columns, handled by buttons
        if (columnId == 9 || columnId == 10 || columnId == 11 || columnId == 12 || columnId == 13)
            return;
        String text;
        const ProbeMetadata& row = probeData[rowNumber];
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
    
    probeData.add(sample1);
    probeData.add(sample2);
    probeData.add(sample3);
    
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
            comboBox->addItem("62219", 1);
            comboBox->addItem("62220", 2);
            comboBox->addItem("62221", 3);
            comboBox->setName("Manipulator");
            comboBox->addListener(this);
        }
        
        // Set the current selection based on the data
        if (rowNumber < probeData.size())
        {
            String currentValue = probeData[rowNumber].manipulator;
            if (currentValue == "62219") comboBox->setSelectedId(1);
            else if (currentValue == "62220") comboBox->setSelectedId(2);
            else if (currentValue == "62221") comboBox->setSelectedId(3);
            else comboBox->setSelectedId(1); // Default to first option
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
                    if (rowNumber < probeData.size())
                    {
                        String validValue;
                        if (columnId == 3) validValue = probeData[rowNumber].arcAngle;
                        else if (columnId == 4) validValue = probeData[rowNumber].moduleAngle;
                        else if (columnId == 5) validValue = probeData[rowNumber].rotation;
                        
                        label->setText(validValue, dontSendNotification);
                        return;
                    }
                }
                
                // Update the data if valid
                if (rowNumber < probeData.size())
                {
                    if (columnId == 3) probeData.getReference(rowNumber).arcAngle = newText;
                    else if (columnId == 4) probeData.getReference(rowNumber).moduleAngle = newText;
                    else if (columnId == 5) probeData.getReference(rowNumber).rotation = newText;
                    
                    LOGD("Angle value for row ", rowNumber, " column ", columnId, " changed to: ", newText);
                }
            };
        }
        
        // Set the current text based on the data
        if (rowNumber < probeData.size())
        {
            String currentValue;
            if (columnId == 3) currentValue = probeData[rowNumber].arcAngle;
            else if (columnId == 4) currentValue = probeData[rowNumber].moduleAngle;
            else if (columnId == 5) currentValue = probeData[rowNumber].rotation;
            
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
        if (rowNumber < probeData.size())
        {
            String currentSurface = probeData[rowNumber].surface;
            String currentZ = probeData[rowNumber].z;
            
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
                if (rowNumber < probeData.size())
                {
                    probeData.getReference(rowNumber).targetStructure = autoCompleteEditor->getText();
                    //LOGD("Target structure for row ", rowNumber, " changed to: ", autoCompleteEditor->getText());
                }
            };
        }
        
        // Set the current text based on the data
        if (rowNumber < probeData.size())
        {
            String currentValue = probeData[rowNumber].targetStructure;
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
        if (rowNumber < probeData.size())
        {
            String currentDye = probeData[rowNumber].dye;
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
                probeData.getReference(rowNumber).dye = "CM-DiI";
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
        if (rowNumber < probeData.size())
        {
            String fullNotes = probeData[rowNumber].notes;
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
    LOGD("ComboBox: ", comboBoxThatHasChanged->getName(), " changed to ", comboBoxThatHasChanged->getText());
}


void EphysMetadataGeneratorCanvas::labelTextChanged(Label* labelThatHasChanged)
{
    LOGD("Label: ", labelThatHasChanged->getName(), " changed to ", labelThatHasChanged->getText());
}

void EphysMetadataGeneratorCanvas::buttonClicked(Button* buttonThatWasClicked)
{
    String buttonName = buttonThatWasClicked->getName();
    
    // Check if it's the Add Row button
    if (buttonName == "AddRowButton")
    {
        // Create a new probe metadata entry with default values
        ProbeMetadata newProbe;
        newProbe.probe = "Probe " + String(char('A' + probeData.size())); // Generate next letter
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
        probeData.add(newProbe);
        
        // Update the table to show the new row
        metadataTable->updateContent();
        metadataTable->repaint();
        
        LOGD("Added new probe row: ", newProbe.probe, " (total rows: ", probeData.size(), ")");
        
        return; // Early return to avoid checking other button types
    }
    
    // Check if it's a surface button
    if (buttonName.startsWith("SurfaceButton_"))
    {
        // Extract the row number from the button name
        String rowString = buttonName.substring(14); // Remove "SurfaceButton_" prefix
        int rowNumber = rowString.getIntValue();
        
        if (rowNumber < probeData.size())
        {
            TextButton* surfaceButton = static_cast<TextButton*>(buttonThatWasClicked);
            String currentSurface = probeData[rowNumber].surface;
            String currentZ = probeData[rowNumber].z;
            
            // Check if surface is currently set from Z (button is active/green)
            bool isSurfaceSetFromZ = (currentSurface == currentZ && currentSurface != "0.0" && !currentSurface.isEmpty());
            
            if (isSurfaceSetFromZ)
            {
                // Button is currently active - revert to inactive state
                probeData.getReference(rowNumber).surface = "0.0";
                
                // Update button appearance to inactive state
                surfaceButton->setButtonText("Set Surface");
                surfaceButton->setColour(TextButton::buttonColourId, Colours::lightgrey);
                surfaceButton->setColour(TextButton::textColourOnId, Colours::black);
                
                LOGD("Surface for row ", rowNumber, " cleared (reverted to inactive)");
            }
            else
            {
                // Button is currently inactive - set surface from Z value
                probeData.getReference(rowNumber).surface = currentZ;
                
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
        
        if (rowNumber < probeData.size())
        {
            TextButton* dyeButton = static_cast<TextButton*>(buttonThatWasClicked);
            String currentDye = probeData[rowNumber].dye;
            
            // Toggle between the three dye options
            if (currentDye == "CM-DiI")
            {
                // Switch to DS-DiD
                probeData.getReference(rowNumber).dye = "DS-DiD";
                dyeButton->setButtonText("DS-DiD");
                dyeButton->setColour(TextButton::buttonColourId, Colours::darkblue);
                dyeButton->setColour(TextButton::textColourOnId, Colours::white);
                LOGD("Dye for row ", rowNumber, " changed to DS-DiD");
            }
            else if (currentDye == "DS-DiD")
            {
                // Switch to None
                probeData.getReference(rowNumber).dye = "None";
                dyeButton->setButtonText("None");
                dyeButton->setColour(TextButton::buttonColourId, Colours::darkgrey);
                dyeButton->setColour(TextButton::textColourOnId, Colours::white);
                LOGD("Dye for row ", rowNumber, " changed to None");
            }
            else {
                // Switch back to CM-DiI
                probeData.getReference(rowNumber).dye = "CM-DiI";
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
        
        if (rowNumber < probeData.size())
        {
            // Show confirmation dialog before deleting
            String probeIdentifier = probeData[rowNumber].probe;
            
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
                probeData.remove(rowNumber);
                
                // Update the table to reflect the deletion
                metadataTable->updateContent();
                metadataTable->repaint();
                
                LOGD("Deleted probe row: ", probeIdentifier, " (remaining rows: ", probeData.size(), ")");
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
        
        if (rowNumber < probeData.size())
        {
            // Create and show the popup editor
            String currentNotes = probeData[rowNumber].notes;
            
            auto popup = new NotesEditorPopup(currentNotes, [this, rowNumber](const String& newText)
            {
                // Update the notes data
                probeData.getReference(rowNumber).notes = newText;
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
    
    // Update the X, Y, Z values for each row based on the received positions
    for (int i = 0; i < jmin(positions.size(), probeData.size()); ++i)
    {
        const StagePosition& pos = positions[i];
        
        // Update the probe data
        probeData.getReference(i).x = pos.x;
        probeData.getReference(i).y = pos.y;
        probeData.getReference(i).z = pos.z;
        
        LOGD("Updated row ", i, " with position: X=", pos.x, " Y=", pos.y, " Z=", pos.z);
    }
    
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
    XmlElement* probeDataElement = xml->createNewChildElement("PROBE_METADATA");
    
    for (int i = 0; i < probeData.size(); ++i)
    {
        const ProbeMetadata& probe = probeData[i];
        
        XmlElement* probeElement = probeDataElement->createNewChildElement("PROBE");
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
    
    LOGD("Saved ", probeData.size(), " probe metadata entries to XML");
}

void EphysMetadataGeneratorCanvas::loadCustomParametersFromXml(XmlElement* xml)
{
    // Clear existing probe data
    probeData.clear();
    
    XmlElement* probeDataElement = xml->getChildByName("PROBE_METADATA");
    
    if (probeDataElement != nullptr)
    {
        // Load each probe entry
        forEachXmlChildElement(*probeDataElement, probeElement)
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
                
                probeData.add(probe);
            }
        }
        
        LOGD("Loaded ", probeData.size(), " probe metadata entries from XML");
    }
    else
    {
        // If no saved data found, initialize with default sample data
        LOGD("No probe metadata found in XML, using default sample data");
        initializeTable(); // This will create the default sample data
        return; // Exit early since initializeTable() already updates the table
    }
    
    // Update the table to reflect the loaded data
    if (metadataTable != nullptr)
    {
        metadataTable->updateContent();
        metadataTable->repaint();
    }
}

