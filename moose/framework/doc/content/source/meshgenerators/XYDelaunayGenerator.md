# XYDelaunayGenerator

!syntax description /Mesh/XYDelaunayGenerator

An input mesh will be used to define the outer boundary of the
generated output mesh, as well as to specify any Steiner points
interior to that boundary which must be included in as output mesh
vertices.  Additional interior vertices can be automatically generated
by specifying a desired triangle area (either as a constant or as a
function of triangle center location).  Additional interior boundary
"holes" can be generated from inputs from other specified "holes" mesh
generators.

Each input mesh, as specified in the
[!param](/Mesh/XYDelaunayGenerator/boundary) parameter and optionally
in the [!param](/Mesh/XYDelaunayGenerator/holes) parameter,
can define a ring of boundary segments either by including edge
elements for each segment or by including face elements whose external
edges are to be treated as segments.  For the outer
[!param](/Mesh/XYDelaunayGenerator/boundary)
mesh, a subset of edges or faces can be examined instead of
the entire mesh, using the
[!param](/Mesh/XYDelaunayGenerator/input_subdomain_names) parameter to
list edge element subdomains or the
[!param](/Mesh/XYDelaunayGenerator/input_boundary_names) parameter to
list face boundaries.

If multiple boundary rings exist in a mesh, only the ring enclosing
the rest of the mesh is considered to be "the" boundary.  Input meshes
which are not simply connected, which therefore have multiple outer
boundary rings, are not yet supported.

If the input boundary mesh has nodes inside the boundary ring, these
nodes are treated as required in the output mesh.  This is useful for
ensuring that the output mesh has a vertex at a specific point of
interest.  For meshes with interior nodes that should not be
considered required, a user can first use a
[LowerDBlockFromSidesetGenerator.md] to generate the desired boundary
ring and a [BlockDeletionGenerator.md] on that to remove everything
except the desired boundary edges.  Setting
[!param](/Mesh/BlockDeletionGenerator/delete_exteriors) to
false is necessary in this workflow to prevent the new edge elements
from being deleted along with their interior-parent elements.

Using stitching options, meshes used as "holes" can subsequently be
stitched into those portions of the output mesh.  For this use case
some care may be required: refinement of stitched hole boundaries
should be disallowed so that the boundary nodes in the newly
triangulated mesh still precisely match the boundary nodes in the hole
mesh.

Interior vertices can be adjusted after mesh generation using the [!param](/Mesh/XYDelaunayGenerator/smooth_triangulation) parameter, to produce a
more "smooth" mesh, but currently the only mesh smoother option is a
simple Laplacian smoother; this can have unwanted side-effects on
meshes with concave boundaries or poor nodal valences, and so it is
disabled by default for robustness.

## Examples

Using instances of the [PolyLineMeshGenerator.md] to create a boundary
and a few holes, followed by a `XYDelaunayGenerator` object to
triangulate the region between them, the [Mesh](/Mesh/index.md) block
shown in the input file snippet below generates the final mesh shown
in [Figure 1](#withholes).

For this example a specified fixed interpolation of boundary edges is
used, but refinement to a desired maximum triangle size allows
automatic placement of nodes in the mesh interior.

!listing test/tests/meshgenerators/xy_delaunay_generator/xydelaunay_with_holes.i block=Mesh

!media framework/meshgenerators/poly2tri_with_holes.png
      style=width:32%;
      id=withholes
      caption=Fig. 1: Resulting triangulated mesh from a polyline boundary and holes.

In the input file snippet below, hole stitching is done recursively,
so that each internal "boundary" polyline (after refinement) remains
preserved in the final mesh shown in [Figure 2](#nested).

!listing test/tests/meshgenerators/xy_delaunay_generator/xydelaunay_nested.i block=Mesh

!media framework/meshgenerators/poly2tri_nested.png
      style=width:32%;
      id=nested
      caption=Fig. 2: Resulting triangulated mesh with nested polyline boundaries and an internal grid. 

!syntax parameters /Mesh/XYDelaunayGenerator

!syntax inputs /Mesh/XYDelaunayGenerator