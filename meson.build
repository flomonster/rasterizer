project('rasterizer', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++14'])

assimp_dep = dependency('assimp')
deps = [assimp_dep]

executable('rasterizer',
           'src/main.cpp',
           'lib/obj-loader.hh',
           install : true,
           dependencies: deps)

