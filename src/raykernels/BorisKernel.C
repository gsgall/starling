//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "BorisKernel.h"
// MOOSE includes
#include "Function.h"
#include "MooseMesh.h"
#include "MooseVariable.h"
#include "SubProblem.h"

#include "libmesh/system.h"

registerMooseObject("RayTracingApp", BorisKernel);

InputParameters
BorisKernel::validParams()
{
  InputParameters params = GeneralRayKernel::validParams();

  params.addClassDescription("Compute the value of a variable at a specified location.");
  params.addRequiredParam<std::vector<VariableName>>(
      "components", "The name of the vector components that act on the particles");
  params.addClassDescription("Compute a force on the particle due a vector");
  return params;
}

BorisKernel::BorisKernel(const InputParameters & params)
  : GeneralRayKernel(params),
    _components(getParam<std::vector<VariableName>>("components")),
    _var_numbers(_components.size(), 0),
    _system(_fe_problem.getSystem(_components[0])),
    _vectors(_components.size() / 3, libMesh::Point(0, 0, 0))
{
  if (_components.size() % 3 != 0)
  {
    paramError("components", "Each vector must have 3 components");
  }

  for (const auto i : index_range(_components))
  {
    _var_numbers[i] = _fe_problem
                          .getVariable(_tid,
                                       _components[i],
                                       Moose::VarKindType::VAR_ANY,
                                       Moose::VarFieldType::VAR_FIELD_STANDARD)
                          .number();
  }
}

void
BorisKernel::preTrace()
{
  getVectorValues();
}

void
BorisKernel::onSegment()
{
}

void
BorisKernel::getVectorValues()
{
  auto cur_point = currentRay()->currentPoint();
  auto element = currentRay()->currentElem();
  for (const auto i : index_range(_vectors))
  {
    _vectors[i](0) = _system.point_value(_var_numbers[i], cur_point, element);
    _vectors[i](1) = _system.point_value(_var_numbers[i + 1], cur_point, element);
    _vectors[i](2) = _system.point_value(_var_numbers[i + 2], cur_point, element);
  }
}
