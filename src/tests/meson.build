
test_cflags = [
  '-DTEST_DATA_DIR="@0@/"'.format(meson.current_source_dir())
]

mutest_dep = dependency('mutest-1',
  fallback: [ 'mutest', 'mutest_dep' ],
  default_options: ['static=true'],
  required: false,
  disabler: true,
)

test_attributes = executable('test-attributes', 'test-attributes.c',
         dependencies: [libgydict_dep] + [mutest_dep],
)
test('test of attributes', test_attributes)

