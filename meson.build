project('usbredir', 'c', 'cpp',
    version: '0.8.0',
    license: 'LGPLv2.1+',
    meson_version : '>= 0.53',
    default_options : [
        'buildtype=debugoptimized',
        'warning_level=1',
    ])

summary_info = {'prefix': get_option('prefix')}

usbredir_include_root_dir = include_directories('.')

cc_flags = [
    '-Wp,-D_FORTIFY_SOURCE=2',
    '--param=ssp-buffer-size=4',
]

# Check if we are building from .git
git = run_command('test', '-d', '.git').returncode() == 0
git_werror = get_option('git_werror')
if git_werror.enabled() or git_werror.auto() and git
  cc_flags += [ '-Werror' ]
endif

compiler = meson.get_compiler('c')
supported_cc_flags = compiler.get_supported_arguments(cc_flags)
add_project_arguments(supported_cc_flags, language: 'c')

if host_machine.system() != 'windows'
    add_project_arguments('-fstack-protector', language: 'c')
endif

want_libfuzzer = get_option('llvm-fuzz').enabled()
if want_libfuzzer
    add_languages('cpp', required : true)
    fuzzing_engine = meson.get_compiler('cpp').find_library('Fuzzer', required : false)
    if fuzzing_engine.found()
        add_project_arguments('-fsanitize-coverage=trace-pc-guard,trace-cmp', language : 'c')
    elif compiler.has_argument('-fsanitize=fuzzer-no-link')
        add_project_arguments('-fsanitize=fuzzer-no-link', language : 'c')
    else
        error('Looks like neither libFuzzer nor -fsanitize=fuzzer-no-link is supported')
    endif
    # This is preferable to static linking
    add_project_link_arguments('-shared-libsan', language: ['c', 'cpp'])
endif

config = configuration_data()

#
# write config.h
#
proj_name = meson.project_name()
proj_version = meson.project_version()
config_data = {
    'VERSION' : proj_version,
    'PACKAGE_VERSION' : proj_version,
    'PACKAGE_STRING' : '@0@ @1@'.format(proj_name, proj_version),
    'PACKAGE_BUGREPORT' : 'https://gitlab.freedesktop.org/spice/usbredir/issues',
}

foreach key, value : config_data
    config.set_quoted(key, value)
endforeach

#
# check for system headers
#
headers = [
    'inttypes.h',
    'stdint.h',
    'stdlib.h',
    'strings.h',
    'string.h',
    'sys/stat.h',
    'sys/types.h',
    'unistd.h',
]

foreach header : headers
    if compiler.has_header(header)
        config.set('HAVE_@0@'.format(header.underscorify().to_upper()), '1')
    endif
endforeach

if host_machine.system() == 'windows'
    wixl_arch = 'x64'
    if host_machine.cpu() != 'x86_64'
        wixl_arch = 'x86'
    endif
    config.set('WIXL_ARCH', wixl_arch)
endif

configure_file(output : 'config.h', configuration : config)

subdir('usbredirparser')
subdir('usbredirhost')
if get_option('tools').enabled()
    subdir('tools')
endif
if host_machine.system() != 'windows'
    subdir('usbredirserver')
    subdir('usbredirtestclient')
    if want_libfuzzer
        subdir('fuzzing')
    endif
endif
subdir('data')

summary(summary_info, bool_yn: true)
