[Mesh/gmg]
  type = GeneratedMeshGenerator
  dim = 2
  nx = 20
  ny = 20
  xmax = 20
  ymax = 20
[]

[UserObjects]
  [study]
    type = CollisionRayStudy
    n = 10000
    xmax = 20
    ymax = 20
    vmin = 10
    vmax = 10
    radius = 6e-2
    execute_on = 'TIMESTEP_BEGIN'
    always_cache_traces = true
  []
[]

[RayBCs]
  [reflect]
    type = CollisionReflectRayBC
    boundary = 'top bottom right left'
    study = study
  []
[]

[Executioner]
  type = Transient
  num_steps = 50
[]

[Problem]
  solve = false
[]

[RayKernels]
  [kernel]
    type = CollisionKernel
    study = study
  []
[]

[VectorPostprocessors]
  [ray_data]
    type = CollisionRayDataValues
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
