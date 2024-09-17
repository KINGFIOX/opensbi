# OpenSBI Firmware with Payload (FW_PAYLOAD)

OpenSBI **firmware with Payload (FW_PAYLOAD)** is a firmware which directly
includes the binary for the booting stage to follow the OpenSBI firmware
execution. Typically, this payload will be a bootloader or an OS kernel.

A _FW_PAYLOAD_ firmware is particularly useful when the booting stage executed
prior to the OpenSBI firmware is not capable of loading both the OpenSBI
firmware and the booting stage to follow OpenSBI firmware.

A _FW_PAYLOAD_ firmware is also useful for cases where the booting stage prior
to the OpenSBI firmware does not pass a _flattened device tree (FDT file)_. In
such a case, a _FW_PAYLOAD_ firmware allows embedding a flattened device tree
in the .rodata section of the final firmware.

## Enabling _FW_PAYLOAD_ compilation

The _FW_PAYLOAD_ firmware can be enabled by any of the following methods:

1. Specifying `FW_PAYLOAD=y` on the top level `make` command line.
2. Specifying `FW_PAYLOAD=y` in the target platform _objects.mk_ configuration
   file.

The compiled _FW_PAYLOAD_ firmware ELF file is named _fw_payload.elf_. Its
expanded image file is _fw_payload.bin_. Both files are created in the
platform-specific build directory under the
_build/platform/<platform_subdir>/firmware_ directory.

## Configuration Options

A _FW_PAYLOAD_ firmware is built according to configuration parameters and
options. These configuration parameters can be defined using either the top
level `make` command line or the target platform _objects.mk_ configuration
file. The parameters currently defined are as follows:

- **FW_PAYLOAD_OFFSET** - Offset from the opensbi load address where the payload
  binary will be linked in the final _FW_PAYLOAD_ firmware binary image. This
  configuration parameter is mandatory if _FW_PAYLOAD_ALIGN_ is not defined.
  Compilation errors will result from an incorrect definition of
  _FW_PAYLOAD_OFFSET_ or of _FW_PAYLOAD_ALIGN_, or if neither of these
  parameters are defined.

- **FW_PAYLOAD_ALIGN** - Address alignment constraint where the payload binary
  will be linked after the end of the base firmware binary in the final
  _FW_PAYLOAD_ firmware binary image. This configuration parameter is mandatory
  if _FW_PAYLOAD_OFFSET_ is not defined. If both _FW_PAYLOAD_OFFSET_ and
  _FW_PAYLOAD_ALIGN_ are defined, _FW_PAYLOAD_OFFSET_ is used and
  _FW_PAYLOAD_ALIGN_ is ignored.

- **FW_PAYLOAD_PATH** - Path to the image file of the next booting stage
  binary. If this option is not provided then a simple test payload is
  automatically generated and used as a payload. This test payload executes
  an infinite `while (1)` loop after printing a message on the platform console.

- **FW_PAYLOAD_FDT_ADDR** - Address where the FDT passed by the prior booting
  stage or specified by the _FW_FDT_PATH_ parameter and embedded in the
  _.rodata_ section will be placed before executing the next booting stage,
  that is, the payload firmware. If this option is not provided, then the
  firmware will pass the FDT address passed by the previous booting stage
  to the next booting stage.

- **FW_PAYLOAD_FDT_OFFSET** - Address offset from the opensbi load address where
  the FDT will be passed to the next booting stage. This offset is used as
  relocatable address of the FDT passed to the next booting stage. If
  _FW_PAYLOAD_FDT_ADDR_ is also defined, the firmware will prefer _FW_PAYLOAD_FDT_ADDR_.

## _FW_PAYLOAD_ Example

The _[qemu/virt]_ platforms illustrate how to configure and use a _FW_PAYLOAD_
firmware. Detailed information regarding these platforms can be found in the
platform documentation files.

[qemu/virt]: ../platform/qemu_virt.md
