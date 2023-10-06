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

class BorisKernel : public GeneralRayKernel
{
public:
  BorisKernel(const InputParameters & params);

  static InputParameters validParams();

  virtual void preTrace() override;

  virtual void onSegment() override;

protected:
  /// The variable number of the variable we are operating on
  const std::vector<VariableName> _components;
  std::vector<unsigned int> _var_numbers;

  /// A reference to the system containing the variable
  const System & _system;
  std::vector<libMesh::Point> _vectors;

private:
  void getVectorValues();
};
