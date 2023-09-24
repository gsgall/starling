//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ModifyTrajectoryKernel.h"

#include "PICRayStudy.h"

registerMooseObject("StarlingApp", ModifyTrajectoryKernel);

InputParameters
ModifyTrajectoryKernel::validParams()
{
  auto params = GeneralRayKernel::validParams();
  params.addClassDescription("A RayKernel that creates a new particle every time.");
  return params;
}

ModifyTrajectoryKernel::ModifyTrajectoryKernel(const InputParameters & params)
  : GeneralRayKernel(params)
{
}

void
ModifyTrajectoryKernel::preTrace()
{
  // if ( _fe_problem.)
  if (_fe_problem.timeStep() > 1)
  {
    auto dir = currentRay()->direction();
    dir(1) = -dir(1);
    currentRay()->changeDirection(dir);
  }
  // std::cout << _fe_problem.timeStep() << std::endl;
}

void
ModifyTrajectoryKernel::onSegment()
{
}
