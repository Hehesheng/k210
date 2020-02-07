from building import * 

# get current dir path
cwd = GetCurrentDir()

# init src and inc vars
src = []
inc = []

# add dstr common include
inc = inc + [cwd + '/inc']

# add dstr basic code
src = src + [cwd + '/src/dstr.c']

# add dstr test code
if GetDepend('DSTR_USING_EXAMPLE'):
    src = src + [cwd + '/examples/examples_dstr.c']


# add group to IDE project
group = DefineGroup('dstr', src, depend = ['PKG_USING_DSTR'], CPPPATH = inc)

Return('group')