//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "GeneralVectorPostprocessor.h"
// Forward declarations
class PICRayStudy;

class RayDataValues : public GeneralVectorPostprocessor
{
public:
  static InputParameters validParams();

  RayDataValues(const InputParameters & parameters);

  virtual void initialize() override;
  virtual void execute() override;
  virtual void finalize() override;

protected:
  /// The RayTracingStudy
  const PICRayStudy & _study;
  /// Columns of quadrature point coordinates.
  // VectorPostprocessorValue & _id;
  VectorPostprocessorValue & _x;
  VectorPostprocessorValue & _y;
  VectorPostprocessorValue & _z;

  VectorPostprocessorValue & _v_x;
  VectorPostprocessorValue & _v_y;
  VectorPostprocessorValue & _v_z;
};
