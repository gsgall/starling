//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

// Local Includes
#include "GeneralRayKernel.h"
#include "MooseVariableInterface.h"
#include "CollisionRayStudy.h"
#include "MooseRandom.h"

class CollisionKernel : public GeneralRayKernel
{
public:
  CollisionKernel(const InputParameters & params);

  static InputParameters validParams();
  // easy access point for the data indicies in the study
  const CollisionRayStudy & _col_study;

  virtual void preTrace() override;

  virtual void postTrace() override;

  virtual void onSegment() override;

  /**
   * Method for giving the user an easy way to update the particles velocity
   * @param ray the ray you would like to update the velocity of
   * @param v the new velocity you want to assign to the ray
   */
  void updateVelocity(Ray & ray, const Point v);
};
