//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "BorisKernel.h"
// MOOSE includes
#include "Function.h"
#include "MooseMesh.h"
#include "MooseVariable.h"
#include "SubProblem.h"

#include "libmesh/system.h"

#include "PICRayStudy.h"

registerMooseObject("StarlingApp", BorisKernel);

InputParameters
BorisKernel::validParams()
{
  InputParameters params = GeneralRayKernel::validParams();

  params.addClassDescription("Compute the value of a variable at a specified location.");
  params.addRequiredParam<std::vector<VariableName>>(
      "components", "The name of the vector components that act on the particles");
  params.addClassDescription("Compute a force on the particle due a vector");
  return params;
}

BorisKernel::BorisKernel(const InputParameters & params)
  : GeneralRayKernel(params),
    _pic_study(dynamic_cast<PICRayStudy &>(_study)),
    _components(getParam<std::vector<VariableName>>("components")),
    _var_numbers(_components.size(), 0),
    _system(_fe_problem.getSystem(_components[0])),
    _vectors(_components.size() / 3, libMesh::Point(0, 0, 0))
{
  if (_components.size() % 3 != 0)
  {
    paramError("components", "Each vector must have 3 components");
  }

  for (const auto i : index_range(_components))
  {
    _var_numbers[i] = _fe_problem
                          .getVariable(_tid,
                                       _components[i],
                                       Moose::VarKindType::VAR_ANY,
                                       Moose::VarFieldType::VAR_FIELD_STANDARD)
                          .number();
  }
}

void
BorisKernel::preTrace()
{
  getVectorValues();
  // getting data in a more convient form
  auto ray = currentRay();
  auto E = _vectors[0];
  auto B = _vectors[1];
  Point v = Point(ray->data()[_pic_study._v_x_index],
                  ray->data()[_pic_study._v_y_index],
                  ray->data()[_pic_study._v_z_index]);
  // std::cout << v << std::endl;
  auto dt = _fe_problem.dt() / 2;
  auto q = ray->data()[_pic_study._charge_index];
  auto m = ray->data()[_pic_study._mass_index];

  // taking the first E field half step
  auto v_minus = electricHalfStep(v, E, dt, q, m);

  // rotating the velocity with the magnetic field
  auto v_plus = magneticForce(v_minus, B, dt, q, m);

  // taking the second E field half step
  auto v_new = electricHalfStep(v_plus, E, dt, q, m);

  updateVelocity(*ray, v_new);
}

void
BorisKernel::postTrace()
{
  getVectorValues();
  // getting data in a more convient form
  auto ray = currentRay();
  auto E = _vectors[0];
  auto B = _vectors[1];
  Point v = Point(ray->data()[_pic_study._v_x_index],
                  ray->data()[_pic_study._v_y_index],
                  ray->data()[_pic_study._v_z_index]);
  // std::cout << v << std::endl;
  auto dt = _fe_problem.dt() / 2;
  auto q = ray->data()[_pic_study._charge_index];
  auto m = ray->data()[_pic_study._mass_index];

  // taking the first E field half step
  auto v_minus = electricHalfStep(v, E, dt, q, m);

  // rotating the velocity with the magnetic field
  auto v_plus = magneticForce(v_minus, B, dt, q, m);

  // taking the second E field half step
  auto v_new = electricHalfStep(v_plus, E, dt, q, m);

  ray->data()[_pic_study._v_x_index] = v_new(0);
  ray->data()[_pic_study._v_y_index] = v_new(1);
  ray->data()[_pic_study._v_z_index] = v_new(2);
}

void
BorisKernel::updateVelocity(Ray & ray, const Point v)
{
  ray.data()[_pic_study._v_x_index] = v(0);
  ray.data()[_pic_study._v_y_index] = v(1);
  ray.data()[_pic_study._v_z_index] = v(2);

  ray.setStartingDirection(v.unit());
  ray.setStartingMaxDistance(v.norm() * _fe_problem.dt());
}

Point
BorisKernel::electricHalfStep(
    const Point v, const Point E, const Real dt, const Real q, const Real m)
{
  return v + (q / m) * E * dt / 2;
}

Point
BorisKernel::magneticForce(const Point v, const Point B, const Real dt, const Real q, const Real m)
{
  // constant multiplied on both sides
  const auto a = q * dt / (2 * m);
  // left hand side of the equation
  const auto b = v + a * v.cross(B);
  // std::cout << v << std::endl;
  // std::cout << b << std::endl;
  // solving system of equations using cramer rule
  const auto a2 = a * a;
  const auto B2 = Point(B(0) * B(0), B(1) * B(1), B(2) * B(2));
  const auto denom = (B * B) * a2 + 1;
  // calculating the x component
  auto term1 = b(0) * (B2(0) * a2 + 1);
  auto term2 = b(1) * (B(0) * B(1) * a2 + B(2) * a);
  auto term3 = b(2) * (B(0) * B(2) * a2 - B(1) * a);
  const auto vx = (term1 + term2 + term3) / denom;
  // calculating the y component
  term1 = b(0) * (B(0) * B(1) * a2 - B(2) * a);
  term2 = b(1) * (B2(1) * a2 + 1);
  term3 = b(2) * (B(0) * a + B(1) * B(2) * a2);
  const auto vy = (term1 + term2 + term3) / denom;
  // calculating the z component
  term1 = b(0) * (B(0) * B(2) * a2 + B(1) * a);
  term2 = b(1) * (-B(0) * a + B(1) * B(2) * a2);
  term3 = b(2) * (B2(2) * a2 + 1);
  const auto vz = (term1 + term2 + term3) / denom;
  Point v_new = Point(vx, vy, vz);
  // std::cout << v_new << std::endl;
  return v_new;
}

// Point
// BorisKernel::magneticForce(const Point v, const Point B, const Real dt, const Real q, const Real
// m)
// {
//   // constant multiplied on both sides
//   const auto a = q * dt / (2 * m);
//   // left hand side of the equation
//   const auto b = v + a * v.cross(B);
//   const auto C = a * B;
//   // constants for the matrix inversion
//   const auto D1 = (C(0) - C(1) * C(2)) / (1 + C(2) * C(2));
//   const auto D2 = 1 / (1 + C(2) * C(2));
//   const auto D3 = 1 / (1 + C(1) * C(1) - D1 * (-C(0) - C(1) * C(2)));
//   const auto D4 = D2 * (-C(0) - C(1) * C(2));

//   // redfine the LHS
//   const auto fy = D2 * (b(1) - C(2) * b(0));
//   const auto fz = D3 * (b(0) + C(1) * b(0) - D1 * (b(1) - C(2) * b(0)));
//   // final solution for new velocities
//   const auto vx = b(0) - C(1) * fz + C(2) * (fy - D4 * fz);
//   const auto vy = fy - D4 * fz;
//   // putting it all together in a Point
//   Point v_new = Point(vx, vy, fz);
//   return v_new;
// }

void
BorisKernel::onSegment()
{
}

void
BorisKernel::getVectorValues()
{
  auto cur_point = currentRay()->currentPoint();
  auto element = currentRay()->currentElem();
  for (const auto i : index_range(_vectors))
  {
    _vectors[i](0) = _system.point_value(_var_numbers[i * 3], cur_point, element);
    _vectors[i](1) = _system.point_value(_var_numbers[i * 3 + 1], cur_point, element);
    _vectors[i](2) = _system.point_value(_var_numbers[i * 3 + 2], cur_point, element);
  }
}
