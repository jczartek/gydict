
test_cflags = [
  '-DTEST_DATA_DIR="@0@/"'.format(meson.current_source_dir())
]

test_dict_manager = executable('test-dict-manager', 'test-dict-manager.c',
        c_args: test_cflags,
  dependencies: [libgydict_dep])

test('test-dict-manager', test_dict_manager)

test_observable_source = [
  'test-observer.h',
  'test-observer.c',
  'test-observable.c'
]

test_observable = executable('test-observable', test_observable_source,
          c_args: test_cflags,
    dependencies: [libpattern_dep])

test('test-observable', test_observable)
