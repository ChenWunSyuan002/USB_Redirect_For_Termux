# so verison, see:
# http://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html
usbredir_host_current = 1
usbredir_host_revision = 2
usbredir_host_age = 0
usbredir_host_so_version = '@0@.@1@.@2@'.format(
    usbredir_host_current - usbredir_host_age,
    usbredir_host_age,
    usbredir_host_revision)

summary_info += {'libusbredirhost.so version': usbredir_host_so_version}

usbredir_host_sources = [
    'usbredirhost.c',
    'usbredirhost.h',
]

usbredir_host_map_file = meson.current_source_dir() / 'usbredirhost.map'
usbredir_host_link_args = [
    '-Wl,--version-script=@0@'.format(usbredir_host_map_file),
    '-Wl,--no-undefined',
]

libusb = 'libusb-1.0'
libusb_version = '>= 1.0.9'
libusb_required_version = '@0@ @1@'.format(libusb, libusb_version)
libusb = dependency(libusb, version: libusb_version)

usbredir_host_lib = library('usbredirhost',
    usbredir_host_sources,
    version : usbredir_host_so_version,
    install : true,
    include_directories: usbredir_include_root_dir,
    link_args : [usbredir_host_link_args],
    link_depends : usbredir_host_map_file,
    dependencies : [libusb, usbredir_parser_lib_dep])

usbredir_host_lib_dep = declare_dependency(
    link_with: usbredir_host_lib,
    include_directories: usbredir_include_root_dir,
    dependencies: [libusb, usbredir_parser_lib_dep])

headers = [
   'usbredirhost.h',
]

install_headers(headers)

# libusbredirhost.pc
pkgconfig = import('pkgconfig')
pkgconfig.generate(usbredir_host_lib,
                   name : 'libusbredirhost',
                   description : 'usbredirhost library',
                   requires : [libusb_required_version, 'usbredirparser-0.5'],
                   variables : 'exec_prefix=${prefix}')
