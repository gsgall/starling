[Mesh/gmg]
  type = GeneratedMeshGenerator
  dim = 2
  nx = 25
  ny = 7
  xmax = 22
  ymax = 7
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
    expression = '0'
  []
  [E_y_ic]
    type = ParsedFunction
    expression = '-9.81'
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
    expression = '0'
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
    start_points = '0 0 0'
    start_velocities = '10 10 0'
    species = 'null'
    charge = 1
    mass = 1
    execute_on = 'TIMESTEP_BEGIN'
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
  dt = 1e-3
  end_time = 2
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
    type = SingleRayDataValues
    study = study
    execute_on = 'TIMESTEP_BEGIN'
  []
[]

[Outputs]
  [data]
    type = CSV
    execute_on = 'FINAL'
    file_base = ''
  []
[]
