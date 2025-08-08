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

#include "StageListener.h"

StageListener::StageListener(StagePositionListener* listener, const String& endpoint, int intervalMs)
    : Thread("StageListener"),
      positionListener(listener),
      httpEndpoint(endpoint),
      updateIntervalMs(intervalMs),
      listening(false),
      shouldMakeRequest(false)
{
    jassert(listener != nullptr);
}

StageListener::~StageListener()
{
    stopListening();
}

void StageListener::startListening()
{
    if (listening)
        return;
    
    listening = true;
    shouldMakeRequest = false;
    
    // Start the timer to trigger requests every interval
    startTimer(updateIntervalMs);
    
    LOGD("StageListener started with endpoint: ", httpEndpoint, " and interval: ", updateIntervalMs, "ms");
}

void StageListener::stopListening()
{
    if (!listening)
        return;
    
    listening = false;
    shouldMakeRequest = false;
    
    // Stop the timer
    stopTimer();
    
    // Stop and wait for the thread to finish
    if (isThreadRunning())
    {
        signalThreadShouldExit();
        waitForThreadToExit(5000); // Wait up to 5 seconds
    }
    
    LOGD("StageListener stopped");
}

void StageListener::setEndpoint(const String& newEndpoint)
{
    ScopedLock lock(requestLock);
    httpEndpoint = newEndpoint;
    LOGD("StageListener endpoint changed to: ", httpEndpoint);
}

void StageListener::setUpdateInterval(int intervalMs)
{
    updateIntervalMs = intervalMs;
    
    if (listening)
    {
        // Restart timer with new interval
        stopTimer();
        startTimer(updateIntervalMs);
    }
    
    LOGD("StageListener update interval changed to: ", updateIntervalMs, "ms");
}

bool StageListener::isListening() const
{
    return listening;
}

void StageListener::timerCallback()
{
    if (!listening)
        return;
    
    // Don't start a new request if one is already in progress
    if (isThreadRunning())
    {
        LOGD("StageListener: Previous request still in progress, skipping...");
        return;
    }
    
    // Signal the thread to make a request
    shouldMakeRequest = true;
    startThread();
}

void StageListener::run()
{
    if (!shouldMakeRequest.load() || !listening)
        return;
    
    // Reset the flag
    shouldMakeRequest = false;
    
    try
    {
        String response = fetchStagePositions();
        
        if (threadShouldExit())
            return;
        
        if (response.isNotEmpty())
        {
            Array<StagePosition> positions = parseStagePositions(response);
            
            if (threadShouldExit())
                return;
            
            // Notify listener on the message thread
            MessageManager::callAsync([this, positions]()
            {
                if (positionListener != nullptr && listening)
                {
                    positionListener->stagePositionsUpdated(positions);
                }
            });
        }
        else
        {
            // Notify listener of error on the message thread
            MessageManager::callAsync([this]()
            {
                if (positionListener != nullptr && listening)
                {
                    positionListener->stagePositionError("Failed to fetch data from server");
                }
            });
        }
    }
    catch (const std::exception& e)
    {
        // Notify listener of error on the message thread
        String errorMsg = "Exception during stage position fetch: " + String(e.what());
        MessageManager::callAsync([this, errorMsg]()
        {
            if (positionListener != nullptr && listening)
            {
                positionListener->stagePositionError(errorMsg);
            }
        });
    }
}

String StageListener::fetchStagePositions()
{
    ScopedLock lock(requestLock);
    
    if (threadShouldExit())
        return {};
    
    try
    {
        URL url(httpEndpoint);
        if (!url.isWellFormed())
        {
            LOGE("StageListener: Invalid URL: ", httpEndpoint);
            return {};
        }
        
        // Create HTTP request with timeout
        std::unique_ptr<InputStream> inputStream(url.createInputStream(
            URL::InputStreamOptions(URL::ParameterHandling::inAddress)
                .withConnectionTimeoutMs(5000)  // 5 second connection timeout
                .withResponseHeaders(nullptr)
                .withExtraHeaders("Accept: application/json")
                .withHttpRequestCmd("GET")));
        
        if (inputStream == nullptr)
        {
            LOGE("StageListener: Failed to create input stream for URL: ", httpEndpoint);
            return {};
        }
        
        // Read the response
        String response = inputStream->readEntireStreamAsString();
        
        if (response.isEmpty())
        {
            LOGC("StageListener: Empty response from server");
            return {};
        }
        
        LOGD("StageListener: Received response (", response.length(), " chars): ", response.substring(0, 100), "...");
        return response;
    }
    catch (const std::exception& e)
    {
        LOGE("StageListener: Exception during HTTP request: ", e.what());
        return {};
    }
}

Array<StagePosition> StageListener::parseStagePositions(const String& jsonString)
{
    Array<StagePosition> positions;
    
    try
    {
        var json = JSON::parse(jsonString);
        
        if (json.isUndefined())
        {
            LOGE("StageListener: Failed to parse JSON response");
            return positions;
        }
        
        // Look for different possible JSON structures
        // Structure 1: Direct array [{"x": 1.0, "y": 2.0, "z": 3.0}, ...]
        if (json.isArray())
        {
            for (int i = 0; i < json.size(); ++i)
            {
                var posObj = json[i];
                if (posObj.isObject())
                {
                    String x = posObj.getProperty("x", 0.0).toString();
                    String y = posObj.getProperty("y", 0.0).toString();
                    String z = posObj.getProperty("z", 0.0).toString();
                    positions.add(StagePosition(x, y, z));
                }
            }
        }
        // Structure 2: Object with nested structures
        else if (json.isObject())
        {
            var jsonObject = json.getDynamicObject();
            if (jsonObject != nullptr)
            {
                // Structure 2a: { "positions": [{"x": 1.0, "y": 2.0, "z": 3.0}, ...] }
                if (jsonObject.hasProperty("positions"))
                {
                    var positionsArray = jsonObject.getProperty("positions", var());
                    if (positionsArray.isArray())
                    {
                        for (int i = 0; i < positionsArray.size(); ++i)
                        {
                            var posObj = positionsArray[i];
                            if (posObj.isObject())
                            {
                                String x = posObj.getProperty("x", 0.0).toString();
                                String y = posObj.getProperty("y", 0.0).toString();
                                String z = posObj.getProperty("z", 0.0).toString();
                                positions.add(StagePosition(x, y, z));
                            }
                        }
                    }
                }
                // Structure 2b: { "probes": [{"probe_id": "A", "x": 1.0, "y": 2.0, "z": 3.0}, ...] }
                else if (jsonObject.hasProperty("probes"))
                {
                    var probesArray = jsonObject.getProperty("probes", var());
                    if (probesArray.isArray())
                    {
                        for (int i = 0; i < probesArray.size(); ++i)
                        {
                            var probeObj = probesArray[i];
                            if (probeObj.isObject())
                            {
                                String x = probeObj.getProperty("x", 0.0).toString();
                                String y = probeObj.getProperty("y", 0.0).toString();
                                String z = probeObj.getProperty("z", 0.0).toString();
                                positions.add(StagePosition(x, y, z));
                            }
                        }
                    }
                }
                // Structure 2c: Single position object {"x": 1.0, "y": 2.0, "z": 3.0}
                else if (jsonObject.hasProperty("x") || jsonObject.hasProperty("y") || jsonObject.hasProperty("z"))
                {
                    String x = jsonObject.getProperty("x", 0.0).toString();
                    String y = jsonObject.getProperty("y", 0.0).toString();
                    String z = jsonObject.getProperty("z", 0.0).toString();
                    positions.add(StagePosition(x, y, z));
                }
                else
                {
                    LOGC("StageListener: Unrecognized JSON object structure");
                }
            }
        }
        else
        {
            LOGC("StageListener: Unrecognized JSON structure (not array or object)");
        }
        
        LOGD("StageListener: Parsed ", positions.size(), " stage positions");
    }
    catch (const std::exception& e)
    {
        LOGE("StageListener: Exception parsing JSON: ", e.what());
    }
    
    return positions;
}
