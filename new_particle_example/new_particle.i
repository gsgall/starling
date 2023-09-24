[Mesh/gmg]
  type = GeneratedMeshGenerator
  dim = 2
  nx = 20
  ny = 20
  xmax = 20
  ymax = 20
[]

[UserObjects/study]
  type = PICRayStudy
  start_points = '0 10.5 0'
  start_velocities = '1 0 1'
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
  type = NewParticleKernel
[]

[Outputs/rays]
  type = RayTracingExodus
  study = study
  execute_on = 'TIMESTEP_BEGIN'
[]
