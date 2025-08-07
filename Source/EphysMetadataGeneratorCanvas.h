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

class EphysMetadataGenerator;

/**
 * AutoComplete suggestion list component
 */
class AutoCompleteSuggestionList : public Component,
                                   public ListBoxModel
{
public:
    AutoCompleteSuggestionList(const StringArray& suggestions, std::function<void(const String&)> onSelection);
    ~AutoCompleteSuggestionList();
    
    void paint(Graphics& g) override;
    void resized() override;
    
    // Mouse handling to prevent focus loss
    void mouseEnter(const MouseEvent& event) override;
    void mouseExit(const MouseEvent& event) override;
    bool hitTest(int x, int y) override;
    
    void updateSuggestions(const StringArray& newSuggestions);
    void selectNext();
    void selectPrevious();
    void selectCurrent();
    
    // ListBoxModel
    int getNumRows() override;
    void paintListBoxItem(int rowNumber, Graphics& g, int width, int height, bool rowIsSelected) override;
    void listBoxItemClicked(int row, const MouseEvent& e) override;
    
    std::unique_ptr<ListBox> listBox;  // Make public so AutoCompleteTextEditor can access it
    
private:
    StringArray filteredSuggestions;
    std::function<void(const String&)> onSelectionCallback;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AutoCompleteSuggestionList)
};

/**
 * TextEditor with autocomplete functionality
 */
class AutoCompleteTextEditor : public TextEditor,
                               public TextEditor::Listener
{
public:
    AutoCompleteTextEditor(const StringArray& suggestions);
    ~AutoCompleteTextEditor();
    
    void textEditorTextChanged(TextEditor& editor) override;
    bool keyPressed(const KeyPress& key) override;
    void focusLost(FocusChangeType cause) override;
    
    void setSuggestions(const StringArray& newSuggestions);
    
private:
    StringArray allSuggestions;
    std::unique_ptr<AutoCompleteSuggestionList> suggestionList;
    
    void showSuggestions(const StringArray& suggestions);
    void hideSuggestions();
    StringArray filterSuggestions(const String& input);
    void validateText();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AutoCompleteTextEditor)
};

/**
 * Popup component for editing notes text
 */
class NotesEditorPopup : public Component,
                         public TextEditor::Listener
{
public:
    NotesEditorPopup(const String& initialText, std::function<void(const String&)> onTextChanged);
    ~NotesEditorPopup();
    
    void paint(Graphics& g) override;
    void resized() override;
    
    // TextEditor::Listener
    void textEditorReturnKeyPressed(TextEditor& editor) override;
    void textEditorEscapeKeyPressed(TextEditor& editor) override;
    void textEditorFocusLost(TextEditor& editor) override;
    
private:
    std::unique_ptr<TextEditor> textEditor;
    std::function<void(const String&)> onTextChangedCallback;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NotesEditorPopup)
};

/**
*
    Draws data in real time

*/
class EphysMetadataGeneratorCanvas : public Visualizer,
                                     public TableListBoxModel,
                                     public ComboBox::Listener,
                                     public Label::Listener,
                                     public Button::Listener
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

    /** Generates an assertion if this class leaks */
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EphysMetadataGeneratorCanvas);
};

#endif // EPHYSMETADATAGENERATORCANVAS_H_INCLUDED
