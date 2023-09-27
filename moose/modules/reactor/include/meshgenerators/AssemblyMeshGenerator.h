//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "ReactorGeometryMeshBuilderBase.h"

/**
 * Mesh generator for defining a reactor assembly using a Cartesian or hexagonal lattice with the
 * option to be 2-D or 3-D.
 */
class AssemblyMeshGenerator : public ReactorGeometryMeshBuilderBase
{
public:
  static InputParameters validParams();

  AssemblyMeshGenerator(const InputParameters & parameters);

  std::unique_ptr<MeshBase> generate() override;

protected:
  // Define metadata associated with AssemblyMeshGenerator
  void generateMetadata();

  ///The names of the pins that compose the Assembly
  const std::vector<MeshGeneratorName> _inputs;

  ///The id number for the type of the assembly
  const subdomain_id_type _assembly_type;

  ///The 2D pin-by-pin layout of the assembly mapping indices into _inputs
  const std::vector<std::vector<unsigned int>> _pattern;

  ///The inner apothem of any ducts around the assembly
  const std::vector<Real> _duct_sizes;

  ///The number of divisions in the mesh outside of the pins and inside of the ducts
  const unsigned int _background_intervals;

  ///The number of divisions in the meshes of the ducts
  const std::vector<unsigned int> _duct_intervals;

  ///Vector used to set the "region_id" extra-element integer of the assembly background elements
  std::vector<subdomain_id_type> _background_region_id;

  ///2-D vector (axial outer indexing, radial inner indexing) used to set the "region_id" extra-element integer of the assembly duct elements
  std::vector<std::vector<subdomain_id_type>> _duct_region_ids;

  ///Whether block names for assembly background elements have been provided by user
  bool _has_background_block_name;

  ///Whether block names for assembly duct elements have been provided by user
  bool _has_duct_block_names;

  ///Optional vector used to set the block names of the assembly background elements
  std::vector<std::string> _background_block_name;

  ///Optional 2-D vector (axial outer indexing, radial inner indexing) used to set the block names of the assembly duct elements
  std::vector<std::vector<std::string>> _duct_block_names;

  ///Whether this mesh should be extruded to 3-D, making it the final structure in the reactor mesh
  const bool _extrude;

  ///The type of geometry that is being described (Square or Hex, declared in the ReactorMeshParams object)
  std::string _geom_type;

  ///The number of dimensions the mesh is ultimately going to have (2 or 3, declared in the ReactorMeshParams object)
  int _mesh_dimensions;

  ///The heights of the axial regions that are stored on the ReactorMeshParams object
  std::vector<Real> _axial_boundaries;

  ///The region ids of peripheral regions in the 2D mesh. This includes the assembly background region and ducts
  std::vector<subdomain_id_type> _peripheral_region_ids;

  ///A mapping from pin-type IDs to region IDs used when assigning region IDs during the pin stitching stage
  std::map<subdomain_id_type, std::vector<std::vector<subdomain_id_type>>> _pin_region_id_map;

  ///A mapping from pin-type IDs to block names used when assigning block names during the pin stitching stage
  std::map<subdomain_id_type, std::vector<std::vector<std::string>>> _pin_block_name_map;

  ///The ID of the assembly outer boundary, equal to the assembly type ID + 2000
  boundary_id_type _assembly_boundary_id;

  ///The name of the assembly outer boundary, equal to the concatenation of "outer_assembly_" and the assembly-type ID
  std::string _assembly_boundary_name;

  /// The final mesh that is generated by the subgenerators;
  /// This mesh is generated by the subgenerators with only element and boundary IDs changed.
  std::unique_ptr<MeshBase> * _build_mesh;
};
