//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "CollisionKernel.h"
// MOOSE includes
#include "Function.h"
#include "MooseMesh.h"
#include "MooseVariable.h"
#include "SubProblem.h"

#include "libmesh/system.h"

#include "CollisionRayStudy.h"

registerMooseObject("StarlingApp", CollisionKernel);

InputParameters
CollisionKernel::validParams()
{
  InputParameters params = GeneralRayKernel::validParams();

  params.addClassDescription("Check for collisions between particles in a single element");
  return params;
}

CollisionKernel::CollisionKernel(const InputParameters & params)
  : GeneralRayKernel(params), _col_study(dynamic_cast<CollisionRayStudy &>(_study))
{
}

void
CollisionKernel::postTrace()
{
  // if this element has not been added to the map yet we add it
  if (!(*_col_study._p_map).count(currentRay()->currentElem()))
  {
    std::vector<std::shared_ptr<Ray>> p(1, currentRay());
    (*_col_study._p_map)[currentRay()->currentElem()] = p;
    return;
  }
  else
  {
    // current particles in the same element that have not had a collision
    auto in_elem = (*_col_study._p_map)[currentRay()->currentElem()];

    for (auto it = in_elem.begin(); in_elem.size() != 0 && it != in_elem.end(); ++it)
    {
      auto ray = *it;

      auto dist = (currentRay()->currentPoint() - ray->currentPoint()).norm();
      // if here no collision has occured
      if (dist > 2 * _col_study._rad)
      {
        continue;
      }

      // if here we have a collision!
      auto rdiff = currentRay()->currentPoint() - ray->currentPoint();
      auto denom = dist * dist;

      auto v1 = Point(currentRay()->data()[_col_study._v_x_index],
                      currentRay()->data()[_col_study._v_y_index],
                      currentRay()->data()[_col_study._v_z_index]);

      auto v2 = Point(ray->data()[_col_study._v_x_index],
                      ray->data()[_col_study._v_y_index],
                      ray->data()[_col_study._v_z_index]);
      auto vdiff = v1 - v2;
      auto dv = (vdiff * rdiff / denom) * rdiff;
      auto v1_new = v1 - dv;
      auto v2_new = v2 + dv;

      updateVelocity(*currentRay(), v1_new);
      updateVelocity(*ray, v2_new);
      // remove the current velocity vector from the list
      in_elem.erase(it);
      // update the map with this change
      (*_col_study._p_map)[currentRay()->currentElem()] = in_elem;
      // if there is a collision we are done
      // std::cout << "We have a collision" << std::endl;
      return;
    }
    // if our particle had no collisions then we can add it to the list of uncollided particles
    (*_col_study._p_map)[currentRay()->currentElem()].push_back(currentRay());
  }
}

void
CollisionKernel::updateVelocity(Ray & ray, const Point v)
{
  ray.data()[_col_study._v_x_index] = v(0);
  ray.data()[_col_study._v_y_index] = v(1);
  ray.data()[_col_study._v_z_index] = v(2);
}

void
CollisionKernel::onSegment()
{
}

void
CollisionKernel::preTrace()
{
}
