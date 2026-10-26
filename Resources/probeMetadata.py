import numpy as np

import aind_data_schema.components.coordinates as coordinates
from aind_data_schema.components.configs import EphysAssemblyConfig, ProbeConfig, MISModuleConfig, ManipulatorConfig
from aind_data_schema_models.brain_atlas import CCFv3

"""Five transformations define the position and orientation of the probe in the global coordinate space:

Rotation 1: places the probe's local axes in the default position (left-most shank aligned with SI axis, electrodes facing mouse left).
Rotation 2: probe spin (Z-rotation only). 0 degrees = electrodes facing mouse left, 90 degrees = electrodes facing posterior, 180 degrees = electrodes facing mouse right, 270 degrees = electrodes facing anterior.
Translation 1: manipulator coordinates in mm. The fourth value should always be 1 to create a valid transformation matrix.
Rotation 3: Euler angles in degrees defining the probe's orientation.
Translation 2: measured offset in mm required to place probe tip in the global position. The fourth (depth) coordinate tracks the distance the probe tip advanced along the insertion axis from the brain surface, and is independent of the global coordinate system.

To find the global position of the probe tip, create a 4 x 4 transformation matrix from Rotation 3 and Translation 2, and apply it to Translation 1.
"""
### Probe and manipulator metadata

probe_name = "ProbeA"

# conventions:
# arc_angle: positive anterior; do not include 14 degree mouse tilt
# module_angle: positive right
# manipulator coordinates: X = +right, Y = +posterior, Z = +inferior

arc_angle = -10
module_angle = 20
probe_tilt = arc_angle - 14 # account for mouse head angle (14 degrees toward anterior)
spin_angle = -90
surface_Z = 5.0
final_Z = 10.0

manipulator_coords_at_bregma = [5000,6000,2000]
manipulator_coords_at_surface = [6000,7000, surface_Z]
targeted_region = "CP"

####### Our coordinate systems

global_coordinate_system = coordinates.CoordinateSystem(
    name = "BREGMA_RAS",
    origin = coordinates.Origin.BREGMA,
    axis_unit=coordinates.SizeUnit.MM,
    axes = [
        coordinates.Axis(name = coordinates.AxisName.ML, direction=coordinates.Direction.LR),
        coordinates.Axis(name = coordinates.AxisName.AP, direction=coordinates.Direction.PA),
        coordinates.Axis(name = coordinates.AxisName.SI, direction=coordinates.Direction.IS)
    ]
)

probe_coordinate_system = coordinates.CoordinateSystem(
    name = "PROBE_RUF",
    origin=coordinates.Origin.TIP,
    axis_unit=coordinates.SizeUnit.MM,
    axes = [
        coordinates.Axis(name = coordinates.AxisName.X, direction=coordinates.Direction.LR),
        coordinates.Axis(name = coordinates.AxisName.Y, direction=coordinates.Direction.DU),
        coordinates.Axis(name = coordinates.AxisName.Z, direction=coordinates.Direction.BF)
    ]
)

mis_manip_coordinate_system = coordinates.CoordinateSystem(
    name="MIS_MANIP_RPI",
    origin=coordinates.Origin.ORIGIN,
    axis_unit=coordinates.SizeUnit.MM,
    axes=[
        coordinates.Axis(name=coordinates.AxisName.X, direction=coordinates.Direction.LR),
        coordinates.Axis(name=coordinates.AxisName.Y, direction=coordinates.Direction.AP),
        coordinates.Axis(name=coordinates.AxisName.Z, direction=coordinates.Direction.SI),
    ],
)


#######

manipulator_offset = np.array(manipulator_coords_at_surface) - np.array(manipulator_coords_at_bregma)

config = EphysAssemblyConfig(

    device_name = "Ephys Assembly " + probe_name,

    manipulator = ManipulatorConfig(
        device_name = "Manipulator " + probe_name, 
        coordinate_system = mis_manip_coordinate_system,
        local_axis_positions = coordinates.Translation(
                translation=manipulator_coords_at_surface
        )
    ),

    probes = [ProbeConfig(
        device_name = probe_name,
        notes = "insert notes here",
        dye = "DiI",
        primary_targeted_structure = CCFv3.by_acronym(targeted_region),
        coordinate_system=probe_coordinate_system,
        transform = [
            # The following transforms move the probe coordinates into the global coordinate system:
            # First rotation sets default position, same for every experiment
            coordinates.Rotation(angles = [90.0, 0.0, -90.0], 
                                    angles_unit=coordinates.AngleUnit.DEG,
                                    reference_coordinate_system=coordinates.ReferenceCoordinateSystem.GLOBAL), 

            # Second rotation is for the arc angle (rotation around global X)
            coordinates.Rotation(angles = [probe_tilt, 0, 0], 
                                    angles_unit=coordinates.AngleUnit.DEG, # Euler angles in degrees
                                    reference_coordinate_system=coordinates.ReferenceCoordinateSystem.GLOBAL,
                                    rotation_direction=coordinates.RotationDirection.LEFT_HAND),

            # Third rotation is for the module angle, rotation around local X
            coordinates.Rotation(angles = [module_angle, 0, 0], angles_unit=coordinates.AngleUnit.DEG, 
                                                                reference_coordinate_system=coordinates.ReferenceCoordinateSystem.LOCAL,
                                                                rotation_direction=coordinates.RotationDirection.LEFT_HAND),

            # Fourth rotation is for spinning the probe along its insertion axis (local Y); CCW = positive
            coordinates.Rotation(angles = [0, spin_angle, 0], angles_unit=coordinates.AngleUnit.DEG, 
                                    reference_coordinate_system=coordinates.ReferenceCoordinateSystem.LOCAL,
                                    rotation_direction=coordinates.RotationDirection.LEFT_HAND), 

            # First translation is for the location of the probe at the brain surface
            coordinates.Translation(translation=[manipulator_offset[1], -manipulator_offset[2], -manipulator_offset[0]],
                                    reference_coordinate_system=coordinates.ReferenceCoordinateSystem.LOCAL), 
                    
            # Second translation is for the final recording location
            coordinates.Translation(translation=[0, -(final_Z - surface_Z), 0],
                                    reference_coordinate_system=coordinates.ReferenceCoordinateSystem.LOCAL), 
            ]
    )],
    
    modules = [MISModuleConfig(
        arc_angle = arc_angle,
        module_angle = module_angle,
        angle_unit = "degrees"
    )]
)

with open("probe_metadata.json", "w", encoding="utf-8") as f:
    f.write(config.model_dump_json(indent=2))
