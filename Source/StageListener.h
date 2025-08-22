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

#ifndef STAGELISTENER_H_INCLUDED
#define STAGELISTENER_H_INCLUDED

#include <VisualizerWindowHeaders.h>

/**
 * Structure to hold stage position data
 */
struct StagePosition
{
    String x;
    String y;
    String z;
    Time timestamp;
    String serialNumber;
    
    StagePosition() = default;
    StagePosition(const String& serialNumber_, const String& xPos, const String& yPos, const String& zPos)
        : serialNumber(serialNumber_), x(xPos), y(yPos), z(zPos), timestamp(Time::getCurrentTime()) {}
};

/**
 * Callback interface for receiving stage position updates
 */
class StagePositionListener
{
public:
    virtual ~StagePositionListener() = default;
    
    /**
     * Called when new stage positions are received from the HTTP endpoint
     * @param positions Array of stage positions, one for each probe/row
     */
    virtual void stagePositionsUpdated(const Array<StagePosition>& positions) = 0;
    
    /**
     * Called when there's an error fetching stage positions
     * @param errorMessage Description of the error
     */
    virtual void stagePositionError(const String& errorMessage) = 0;
};

/**
 * Class that periodically fetches stage positions from an HTTP endpoint
 */
class StageListener : public Timer,
                      public Thread
{
public:
    /**
     * Constructor
     * @param listener Callback object to receive position updates
     * @param endpoint HTTP endpoint URL (default: "http://localhost:8080/stage/positions")
     * @param intervalMs Update interval in milliseconds (default: 10000 = 10 seconds)
     */
    StageListener(StagePositionListener* listener, 
                  const String& endpoint = "http://10.128.49.150:8080/stage/positions",
                  int intervalMs = 2000);
    
    /**
     * Destructor
     */
    ~StageListener();
    
    /**
     * Start listening for stage position updates
     */
    void startListening();
    
    /**
     * Stop listening for stage position updates
     */
    void stopListening();
    
    /**
     * Set the HTTP endpoint URL
     * @param newEndpoint New endpoint URL
     */
    void setEndpoint(const String& newEndpoint);
    
    /**
     * Set the update interval
     * @param intervalMs New interval in milliseconds
     */
    void setUpdateInterval(int intervalMs);
    
    /**
     * Check if the listener is currently active
     * @return true if listening, false otherwise
     */
    bool isListening() const;

private:
    // Timer callback - triggered every interval to start a new request
    void timerCallback() override;
    
    // Thread run method - performs the actual HTTP request
    void run() override;
    
    /**
     * Parse JSON response and extract stage positions
     * @param jsonString Raw JSON response from server
     * @return Array of parsed stage positions
     */
    Array<StagePosition> parseStagePositions(const String& jsonString);
    
    /**
     * Perform HTTP request to fetch stage positions
     * @return Raw response string from server, or empty string on error
     */
    String fetchStagePositions();
    
    StagePositionListener* positionListener;
    String httpEndpoint;
    int updateIntervalMs;
    bool listening;
    
    // Thread safety
    CriticalSection requestLock;
    std::atomic<bool> shouldMakeRequest;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StageListener)
};

#endif // STAGELISTENER_H_INCLUDED
