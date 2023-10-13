//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "CollisionRayStudy.h"

#include "ClaimRays.h"
#include "Function.h"
#include "MooseRandom.h"

registerMooseObject("StarlingApp", CollisionRayStudy);

InputParameters
CollisionRayStudy::validParams()
{
  auto params = RayTracingStudy::validParams();

  params.addRequiredParam<Real>("n", "The number of particles in the study");
  params.addParam<Real>("xmin", 0, "The x minimum of where particles should be generated");
  params.addRequiredParam<Real>("xmax", "The x maximuim of where particles should be generated");
  params.addParam<Real>("ymin", 0, "The y minimum of where particles should be generated");
  params.addRequiredParam<Real>("ymax", "The y maximuim of where particles should be generated");
  params.addRequiredParam<Real>(
      "vmin", "The minimum velocity to use when randomly sampling for base particles");
  params.addRequiredParam<Real>(
      "vmax", "The maximum velocity to use when randomly sampling for base particles");
  params.addRequiredParam<Real>("radius", "The effective particle radius");
  // We're not going to use registration because we don't care to name our rays because
  // we will have a lot of them
  params.set<bool>("_use_ray_registration") = false;

  return params;
}

CollisionRayStudy::CollisionRayStudy(const InputParameters & parameters)
  : RayTracingStudy(parameters),
    _rad(getParam<Real>("radius")),
    _v_x_index(registerRayData("v_x")),
    _v_y_index(registerRayData("v_y")),
    _v_z_index(registerRayData("v_z")),
    _p_map(
        libmesh_make_unique<std::unordered_map<const Elem *, std::vector<std::shared_ptr<Ray>>>>()),
    _banked_rays(
        declareRestartableDataWithContext<std::vector<std::shared_ptr<Ray>>>("_banked_rays", this)),
    _n(getParam<Real>("n")),
    _xmin(getParam<Real>("xmin")),
    _xmax(getParam<Real>("xmax")),
    _ymin(getParam<Real>("ymin")),
    _ymax(getParam<Real>("ymax")),
    _vmin(getParam<Real>("vmin")),
    _vmax(getParam<Real>("vmax")),
    _has_generated(declareRestartableData<bool>("has_generated", false))
{
}

void
CollisionRayStudy::generateRays()
{
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
    std::vector<std::shared_ptr<Ray>> rays(_n);
    // create a buffer so we don't have the possibility of creating a particle
    // on the boundaries
    const float xbuffer = 1e-3;
    const float ybuffer = 1e-3;
    // the distribution to sample from
    // Distribution const * dist = &getDistributionByName("uniform");
    for (int i = 0; i < _n; ++i)
    {
      rays[i] = acquireReplicatedRay();
      auto xstart = MooseRandom::rand() * ((_xmax - xbuffer) - (_xmin + xbuffer)) + _xmin + xbuffer;
      auto ystart = MooseRandom::rand() * ((_ymax - ybuffer) - (_ymin + ybuffer)) + _ymin + ybuffer;
      rays[i]->setStart(Point(xstart, ystart, 0));

      // randomly assign startign velocities
      auto vx_start = MooseRandom::rand() * (_vmax - _vmin) + _vmin;
      if (MooseRandom::rand() > 0.5)
      {
        vx_start *= -1;
      }

      auto vy_start = MooseRandom::rand() * (_vmax - _vmin) + _vmin;

      if (MooseRandom::rand() > 0.5)
      {
        vy_start *= -1;
      }

      auto vz_start = MooseRandom::rand() * (_vmax - _vmin) + _vmin;

      if (MooseRandom::rand() > 0.5)
      {
        vz_start *= -1;
      }

      rays[i]->data()[_v_x_index] = vx_start;
      rays[i]->data()[_v_y_index] = vy_start;
      // ignoring z direction for now
      rays[i]->data()[_v_z_index] = 0;
      // setting the direction and distance the ray can travel
      setDirectionAndMaxDistance(*rays[i]);
    }
    // Claim the rays
    std::vector<std::shared_ptr<Ray>> claimed_rays;
    ClaimRays claim_rays(*this, rays, claimed_rays, false);
    claim_rays.claim();

    // ...and then add them to be traced
    moveRaysToBuffer(claimed_rays);
  }
  // Rays are in the bank: reset them
  else
  {
    // Reset each ray
    for (auto it = _banked_rays.begin(); _banked_rays.size() != 0 && it != _banked_rays.end(); ++it)
    {
      auto ray = *it;
      // Store off the ray's info before we reset it
      const auto start_point = ray->currentPoint();

      const auto elem = ray->currentElem();

      // Reset it (this is required to reuse a ray)
      ray->resetCounters();
      ray->clearStartingInfo();

      // And set the new starting information
      ray->setStart(start_point, elem);
      setDirectionAndMaxDistance(*ray);
    }

    if (_banked_rays.size() != 0)
    {
      moveRaysToBuffer(_banked_rays);
      _banked_rays.clear();
    }
    // Add the rays to be traced
  }

  _has_generated = true;
}

void
CollisionRayStudy::postExecuteStudy()
{
  // Copy the rays that are banked in the study into our own bank
  _banked_rays = rayBank();
  // resetting the paricle map
  _p_map->clear();
}

void
CollisionRayStudy::setDirectionAndMaxDistance(Ray & ray)
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
      velocity(0) = ray.data()[_v_x_index];
      velocity(1) = ray.data()[_v_y_index];
      break;
    case 3:
      velocity(0) = ray.data()[_v_x_index];
      velocity(1) = ray.data()[_v_y_index];
      velocity(2) = ray.data()[_v_z_index];
      break;
  }
  const auto max_dist = std::sqrt(velocity * velocity) * _fe_problem.dt();
  const auto direction = velocity.unit();

  ray.setStartingMaxDistance(max_dist);
  ray.setStartingDirection(direction);
}
