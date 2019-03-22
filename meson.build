project('rasterizer', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++14'])

assimp_dep = dependency('assimp')
deps = [assimp_dep]

inc = include_directories('src/includes')
executable('rasterizer',
           'src/main.cc',
           'src/draw.cc',
           'src/ppm.cc',
           'src/color.cc',
           include_directories: inc,
           install : true,
           dependencies: deps)
