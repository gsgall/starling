//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "RayDataValues.h"
#include "PICRayStudy.h"

registerMooseObject("StarlingApp", RayDataValues);

InputParameters
RayDataValues::validParams()
{
  InputParameters params = GeneralVectorPostprocessor::validParams();
  params.addClassDescription("Collect that value of ray data on every time step");
  params.addRequiredParam<UserObjectName>("study", "The RayTracingStudy that owns the Ray");
  return params;
}

RayDataValues::RayDataValues(const InputParameters & parameters)
  : GeneralVectorPostprocessor(parameters),
    _study(getUserObject<PICRayStudy>("study")),
    // _id(declareVector("id")),
    _x(declareVector("x")),
    _y(declareVector("y")),
    _z(declareVector("z")),
    _v_x(declareVector("v_x")),
    _v_y(declareVector("v_y")),
    _v_z(declareVector("v_z"))
{
}

void
RayDataValues::initialize()
{
  // _id.clear();
  _x.clear();
  _y.clear();
  _z.clear();
  _v_x.clear();
  _v_y.clear();
  _v_z.clear();
}

void
RayDataValues::execute()
{
  const auto rays = _study._banked_rays;
  int count = 0;
  for (auto it = rays.begin(); rays.size() != 0 && it != rays.end(); ++it)
  {
    auto ray = *it;
    // _id.push_back(count);
    _x.push_back(ray->currentPoint()(0));
    _y.push_back(ray->currentPoint()(1));
    _z.push_back(ray->currentPoint()(2));
    _v_x.push_back(ray->data()[_study._v_x_index]);
    _v_y.push_back(ray->data()[_study._v_y_index]);
    _v_z.push_back(ray->data()[_study._v_z_index]);
    count++;
  }
}

void
RayDataValues::finalize()
{
  // collect all processor data
  // comm().gather(0, _id);
  comm().gather(0, _x);
  comm().gather(0, _y);
  comm().gather(0, _z);
  comm().gather(0, _v_x);
  comm().gather(0, _v_y);
  comm().gather(0, _v_z);
}
