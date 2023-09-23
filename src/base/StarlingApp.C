#include "StarlingApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

InputParameters
StarlingApp::validParams()
{
  InputParameters params = MooseApp::validParams();
  params.set<bool>("use_legacy_material_output") = false;
  return params;
}

StarlingApp::StarlingApp(InputParameters parameters) : MooseApp(parameters)
{
  StarlingApp::registerAll(_factory, _action_factory, _syntax);
}

StarlingApp::~StarlingApp() {}

void 
StarlingApp::registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  ModulesApp::registerAllObjects<StarlingApp>(f, af, s);
  Registry::registerObjectsTo(f, {"StarlingApp"});
  Registry::registerActionsTo(af, {"StarlingApp"});

  /* register custom execute flags, action syntax, etc. here */
}

void
StarlingApp::registerApps()
{
  registerApp(StarlingApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
extern "C" void
StarlingApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  StarlingApp::registerAll(f, af, s);
}
extern "C" void
StarlingApp__registerApps()
{
  StarlingApp::registerApps();
}
