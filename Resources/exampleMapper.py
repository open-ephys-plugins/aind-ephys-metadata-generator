import requests
import time
import json

from aind_data_schema.core.acquisition import (
    Acquisition,
    StimulusEpoch,
    DataStream,
    AcquisitionSubjectDetails,
)

from aind_data_schema.components.coordinates import (
    CoordinateSystemLibrary,
)

endpoint = "http://localhost:37497/api/"
processor_id = 101

print("Clearing history...")
response = requests.put(
    f"{endpoint}processors/{processor_id}/config",
    json={"text" : "clear history"}
)

print("Starting recording...")
requests.put(
        f"{endpoint}status",
        json={"mode": "RECORD"},
)

time.sleep(3)  # waits for 3 seconds

print("Stopping recording...")

requests.put(
        f"{endpoint}status",
        json={"mode": "IDLE"},
)

print("Receiving metadata...")

response = requests.put(
    f"{endpoint}processors/{processor_id}/config",
    json={"text" : "get metadata"}
)

metadata = json.loads(json.loads(response.text)['info'])
streams = [DataStream.model_validate(stream) for stream in metadata]

acquisition = Acquisition(
    experimenters=["John Smith"],
    subject_id="664484",
    acquisition_start_time=streams[0].stream_start_time,
    acquisition_end_time=streams[-1].stream_end_time,
    acquisition_type="Receptive field mapping",
    instrument_id="EPHYS1",
    ethics_review_id=["2109"],
    subject_details=AcquisitionSubjectDetails(
        mouse_platform_name="Running Wheel",
    ),
    coordinate_system=CoordinateSystemLibrary.BREGMA_RAS,
    stimulus_epochs =[ ],
    data_streams=streams
)

acquisition.write_standard_file(prefix="ephys")