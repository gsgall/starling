[Mesh/gmg]
  type = GeneratedMeshGenerator
  dim = 2
  nx = 20
  ny = 20
  xmin = -10
  xmax = 10
  ymin = -10
  ymax = 10
[]

[Variables]
  [Ex]
  []
  [Ey]
  []
  [Ez]
  []
  [Bx]
  []
  [By]
  []
  [Bz]
  []
[]

[Kernels]
  [Ex_null]
    type = NullKernel
    variable = Ex
  []
  [Ey_null]
    type = NullKernel
    variable = Ey
  []
  [Ez_null]
    type = NullKernel
    variable = Ez
  []
  [Bx_null]
    type = NullKernel
    variable = Bx
  []
  [By_null]
    type = NullKernel
    variable = By
  []
  [Bz_null]
    type = NullKernel
    variable = Bz
  []
[]

[Functions]
  [E_x_ic]
    type = ParsedFunction
    expression = '1e-2 * x / (x^2 + y^2 + 1e-6)^(-3/2)'
  []
  [E_y_ic]
    type = ParsedFunction
    expression = '1e-2 * y / (x^2 + y^2 + 1e-6)^(-3/2)'
  []
  [E_z_ic]
    type = ParsedFunction
    expression = '0'
  []
  [B_x_ic]
    type = ParsedFunction
    expression = '0'
  []
  [B_y_ic]
    type = ParsedFunction
    expression = '0'
  []
  [B_z_ic]
    type = ParsedFunction
    expression = 'sqrt(x^2 + y^2)'
  []
[]

[ICs]
  [Ex_ic]
    type = FunctionIC
    variable = Ex
    function = E_x_ic
  []
  [Ey_ic]
    type = FunctionIC
    variable = Ey
    function = E_y_ic
  []
  [Ez_ic]
    type = FunctionIC
    variable = Ez
    function = E_z_ic
  []
  [Bx_ic]
    type = FunctionIC
    variable = Bx
    function = B_x_ic
  []
  [By_ic]
    type = FunctionIC
    variable = By
    function = B_y_ic
  []
  [Bz_ic]
    type = FunctionIC
    variable = Bz
    function = B_z_ic
  []
[]

[UserObjects]
  [study]
    type = PICRayStudy
    start_points = '0 1 0'
    start_velocities = '1 0 0'
    species = 'null'
    charge = 1
    mass = 1
    execute_on = 'TIMESTEP_END'
    always_cache_traces = true
  []
[]

[RayBCs]
  [kill]
    type = KillParticleBC
    boundary = 'top bottom right left'
    study = study
  []
[]

[Executioner]
  type = Transient
  dt = 1e-2
  num_steps = 1000

[]

[Problem]
  solve = false
[]

[RayKernels/kernel]
  type = BorisKernel
  components = 'Ex Ey Ez
                Bx By Bz'
[]

[VectorPostprocessors]
  [ray_data]
    type = RayDataValues
    study = study
    execute_on = 'TIMESTEP_END'
  []
[]

[Outputs]
  [rays]
    type = RayTracingExodus
    study = study
    execute_on = 'TIMESTEP_END'
  []
  csv = true
[]
