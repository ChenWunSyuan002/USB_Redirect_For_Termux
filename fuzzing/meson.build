compiler.has_header('fuzzer/FuzzedDataProvider.h', required: true)

usbredirparserfuzz_deps = []
usbredirparserfuzz_link_args = []

if fuzzing_engine.found()
  usbredirparserfuzz_deps += fuzzing_engine
else
  usbredirparserfuzz_link_args += ['-fsanitize=fuzzer']
endif

test_llvmfuzzer_function = '''
#include <cstddef>
#include <cstdint>
'''
builtin = compiler.has_function('LLVMFuzzerTestOneInput',
    prefix : test_llvmfuzzer_function)
if not builtin
    standalone_sources = [
        'standalone.c',
    ]

    standalone_lib = static_library(
        'standalone',
        standalone_sources,
        install : false)

    usbredirparserfuzz_deps += declare_dependency(link_with: standalone_lib)
endif

usbredirparserfuzz_sources = [
    'usbredirparserfuzz.cc',
]

usbredirparserfuzz_deps += usbredir_parser_lib_dep

executable('usbredirparserfuzz',
    sources : usbredirparserfuzz_sources,
    c_args : '-Wno-deprecated-declarations',
    install : false,
    link_args: usbredirparserfuzz_link_args,
    dependencies : usbredirparserfuzz_deps)
