//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "NewParticleKernel.h"
#include "PICRayStudy.h"

#include "ClaimRays.h"

registerMooseObject("StarlingApp", NewParticleKernel);

InputParameters
NewParticleKernel::validParams()
{
  auto params = GeneralRayKernel::validParams();
  params.addClassDescription("A RayKernel that creates a new particle every time.");
  return params;
}

NewParticleKernel::NewParticleKernel(const InputParameters & params) : GeneralRayKernel(params) {}

void
NewParticleKernel::preTrace()
{
  // std::cout << currentRay()->currentPoint() << std::endl;
  // std::cout << currentRay()->getInfo() << std::endl;
  // casting to get the data associated with this study
  PICRayStudy * picStudy = dynamic_cast<PICRayStudy *>(&_study);
  // do nothing if it's not a PIC study
  if (!picStudy)
    return;

  if (!currentRay()->data()[picStudy->_new_particle_index])
  {
    if (_fe_problem.time() == 5)
    {
      for (int i = 0; i < 2; ++i)
      {
        const Point some_point = currentRay()->currentPoint(); // must be within _current_elem!
        Point some_direction;
        if (i == 0)
        {
          some_direction = Point(1, 1, 0);
        }
        else
        {
          some_direction = Point(1, -1, 0);
        }
        std::shared_ptr<Ray> ray = acquireRay(some_point, some_direction);
        ray->data()[picStudy->_v_x_index] = some_direction(0);
        ray->data()[picStudy->_v_y_index] = some_direction(1);
        // set this true so this ray cannot make other rays
        ray->data()[picStudy->_new_particle_index] = true;
        // set this true so this ray cannot make another rays
        currentRay()->data()[picStudy->_new_particle_index] = true;
        // ray->setStartingMaxDistance(picStudy->maxDistance(*ray));
        ray->setShouldContinue(false);
        moveRayToBuffer(ray);
      }
    }
  }
}

void
NewParticleKernel::onSegment()
{
}
