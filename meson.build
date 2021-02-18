project('usbredir', 'c',
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

configure_file(output : 'config.h', configuration : config)

subdir('usbredirparser')
subdir('usbredirhost')
if host_machine.system() != 'windows'
    subdir('usbredirserver')
    subdir('usbredirtestclient')
endif

summary(summary_info, bool_yn: true)
