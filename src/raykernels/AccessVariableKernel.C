//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "AccessVariableKernel.h"
// MOOSE includes
#include "Function.h"
#include "MooseMesh.h"
#include "MooseVariable.h"
#include "SubProblem.h"

#include "libmesh/system.h"

registerMooseObject("RayTracingApp", AccessVariableKernel);

InputParameters
AccessVariableKernel::validParams()
{
  InputParameters params = GeneralRayKernel::validParams();

  params.addClassDescription("Compute the value of a variable at a specified location.");
  params.addRequiredParam<VariableName>(
      "variable", "The name of the variable that this postprocessor operates on.");
  params.addClassDescription("Compute the value of a variable at a specified location");
  return params;
}

AccessVariableKernel::AccessVariableKernel(const InputParameters & params)
  : GeneralRayKernel(params),
    _var_number(_fe_problem
                    .getVariable(_tid,
                                 params.get<VariableName>("variable"),
                                 Moose::VarKindType::VAR_ANY,
                                 Moose::VarFieldType::VAR_FIELD_STANDARD)
                    .number()),
    _system(_fe_problem.getSystem(getParam<VariableName>("variable"))),
    _value(0)
{
}

void
AccessVariableKernel::preTrace()
{
  std::cout << _system.point_value(
                   _var_number, currentRay()->currentPoint(), currentRay()->currentElem())
            << std::endl;
}

void
AccessVariableKernel::onSegment()
{
}
