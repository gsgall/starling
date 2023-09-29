[Mesh/gmg]
  type = GeneratedMeshGenerator
  dim = 2
  nx = 10
  ny = 10
  xmax = 10
  ymax = 10
[]

[UserObjects/study]
  type = PICRayStudy
  start_points = '0 5.5 0'
  start_velocities = '1 0 0 '
  species = 'null'
  execute_on = 'TIMESTEP_BEGIN'
  always_cache_traces = true
  tolerate_failure = true
[]

[RayBCs]
  [kill]
    type = KillRayBC
    boundary = 'top bottom right left'
    study = study
  []
[]

[Executioner]
  type = Transient
  num_steps = 11
[]

[Problem]
  solve = false
[]

[RayKernels/kernel]
  type = NullRayKernel
[]

[Outputs/rays]
  type = RayTracingExodus
  study = study
  execute_on = 'TIMESTEP_BEGIN'
[]
