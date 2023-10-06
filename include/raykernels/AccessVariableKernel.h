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

class AccessVariableKernel : public GeneralRayKernel
{
public:
  AccessVariableKernel(const InputParameters & params);

  static InputParameters validParams();

  virtual void preTrace() override;

  virtual void onSegment() override;

protected:
  /// The variable number of the variable we are operating on
  const unsigned int _var_number;

  /// A reference to the system containing the variable
  const System & _system;

  /// The value of the variable at the desired location
  Real _value;
};
