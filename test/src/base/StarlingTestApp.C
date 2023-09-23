//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
#include "StarlingTestApp.h"
#include "StarlingApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "MooseSyntax.h"

InputParameters
StarlingTestApp::validParams()
{
  InputParameters params = StarlingApp::validParams();
  params.set<bool>("use_legacy_material_output") = false;
  return params;
}

StarlingTestApp::StarlingTestApp(InputParameters parameters) : MooseApp(parameters)
{
  StarlingTestApp::registerAll(
      _factory, _action_factory, _syntax, getParam<bool>("allow_test_objects"));
}

StarlingTestApp::~StarlingTestApp() {}

void
StarlingTestApp::registerAll(Factory & f, ActionFactory & af, Syntax & s, bool use_test_objs)
{
  StarlingApp::registerAll(f, af, s);
  if (use_test_objs)
  {
    Registry::registerObjectsTo(f, {"StarlingTestApp"});
    Registry::registerActionsTo(af, {"StarlingTestApp"});
  }
}

void
StarlingTestApp::registerApps()
{
  registerApp(StarlingApp);
  registerApp(StarlingTestApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
// External entry point for dynamic application loading
extern "C" void
StarlingTestApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  StarlingTestApp::registerAll(f, af, s);
}
extern "C" void
StarlingTestApp__registerApps()
{
  StarlingTestApp::registerApps();
}
