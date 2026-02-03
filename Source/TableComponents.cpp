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

#include "TableComponents.h"

// NotesEditorPopup Implementation
NotesEditorPopup::NotesEditorPopup(const String& initialText, std::function<void(const String&)> onTextChanged)
    : onTextChangedCallback(onTextChanged)
{
    textEditor = std::make_unique<TextEditor>();
    textEditor->setMultiLine(true, false);
    textEditor->setScrollBarThickness (10);
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
    g.fillAll(getLookAndFeel().findColour(ThemeColours::componentParentBackground));
    g.setColour(findColour(ThemeColours::outline));
    g.drawRect(getLocalBounds(), 2);
    
    // Draw title
    g.setColour(getLookAndFeel().findColour(ThemeColours::controlPanelText));
    g.setFont(FontOptions ("Inter", "Semi Bold", 15.0f));
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
    listBox->setMouseMoveSelectsRows (true);
    listBox->setOutlineThickness(1);
    listBox->setColour(ListBox::outlineColourId, findColour(ThemeColours::outline));
    listBox->setColour(ListBox::backgroundColourId, findColour(ThemeColours::menuBackground));
    addAndMakeVisible(listBox.get());
    
    setSize(200, jmin(100, suggestions.size() * 20 + 2));
}

AutoCompleteSuggestionList::~AutoCompleteSuggestionList()
{
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
        g.setColour(findColour(ThemeColours::menuHighlightBackground));
        g.fillRect(0, 0, width, height);
    }
    
    g.setColour(findColour(rowIsSelected ? ThemeColours::menuHighlightText : ThemeColours::defaultText));
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
    
    // Set initial colors to match the theme
    updateColours();
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
    WeakReference<AutoCompleteTextEditor> safeThis(this);
    Timer::callAfterDelay(150, [safeThis]() {
        // Check if component still exists before accessing it
        if (safeThis == nullptr)
            return;
            
        // Check if the suggestion list itself has focus or if we got focus back
        if (safeThis->hasKeyboardFocus(false) || (safeThis->suggestionList && safeThis->suggestionList->hasKeyboardFocus(true)))
        {
            return; // Don't hide if we or the suggestion list has focus
        }
        safeThis->hideSuggestions();
        
        // Validate the current text against available suggestions
        safeThis->validateText();
    });
    
    TextEditor::focusLost(cause);
}

void AutoCompleteTextEditor::setSuggestions(const StringArray& newSuggestions)
{
    allSuggestions = newSuggestions;
}

void AutoCompleteTextEditor::updateColours()
{
    // Update the TextEditor colors to match the current theme
    applyColourToAllText (findColour(ThemeColours::defaultText));

    setColour(TextEditor::backgroundColourId, Colours::white.withAlpha(0.0f));

    // Repaint to apply the new colors
    repaint();
}

void AutoCompleteTextEditor::showSuggestions(const StringArray& suggestions)
{
    // Create a WeakReference to check validity throughout this method
    WeakReference<AutoCompleteTextEditor> safeThis(this);
    if (safeThis == nullptr)
        return;
        
    if (suggestionList == nullptr)
    {
        suggestionList = std::make_unique<AutoCompleteSuggestionList>(suggestions, 
            [this](const String& selection)
            {
                // Use WeakReference to safely check if component still exists
                WeakReference<AutoCompleteTextEditor> safeThis(this);
                if (safeThis != nullptr)
                {
                    setText(selection);
                    hideSuggestions();
                    // Only grab focus if component is still valid and visible
                    if (safeThis != nullptr && safeThis->isShowing())
                        safeThis->grabKeyboardFocus();
                }
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
        
        // Ensure the TextEditor maintains keyboard focus - only if still showing
        if (safeThis != nullptr && safeThis->isShowing())
            safeThis->grabKeyboardFocus();
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
