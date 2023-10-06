[Mesh/gmg]
  type = GeneratedMeshGenerator
  dim = 2
  nx = 20
  ny = 20
  xmax = 20
  ymax = 20
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
  [ic_x]
    type = ParsedFunction
    expression = 'x'
  []
  [ic_y]
    type = ParsedFunction
    expression = '4 * x'
  []
  [ic_z]
    type = ParsedFunction
    expression = '-x'
  []
[]
[UserObjects/study]
  type = PICRayStudy
  start_points = '0 19 0
                  19 0 0'
  start_velocities = '1 -1 1
                      -1.1 0.28 0'
  species = 'null'
  execute_on = 'TIMESTEP_BEGIN'
  always_cache_traces = true
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
  num_steps = 10
[]

[Problem]
  solve = false
[]

[RayKernels/kernel]
  type = VectorForceKernel
  components = 'Ex Ey Ez
                Bx By Bz'
[]

[VectorPostprocessors]
  [ray_data]
    type = RayDataValues
    study = study
    execute_on = 'TIMESTEP_BEGIN'
  []
[]

[Outputs]
  [rays]
    type = RayTracingExodus
    study = study
    execute_on = 'TIMESTEP_BEGIN'
  []
  csv = true
[]
