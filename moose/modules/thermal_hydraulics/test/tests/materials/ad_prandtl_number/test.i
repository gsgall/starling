[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 1
  allow_renumbering = false
[]

[Variables]
  [u]
  []
[]

[Kernels]
  [diff]
    type = ADDiffusion
    variable = u
  []
[]

[BCs]
  [left]
    type = DirichletBC
    variable = u
    boundary = left
    value = 0
  []
  [right]
    type = DirichletBC
    variable = u
    boundary = right
    value = 1
  []
[]

[AuxVariables]
  [Pr]
    family = MONOMIAL
    order = CONSTANT
  []
[]

[AuxKernels]
  [RPr_ak]
    type = ADMaterialRealAux
    variable = Pr
    property = Pr
  []
[]

[Materials]
  [props]
    type = ADGenericConstantMaterial
    prop_names = 'cp mu k'
    prop_values = '1 2 4'
  []

  [Pr_material]
    type = ADPrandtlNumberMaterial
    cp = cp
    k = k
    mu = mu
  []
[]

[Executioner]
  type = Steady
[]

[Postprocessors]
  [Pr]
    type = ElementalVariableValue
    elementid = 0
    variable = Pr
  []
[]

[Outputs]
  csv = true
  execute_on = timestep_end
[]
