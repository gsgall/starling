//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "KillParticleBC.h"
#include "PICRayStudy.h"

registerMooseObject("StarlingApp", KillParticleBC);

InputParameters
KillParticleBC::validParams()
{
  auto params = KillRayBC::validParams();
  params.addClassDescription("A RayBC that kills a Particle on a boundary.");
  return params;
}

KillParticleBC::KillParticleBC(const InputParameters & params) : KillRayBC(params) {}

void
KillParticleBC::onBoundary(const unsigned int num_applying)
{

  // After RayBCs are completed, ray->shouldContinue() is checked and this will kill the Ray
  PICRayStudy * picStudy = dynamic_cast<PICRayStudy *>(&_study);
  // giving the ray a killed status so that we can tell it was killed by the boundary
  // and not that it just hit its max distance during the time step
  std::cout << "Particle Killed" << std::endl;
  if (picStudy)
    currentRay()->data()[picStudy->_killed_index] = true;
  std::cout << currentRay()->data()[picStudy->_killed_index] << std::endl;
  KillRayBC::onBoundary(num_applying);
}
