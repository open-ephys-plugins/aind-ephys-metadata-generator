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

// NotesEditorPopup Implementation
NotesEditorPopup::NotesEditorPopup(const String& initialText, std::function<void(const String&)> onTextChanged)
    : onTextChangedCallback(onTextChanged)
{
    textEditor = std::make_unique<TextEditor>();
    textEditor->setMultiLine(true);
    textEditor->setReturnKeyStartsNewLine(false);
    textEditor->setText(initialText);
    textEditor->addListener(this);
    textEditor->setWantsKeyboardFocus(true);
    addAndMakeVisible(textEditor.get());
    
    setSize(300, 200);
}

NotesEditorPopup::~NotesEditorPopup()
{
}

void NotesEditorPopup::paint(Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
    g.setColour(Colours::black);
    g.drawRect(getLocalBounds(), 2);
    
    // Draw title
    g.setColour(getLookAndFeel().findColour(Label::textColourId));
    g.setFont(14.0f);
    g.drawText("Edit Notes (Press Enter to save)", 5, 5, getWidth() - 10, 20, Justification::centred);
}

void NotesEditorPopup::resized()
{
    textEditor->setBounds(10, 30, getWidth() - 20, getHeight() - 40);
}

void NotesEditorPopup::textEditorReturnKeyPressed(TextEditor& editor)
{
    if (onTextChangedCallback)
        onTextChangedCallback(editor.getText());
    
    // Close the popup
    if (auto* parent = getParentComponent())
    {
        parent->removeChildComponent(this);
        delete this;
    }
}

void NotesEditorPopup::textEditorEscapeKeyPressed(TextEditor& editor)
{
    // Cancel editing - close without saving
    if (auto* parent = getParentComponent())
    {
        parent->removeChildComponent(this);
        delete this;
    }
}

void NotesEditorPopup::textEditorFocusLost(TextEditor& editor)
{
    // Save changes when focus is lost
    if (onTextChangedCallback)
        onTextChangedCallback(editor.getText());
    
    // Close the popup
    if (auto* parent = getParentComponent())
    {
        parent->removeChildComponent(this);
        delete this;
    }
}

// AutoCompleteSuggestionList Implementation
AutoCompleteSuggestionList::AutoCompleteSuggestionList(const StringArray& suggestions, std::function<void(const String&)> onSelection)
    : filteredSuggestions(suggestions), onSelectionCallback(onSelection)
{
    listBox = std::make_unique<ListBox>();
    listBox->setModel(this);
    listBox->setRowHeight(20);
    listBox->setOutlineThickness(1);
    listBox->setColour(ListBox::outlineColourId, Colours::black);
    listBox->setColour(ListBox::backgroundColourId, Colours::white);
    addAndMakeVisible(listBox.get());
    
    setSize(200, jmin(100, suggestions.size() * 20 + 2));
}

AutoCompleteSuggestionList::~AutoCompleteSuggestionList()
{
}

void AutoCompleteSuggestionList::paint(Graphics& g)
{
    g.setColour(Colours::white);
    g.fillRect(getLocalBounds());
    g.setColour(Colours::black);
    g.drawRect(getLocalBounds(), 1);
}

void AutoCompleteSuggestionList::resized()
{
    listBox->setBounds(getLocalBounds().reduced(1));
}

void AutoCompleteSuggestionList::updateSuggestions(const StringArray& newSuggestions)
{
    filteredSuggestions = newSuggestions;
    listBox->updateContent();
    setSize(200, jmin(100, filteredSuggestions.size() * 20 + 2));
}

void AutoCompleteSuggestionList::selectNext()
{
    int currentRow = listBox->getSelectedRow();
    int nextRow = (currentRow + 1) % filteredSuggestions.size();
    listBox->selectRow(nextRow);
}

void AutoCompleteSuggestionList::selectPrevious()
{
    int currentRow = listBox->getSelectedRow();
    int prevRow = (currentRow - 1 + filteredSuggestions.size()) % filteredSuggestions.size();
    listBox->selectRow(prevRow);
}

void AutoCompleteSuggestionList::selectCurrent()
{
    int selectedRow = listBox->getSelectedRow();
    if (selectedRow >= 0 && selectedRow < filteredSuggestions.size())
    {
        if (onSelectionCallback)
            onSelectionCallback(filteredSuggestions[selectedRow]);
    }
}

int AutoCompleteSuggestionList::getNumRows()
{
    return filteredSuggestions.size();
}

void AutoCompleteSuggestionList::paintListBoxItem(int rowNumber, Graphics& g, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
    {
        g.setColour(Colour(0x3f7fbfff));
        g.fillRect(0, 0, width, height);
    }
    
    g.setColour(Colours::black);
    g.setFont(14.0f);
    
    if (rowNumber < filteredSuggestions.size())
    {
        g.drawText(filteredSuggestions[rowNumber], 4, 0, width - 8, height, Justification::centredLeft);
    }
}

void AutoCompleteSuggestionList::listBoxItemClicked(int row, const MouseEvent& e)
{
    if (row >= 0 && row < filteredSuggestions.size())
    {
        if (onSelectionCallback)
            onSelectionCallback(filteredSuggestions[row]);
    }
}

void AutoCompleteSuggestionList::mouseEnter(const MouseEvent& event)
{
    // Don't do anything special, just ensure we can receive mouse events
    Component::mouseEnter(event);
}

void AutoCompleteSuggestionList::mouseExit(const MouseEvent& event)
{
    // Don't do anything special, just ensure we can receive mouse events
    Component::mouseExit(event);
}

bool AutoCompleteSuggestionList::hitTest(int x, int y)
{
    // Always return true for mouse hit testing to ensure we can capture clicks
    return true;
}

// AutoCompleteTextEditor Implementation
AutoCompleteTextEditor::AutoCompleteTextEditor(const StringArray& suggestions)
    : allSuggestions(suggestions)
{
    addListener(this);
    setWantsKeyboardFocus(true);
}

AutoCompleteTextEditor::~AutoCompleteTextEditor()
{
    hideSuggestions();
}

void AutoCompleteTextEditor::textEditorTextChanged(TextEditor& editor)
{
    String currentText = getText();
    
    if (currentText.isEmpty())
    {
        hideSuggestions();
        return;
    }
    
    StringArray filtered = filterSuggestions(currentText);
    
    if (filtered.size() > 0)
    {
        showSuggestions(filtered);
    }
    else
    {
        hideSuggestions();
    }
}

bool AutoCompleteTextEditor::keyPressed(const KeyPress& key)
{
    if (suggestionList != nullptr && suggestionList->isVisible())
    {
        // Only handle specific navigation keys when suggestions are visible
        if (key == KeyPress::downKey)
        {
            suggestionList->selectNext();
            return true;
        }
        else if (key == KeyPress::upKey)
        {
            suggestionList->selectPrevious();
            return true;
        }
        else if (key == KeyPress::returnKey || key == KeyPress::tabKey)
        {
            suggestionList->selectCurrent();
            return true;
        }
        else if (key == KeyPress::escapeKey)
        {
            hideSuggestions();
            return true;
        }
        // For all other keys (including delete, backspace, typing), let the TextEditor handle them
        // This ensures normal text editing functionality continues to work
    }
    
    // Always call the parent TextEditor's keyPressed for normal text editing
    return TextEditor::keyPressed(key);
}

void AutoCompleteTextEditor::focusLost(FocusChangeType cause)
{
    // Only hide suggestions if focus is lost for reasons other than clicking on the suggestion list
    // Use a longer delay to allow for suggestion list interaction
    Timer::callAfterDelay(150, [this]() {
        // Check if the suggestion list itself has focus or if we got focus back
        if (hasKeyboardFocus(false) || (suggestionList && suggestionList->hasKeyboardFocus(true)))
        {
            return; // Don't hide if we or the suggestion list has focus
        }
        hideSuggestions();
        
        // Validate the current text against available suggestions
        validateText();
    });
    
    TextEditor::focusLost(cause);
}

void AutoCompleteTextEditor::setSuggestions(const StringArray& newSuggestions)
{
    allSuggestions = newSuggestions;
}

void AutoCompleteTextEditor::showSuggestions(const StringArray& suggestions)
{
    if (suggestionList == nullptr)
    {
        suggestionList = std::make_unique<AutoCompleteSuggestionList>(suggestions, 
            [this](const String& selection)
            {
                setText(selection);
                hideSuggestions();
                grabKeyboardFocus();
            });
    }
    else
    {
        suggestionList->updateSuggestions(suggestions);
    }
    
    // Find the top-level canvas to add the suggestion list to avoid clipping
    Component* topLevelParent = this;
    while (topLevelParent->getParentComponent() != nullptr)
    {
        topLevelParent = topLevelParent->getParentComponent();
    }
    
    // Position the suggestion list below the text editor in global coordinates
    if (topLevelParent != nullptr)
    {
        Rectangle<int> bounds = getBounds();
        Point<int> globalPosition = topLevelParent->getLocalPoint(this, Point<int>(0, getHeight()));
        suggestionList->setTopLeftPosition(globalPosition);
        
        topLevelParent->addAndMakeVisible(suggestionList.get());
        suggestionList->toFront(false);  // Bring to front but don't grab focus
        
        // Select the first item by default
        if (suggestions.size() > 0)
        {
            suggestionList->listBox->selectRow(0);
        }
        
        // Ensure the TextEditor maintains keyboard focus
        grabKeyboardFocus();
    }
}

void AutoCompleteTextEditor::hideSuggestions()
{
    if (suggestionList != nullptr)
    {
        if (auto* parent = suggestionList->getParentComponent())
        {
            parent->removeChildComponent(suggestionList.get());
        }
        suggestionList.reset();
    }
}

StringArray AutoCompleteTextEditor::filterSuggestions(const String& input)
{
    StringArray filtered;
    String lowerInput = input.toLowerCase();
    
    for (const String& suggestion : allSuggestions)
    {
        if (suggestion.toLowerCase().startsWith(lowerInput))
        {
            filtered.add(suggestion);
        }
    }
    
    // Limit to maximum 10 suggestions
    while (filtered.size() > 10)
    {
        filtered.removeRange(10, filtered.size() - 10);
    }
    
    return filtered;
}

void AutoCompleteTextEditor::validateText()
{
    String currentText = getText();
    
    // If text is empty, nothing to validate
    if (currentText.isEmpty())
        return;
        
    // Check if the current text exactly matches any suggestion
    bool isValidSuggestion = false;
    String lowerCurrentText = currentText.toLowerCase();
    
    for (const String& suggestion : allSuggestions)
    {
        if (suggestion.toLowerCase() == lowerCurrentText)
        {
            isValidSuggestion = true;
            break;
        }
    }
    
    // If the text doesn't match any suggestion, clear it
    if (!isValidSuggestion)
    {
        setText("", sendNotification);
    }
}


EphysMetadataGeneratorCanvas::EphysMetadataGeneratorCanvas(EphysMetadataGenerator* processor_)
    : processor(processor_)
{
    // Create and configure the metadata table
    metadataTable = std::make_unique<TableListBox>();
    metadataTable->setModel(this);
    metadataTable->setColour(ListBox::outlineColourId, Colours::grey);
    metadataTable->setOutlineThickness(1);
    addAndMakeVisible(metadataTable.get());
    
    initializeTable();
}


EphysMetadataGeneratorCanvas::~EphysMetadataGeneratorCanvas()
{

}


void EphysMetadataGeneratorCanvas::resized()
{
    if (metadataTable != nullptr)
    {
        metadataTable->setBounds(10, 10, getWidth() - 20, getHeight() - 20);
    }
}

void EphysMetadataGeneratorCanvas::updateSettings()
{
    
    
}

void EphysMetadataGeneratorCanvas::refreshState()
{

}


void EphysMetadataGeneratorCanvas::refresh()
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
        // Do not draw text for Dye column and Notes column, handled by buttons
        if (columnId == 10 || columnId == 11 || columnId == 12)
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
    
    probeData.add(sample1);
    probeData.add(sample2);
    
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
    
    // AutoCompleteTextEditor for the Target Structure column (column 10)
    if (columnId == 10)
    {
        AutoCompleteTextEditor* autoCompleteEditor = dynamic_cast<AutoCompleteTextEditor*>(existingComponentToUpdate);
        
        if (autoCompleteEditor == nullptr)
        {
            // Create common brain region suggestions
            CCFRegionList regionList;
            
            autoCompleteEditor = new AutoCompleteTextEditor(regionList.getRegionNames());
            autoCompleteEditor->setName("TargetStructure_" + String(rowNumber));
            
            // Add listener to update data when text changes
            autoCompleteEditor->onTextChange = [this, autoCompleteEditor, rowNumber]()
            {
                if (rowNumber < probeData.size())
                {
                    probeData.getReference(rowNumber).targetStructure = autoCompleteEditor->getText();
                    LOGD("Target structure for row ", rowNumber, " changed to: ", autoCompleteEditor->getText());
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
