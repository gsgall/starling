//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "RayTracingStudy.h"

/**
 * Test study for generating rays for a basic particle-in-cell capability,
 * where Rays have propagate a bit each time step
 */
class CollisionRayStudy : public RayTracingStudy
{
public:
  CollisionRayStudy(const InputParameters & parameters);

  static InputParameters validParams();

  virtual void generateRays() override;
  // the particle radius
  const Real _rad;
  const RayDataIndex _v_x_index;
  const RayDataIndex _v_y_index;
  const RayDataIndex _v_z_index;
  // map of particles to enable collision checking
  std::unique_ptr<std::unordered_map<const Elem *, std::vector<std::shared_ptr<Ray>>>> _p_map;
  // sets the rays max dist and direction based on velocity
  void setDirectionAndMaxDistance(Ray & ray);

  /// The banked rays to be used on the next timestep (restartable)
  std::vector<std::shared_ptr<Ray>> & _banked_rays;

protected:
  virtual void postExecuteStudy() override;

private:
  // the number of particles in the study
  const Real & _n;
  // the min and max bounds where particles will be generated
  const Real & _xmin;
  const Real & _xmax;
  const Real & _ymin;
  const Real & _ymax;
  // the minimum velocity used when generating the rays
  const Real & _vmin;
  const Real & _vmax;
  /// Whether or not we've generated rays yet (restartable)
  bool & _has_generated;
};
