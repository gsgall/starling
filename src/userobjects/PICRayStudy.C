//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "PICRayStudy.h"

#include "ClaimRays.h"
#include "Function.h"

registerMooseObject("StarlingApp", PICRayStudy);

InputParameters
PICRayStudy::validParams()
{
  auto params = RayTracingStudy::validParams();

  params.addRequiredParam<std::vector<Point>>("start_points",
                                              "The point(s) where the ray(s) start");
  params.addRequiredParam<std::vector<Point>>(
      "start_velocities",
      "The direction(s) that the ray(s) start in (does not need to be normalized)");

  params.addRequiredParam<std::vector<std::string>>(
      "species", "The species of particles that exist in the study");

  // We're not going to use registration because we don't care to name our rays because
  // we will have a lot of them
  params.set<bool>("_use_ray_registration") = false;

  return params;
}

PICRayStudy::PICRayStudy(const InputParameters & parameters)
  : RayTracingStudy(parameters),
    _killed_index(registerRayData("killed")),
    _charge_index(registerRayData("charge")),
    _mass_index(registerRayData("mass")),
    _species_index(registerRayData("species")),
    _v_x_index(registerRayData("v_x")),
    _v_y_index(registerRayData("v_y")),
    _v_z_index(registerRayData("v_z")),
    _start_points(getParam<std::vector<Point>>("start_points")),
    _start_velocities(getParam<std::vector<Point>>("start_velocities")),
    _species_list(getParam<std::vector<std::string>>("species")),
    _has_generated(declareRestartableData<bool>("has_generated", false)),
    _banked_rays(
        declareRestartableDataWithContext<std::vector<std::shared_ptr<Ray>>>("_banked_rays", this))
{
  if (_start_points.size() != _start_velocities.size())
    paramError("start_velocities", "Must be the same size as 'start_points'");
}

void
PICRayStudy::generateRays()
{
  // mapping the species names to an index for each one provided
  for (const auto i : index_range(_species_list))
  {
    _species_map[i] = _species_list[i];
  }
  // We generate rays the first time only, after that we will
  // pull from the bank and update velocities/max distances
  if (!_has_generated)
  {
    // The unclaimed rays that we're going to generate
    // Here we need to "claim" rays because in parallel, we have
    // a list of points but do not know which processor will
    // own the point that that ray starts in. So, we duplicate
    // the rays on all processors and then let one processor pick them.
    // Basically - we fill them here and then pass them to a ClaimRays
    // object to do all of the magic. In a real PIC case, we'll just
    // generate the rays for the local rays that we care about
    // and the claiming probably won't be necessary
    std::vector<std::shared_ptr<Ray>> rays(_start_points.size());
    // Create a ray for each point/direction/velocity triplet
    // Note that instead of keeping track of the velocity, we're
    // just going to set the maximum distance that a ray can
    // travel based on the timestep * the starting velocity.
    for (const auto i : index_range(_start_points))
    {
      // std::cout << _fe_problem.mesh().dimension() << std::endl;
      rays[i] = acquireReplicatedRay();
      rays[i]->setStart(_start_points[i]);
      // killed index to check if the ray was killed by a boundary
      // or collision
      rays[i]->data()[_killed_index] = false;

      // saving the inital velocities so we can have 3v (1d or 2d)
      rays[i]->data()[_v_x_index] = _start_velocities[i](0);
      rays[i]->data()[_v_y_index] = _start_velocities[i](1);
      rays[i]->data()[_v_z_index] = _start_velocities[i](2);

      // temp direction so we can adjust the direction the ray is
      // traced in for the problem dimension
      libMesh::Point direction = Point(0, 0, 0);

      // setting directions so rays don't try and go outside of the mesh
      switch (_fe_problem.mesh().dimension())
      {
        case 1:
          direction(0) = _start_velocities[i](0);
          break;
        case 2:
          std::cout << "2 D Problem" << std::endl;
          direction(0) = _start_velocities[i](0);
          direction(1) = _start_velocities[i](1);
          break;
        case 3:
          std::cout << "3 D Problem" << std::endl;
          direction(0) = _start_velocities[i](0);
          direction(1) = _start_velocities[i](1);
          direction(2) = _start_velocities[i](2);
          break;
      }
      rays[i]->data()[_charge_index] = 0;
      rays[i]->data()[_mass_index] = 0;
      rays[i]->data()[_species_index] = 0;

      std::cout << _fe_problem.mesh().dimension() << std::endl;
      rays[i]->setStartingDirection(direction.unit());
      rays[i]->setStartingMaxDistance(maxDistance(*rays[i]));
    }

    // Claim the rays
    std::vector<std::shared_ptr<Ray>> claimed_rays;
    ClaimRays claim_rays(*this, rays, claimed_rays, false);
    claim_rays.claim();
    // ...and then add them to be traced
    moveRaysToBuffer(claimed_rays);
  }
  // Rays are in the bank: reset them and update the velocities
  else
  {
    // Reset each ray
    for (auto it = _banked_rays.begin(); it != _banked_rays.end(); ++it)
    {
      auto ray = *it;
      // if the ray was killed by a boundary or a collision then do not
      // add it to the buffer to retrace
      // really this should add the ray to another holding area to be reused
      // again when a new particle is created during an ionization or injection
      if (ray->data()[_killed_index])
      {
        _banked_rays.erase(it);
        continue;
      }

      // Store off the ray's info before we reset it
      const auto start_point = ray->currentPoint();
      const auto direction = ray->direction();
      const auto elem = ray->currentElem();

      // Reset it (this is required to reuse a ray)
      ray->resetCounters();
      ray->clearStartingInfo();

      // And set the new starting information
      ray->setStart(start_point, elem);
      ray->setStartingDirection(direction);
      ray->setStartingMaxDistance(maxDistance(*ray));
    }
    // Add the rays to be traced
    moveRaysToBuffer(_banked_rays);
    _banked_rays.clear();
  }

  _has_generated = true;
}

void
PICRayStudy::postExecuteStudy()
{
  // Copy the rays that are banked in the study into our own bank
  _banked_rays = rayBank();
}

Real
PICRayStudy::maxDistance(const Ray & ray) const
{
  // velocity * dt
  libMesh::Point velocity = Point(0, 0, 0);
  // calculating max distance for the correct problem dimention
  switch (_fe_problem.mesh().dimension())
  {
    case 1:
      velocity(0) = ray.data()[_v_x_index];
      break;
    case 2:
      std::cout << "2D" << std::endl;
      velocity(0) = ray.data()[_v_x_index];
      velocity(1) = ray.data()[_v_y_index];
      break;
    case 3:
      std::cout << "3D" << std::endl;
      velocity(0) = ray.data()[_v_x_index];
      velocity(1) = ray.data()[_v_y_index];
      velocity(2) = ray.data()[_v_z_index];
      break;
  }
  return std::sqrt(velocity * velocity) * _fe_problem.dt();
}
