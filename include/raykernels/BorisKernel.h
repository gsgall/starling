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
#include "PICRayStudy.h"

class BorisKernel : public GeneralRayKernel
{
public:
  BorisKernel(const InputParameters & params);

  static InputParameters validParams();
  // easy access point for the data indicies in the study
  const PICRayStudy & _pic_study;

  virtual void preTrace() override;

  virtual void postTrace() override;

  virtual void onSegment() override;

  /**
   * Method to take the half steps for the acceleration due to the e field
   * @param v a point that represents the current velocity before acceleration
   * @param E a point representing the electric field at the point of the ray
   * @param dt the current time step
   * @param q the charge of the particle
   * @param m the mass of the particle
   */
  Point electricHalfStep(const Point v, const Point E, const Real dt, const Real q, const Real m);

  /**
   * Method to take the half steps for the acceleration due to the e field
   * @param v a point that represents the current velocity before acceleration
   * @param B a point representing the magnetic field at the point of the ray
   * @param dt the current time step
   * @param q the charge of the particle
   * @param m the mass of the particle
   */
  Point magneticForce(const Point v, const Point B, const Real dt, const Real q, const Real m);

  /**
   * Method for giving the user an easy way to update the particles velocity
   * @param ray the ray you would like to update the velocity of
   * @param v the new velocity you want to assign to the ray
   */
  void updateVelocity(Ray & ray, const Point v);

protected:
  /// The variable number of the variable we are operating on
  const std::vector<VariableName> _components;
  // a place to store the variable numbers of the componets of the
  // vectors to access their values at points
  std::vector<unsigned int> _var_numbers;

  /// A reference to the system containing the variable
  const System & _system;
  // the force vectors acting on the particle
  std::vector<libMesh::Point> _vectors;

private:
  /**
   * Method for sampling the value of the variables stored in the vectors
   * This will update the value of _vectors to be the value of those forces
   * at the current point the ray exists at
   */
  void getVectorValues();
};
