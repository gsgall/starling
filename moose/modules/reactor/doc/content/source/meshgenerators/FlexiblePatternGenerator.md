# FlexiblePatternGenerator

!syntax description /Mesh/FlexiblePatternGenerator

## Overview

`FlexiblePatternGenerator` enhances the capabilities provided in [`PatternedHexMeshGenerator`](PatternedHexMeshGenerator.md) and [`PatternedCartesianMeshGenerator`](PatternedCartesianMeshGenerator.md) by providing more flexibility in both patterning and background shape. It can be used when the background shape is not a hexagon or square and/or when the pin pattern does not follow a hexagonal or cartesian grid.

## Background Shape

Instead of the fixed hexagonal and cartesian background shapes provided respectively by `PatternedHexMeshGenerator` and `PatternedCartesianMeshGenerator`, `FlexiblePatternGenerator` allows adoption of a closed curve of any type, provided by [!param](/Mesh/FlexiblePatternGenerator/boundary_mesh), to be used as the external boundary of the background region, with [!param](/Mesh/FlexiblePatternGenerator/boundary_type) set as `CUSTOM`. The [!param](/Mesh/FlexiblePatternGenerator/boundary_mesh) needs to be a mesh containing a single closed curve that resides in the XY plane and consists of only `EDGE2` elements. Examples of such curve meshes include polygon curves generated by [`PolyLineMeshGenerator`](PolyLineMeshGenerator.md), more generalized curves generated by [`ParsedCurveGenerator`](ParsedCurveGenerator.md), and even externally generated curves through [`FileMeshGenerator`](FileMeshGenerator.md).

If a hexagonal, Cartesian, or circular external boundary are desired, [!param](/Mesh/FlexiblePatternGenerator/boundary_type) should be set as as `HEXAGON`, `CARTESIAN`, and `CIRCLE`, respectively. In that case, [!param](/Mesh/FlexiblePatternGenerator/boundary_size) and [!param](/Mesh/FlexiblePatternGenerator/boundary_sectors) also need to be specified. By default, the external boundary will be assigned an ID of `10000` to be compatible with other Reactor Module tools. Users can also define a custom id for the external boundary using [!param](/Mesh/FlexiblePatternGenerator/external_boundary_id).

The background region is meshed by [`XYDelaunayGenerator`](XYDelaunayGenerator.md) through MOOSE's sub-meshgenerator capability. Some key parameters of `XYDelaunayGenerator` (e.g., [!param](/Mesh/FlexiblePatternGenerator/desired_area), [!param](/Mesh/FlexiblePatternGenerator/desired_area_func), [!param](/Mesh/FlexiblePatternGenerator/verify_holes)) can be set here and then directly passed to `XYDelaunayGenerator`.

## Patterning

Within the interior region defined by [!param](/Mesh/FlexiblePatternGenerator/boundary_mesh), `FlexiblePatternGenerator` can place single or multiple patterns of input unit meshes provided by [!param](/Mesh/FlexiblePatternGenerator/inputs). Supporting patterning mode includes hexagonal, rectangular, and circular geometry. In addition, free-style patterning is supported by directly providing positions of unit meshes. Styles of patterns can be combined as well when using multiple patterns.
The details are discussed as follows.

### Hexagonal Patterning

One or several hexagonal patterns of input unit meshes can be generated. These patterns are provided by a triple-indexed input parameter [!param](/Mesh/FlexiblePatternGenerator/hex_patterns). Each element of [!param](/Mesh/FlexiblePatternGenerator/hex_patterns) follows the same format as the [!param](/Mesh/PatternedHexMeshGenerator/pattern) parameter in [`PatternedHexMeshGenerator`](PatternedHexMeshGenerator.md). The origin position of each pattern is defined through [!param](/Mesh/FlexiblePatternGenerator/hex_origins), while the patterning pitches are provided by [!param](/Mesh/FlexiblePatternGenerator/hex_pitches). Each hexagonal pattern can be rotated around its own origin independently, as directed by [!param](/Mesh/FlexiblePatternGenerator/hex_rotations). Here, a `zero` rotation angle leads to a hexagonal pattern with its flat side parallel to x-axis (so that patterns in [!param](/Mesh/FlexiblePatternGenerator/hex_patterns) are intuitive).

!listing modules/reactor/test/tests/meshgenerators/flexible_pattern_generator/double_hex_pattern.i
         block=Mesh/fpg
         id=hex_pattern
         caption=Input example of hexagonal patterning.

!media reactor/meshgenerators/double_hex_pattern.png
      style=display: block;margin-left:auto;margin-right:auto;width:40%;
      id=double_hex_pattern
      caption=Output example of hexagonal patterning.

### Rectangular Patterning 

One or several rectangular patterns of input unit meshes can be generated. These patterns are provided by a triple-indexed input parameter [!param](/Mesh/FlexiblePatternGenerator/rect_patterns). Each element of [!param](/Mesh/FlexiblePatternGenerator/rect_patterns) follows the same format as the [!param](/Mesh/PatternedCartesianMeshGenerator/pattern) parameter in [`PatternedCartesianMeshGenerator`](PatternedCartesianMeshGenerator.md) but allows general rectangular patterning instead of restricted square patterning. The origin position of each pattern is defined through [!param](/Mesh/FlexiblePatternGenerator/rect_origins), while the patterning pitches in both directions are provided by [!param](/Mesh/FlexiblePatternGenerator/rect_pitches_x) and [!param](/Mesh/FlexiblePatternGenerator/rect_pitches_y), respectively. Each rectangular pattern can be rotated around its own origin independently, as directed by [!param](/Mesh/FlexiblePatternGenerator/rect_rotations).

!listing modules/reactor/test/tests/meshgenerators/flexible_pattern_generator/single_rect_pattern.i
         block=Mesh/fpg
         id=rect_pattern
         caption=Input example of rectangular patterning.

!media reactor/meshgenerators/single_rect_pattern.png
      style=display: block;margin-left:auto;margin-right:auto;width:40%;
      id=single_rect_pattern
      caption=Output example of rectangular patterning.

### Circular Patterning

One or several circular patterns of input unit meshes can be generated. The patterns are provided by a double-indexed input parameter [!param](/Mesh/FlexiblePatternGenerator/circular_patterns). Each element of [!param](/Mesh/FlexiblePatternGenerator/circular_patterns) lists all the unit meshes uniformly distributed on a circle. The origin position of each pattern is defined through [!param](/Mesh/FlexiblePatternGenerator/circular_origins), while the radius of each pattern is provided by [!param](/Mesh/FlexiblePatternGenerator/circular_radii), respectively. Each circular pattern can be rotated around its own origin independently, as directed by [!param](/Mesh/FlexiblePatternGenerator/circular_rotations). The first unit mesh will be positioned at the azimuthal angle defined as in [!param](/Mesh/FlexiblePatternGenerator/circular_rotations), while the following unit meshes will be positioned uniformly on the circle counterclockwise.

!listing modules/reactor/test/tests/meshgenerators/flexible_pattern_generator/single_circ_pattern.i
         block=Mesh/fpg
         id=circ_pattern
         caption=Input example of circular patterning.

!media reactor/meshgenerators/single_circ_pattern.png
      style=display: block;margin-left:auto;margin-right:auto;width:40%;
      id=single_circ_pattern
      caption=Output example of circular patterning.

### Custom Patterning

In addition to the hexagonal, rectangular and circular patterning, users can specify the positions of unit meshes freely using [!param](/Mesh/FlexiblePatternGenerator/extra_positions). Corresponding to each position provided in [!param](/Mesh/FlexiblePatternGenerator/extra_positions), an input unit mesh index needs to be provided in [!param](/Mesh/FlexiblePatternGenerator/extra_positions_mg_indices).

!listing modules/reactor/test/tests/meshgenerators/flexible_pattern_generator/custom_pattern.i
         block=Mesh/fpg
         id=custom_pattern
         caption=Input example of custom patterning.

!media reactor/meshgenerators/custom_pattern.png
      style=display: block;margin-left:auto;margin-right:auto;width:40%;
      id=custom_pattern_m
      caption=Output example of custom patterning.

### Dummy Unit Meshes

In both [!param](/Mesh/FlexiblePatternGenerator/hex_patterns) and [!param](/Mesh/FlexiblePatternGenerator/rect_patterns), the use of an input unit mesh index greater than or equal to the size of [!param](/Mesh/FlexiblePatternGenerator/inputs) indicates that patterning position is a dummy and no input unit mesh will be assigned to that position. This feature allows creation of partial patterning.

!listing modules/reactor/test/tests/meshgenerators/flexible_pattern_generator/mixed_pattern.i
         block=Mesh/fpg
         id=mixed_pattern
         caption=Input example of mixed patterning with dummy unit meshes.

!media reactor/meshgenerators/mixed_pattern.png
      style=display: block;margin-left:auto;margin-right:auto;width:40%;
      id=mixed_pattern_m
      caption=Output example of mixed patterning with dummy unit meshes.

!syntax parameters /Mesh/FlexiblePatternGenerator

!syntax inputs /Mesh/FlexiblePatternGenerator

!syntax children /Mesh/FlexiblePatternGenerator