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

#ifndef TABLECOMPONENTS_H_INCLUDED
#define TABLECOMPONENTS_H_INCLUDED

#include <VisualizerWindowHeaders.h>

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
    JUCE_DECLARE_WEAK_REFERENCEABLE(AutoCompleteTextEditor)
    AutoCompleteTextEditor(const StringArray& suggestions);
    ~AutoCompleteTextEditor();
    
    void textEditorTextChanged(TextEditor& editor) override;
    bool keyPressed(const KeyPress& key) override;
    void focusLost(FocusChangeType cause) override;
    
    void setSuggestions(const StringArray& newSuggestions);

    void updateColours();
    
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

#endif // TABLECOMPONENTS_H_INCLUDED
