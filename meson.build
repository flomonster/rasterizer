project('rasterizer', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++17'])

assimp_dep = dependency('assimp')
boost_options_dep = dependency('boost', modules: ['program_options'])
deps = [assimp_dep, boost_options_dep]

inc = include_directories('src/includes')
executable('rasterizer',
           'src/main.cc',
           'src/draw.cc',
           'src/utils.cc',
           'src/ppm.cc',
           'src/color.cc',
           'src/opt-parser.cc',
           'src/shader.cc',
           include_directories: inc,
           install : true,
           dependencies: deps)
