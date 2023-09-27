[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 30
  ny = 6
  xmin = -15.0
  xmax = 15.0
  ymin = -3.0
  ymax = 3.0
  elem_type = QUAD4
[]

[Variables]
  [c]
    order = FIRST
    family = LAGRANGE
    initial_condition = 1
  []
[]

[Postprocessors]
  [dt]
    type = TimestepSize
  []
[]

[UserObjects]
  [arnold]
    type = Terminator
    expression = 'dt > 20'
    fail_mode = HARD
    error_level = INFO
    message = 'Arnold says this should end'
    execute_on = TIMESTEP_END
  []
[]

[Kernels]
  [cres]
    type = Diffusion
    variable = c
  []

  [time]
    type = TimeDerivative
    variable = c
  []
[]

[BCs]
  [c]
    type = DirichletBC
    variable = c
    boundary = left
    value = 0
  []
[]

[Executioner]
  type = Transient
  [TimeStepper]
    type = IterationAdaptiveDT
    dt = 4
  []
  nl_abs_step_tol = 1e-10
[]

[Outputs]
  csv = true
  print_linear_residuals = false
[]
