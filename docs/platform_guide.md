# OpenSBI Platform Support Guideline

The OpenSBI platform support allows an implementation to define a set of
platform-specific hooks (hardware manipulation functions) in the form of a
_struct sbi_platform_ data structure instance. This instance is required by
the platform-independent _libsbi.a_ to execute platform-specific operations.

Each of the reference platform supports provided by OpenSBI defines an instance
of the _struct sbi_platform_ data structure. For each supported platform,
_libplatsbi.a_ integrates this instance with _libsbi.a_ to create a
platform-specific OpenSBI static library. This library is installed
in _<install_directory>/platform/<platform_subdir>/lib/libplatsbi.a_

OpenSBI also provides implementation examples of bootable runtime firmwares for
the supported platforms. These firmwares are linked against _libplatsbi.a_.
Firmware binaries are installed in
_<install_directory>/platform/<platform_subdir>/bin_. These firmwares can be
used as executable runtime firmwares on the supported platforms as a replacement
for the legacy _riscv-pk_ boot loader (BBL).

A complete doxygen-style documentation of _struct sbi_platform_ and related
APIs is available in the file _include/sbi/sbi_platform.h_.

## Adding support for a new platform

Support for a new platform named _&lt;xyz&gt;_ can be added as follows:

1. Create a directory named _&lt;xyz&gt;_ under the _platform/_ directory.
2. Create platform configuration files named _Kconfig_ and _configs/defconfig_
   under the _platform/&lt;xyz&gt;/_ directory. These configuration files will
   provide the build time configuration for the sources to be compiled.
3. Create a _platform/&lt;xyz&gt;/objects.mk_ file for listing the platform
   object files to be compiled. This file also provides platform-specific
   compiler flags, and select firmware options.
4. Create a _platform/&lt;xyz&gt;/platform.c_ file providing a
   _struct sbi_platform_ instance.

A platform support code template is available under the _platform/template_
directory. Copying this directory and its content as a new directory named
_&lt;xyz&gt;_ under the _platform/_ directory will create all the files
mentioned above.
