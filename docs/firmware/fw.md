# OpenSBI Platform Firmwares

OpenSBI provides firmware builds for specific platforms. Different types of
firmwares are supported to deal with the differences between different platforms
early boot stage. All firmwares will execute the same initialization procedure
of the platform hardware according to the platform specific code as well as
OpenSBI generic library code. The supported firmwares type will differ in how
the arguments passed by the platform early boot stage are handled, as well as
how the boot stage following the firmware will be handled and executed.

The previous booting stage will pass information via the following registers
of RISC-V CPU:

- hartid via _a0_ register
- device tree blob address in memory via _a1_ register. The address must
  be aligned to 8 bytes.

OpenSBI currently supports three different types of firmwares.

## Firmware with Dynamic Information (_FW_DYNAMIC_)

The _FW_DYNAMIC_ firmware gets information about the next booting stage entry,
e.g. a bootloader or an OS kernel, from previous booting stage at runtime.
动态的含义就是: 加载: next get info from prev stage

A _FW_DYNAMIC_ firmware is particularly useful when the booting stage executed
prior to OpenSBI firmware is capable of loading both the OpenSBI firmware
and the booting stage binary to follow OpenSBI firmware.

## Firmware with Jump Address (_FW_JUMP_)

The _FW_JUMP_ firmware assumes a fixed address of the next booting stage
entry, e.g. a bootloader or an OS kernel, without directly including the
binary code for this next stage.

A _FW_JUMP_ firmware is particularly useful when the booting stage executed
prior to OpenSBI firmware is capable of loading both the OpenSBI firmware
and the booting stage binary to follow OpenSBI firmware.

## Firmware with Payload (_FW_PAYLOAD_)

The _FW_PAYLOAD_ firmware directly includes the binary code for the booting
stage to follow OpenSBI firmware execution. Typically, this payload will be a
bootloader or an OS kernel.

A _FW_PAYLOAD_ firmware is particularly useful when the booting stage executed
prior to OpenSBI firmware is not capable of loading both OpenSBI firmware and
the booting stage to follow OpenSBI firmware.

A _FW_PAYLOAD_ firmware is also useful for cases where the booting stage prior
to OpenSBI firmware does not pass a _flattened device tree (FDT file)_. In such
case, a _FW_PAYLOAD_ firmware allows embedding a flattened device tree in the
.rodata section of the final firmware.

## Firmware Configuration and Compilation

All firmware types support the following common compile time configuration
parameters:

- **FW_TEXT_START** - Defines the compile time address of the OpenSBI
  firmware. This configuration parameter is optional and assumed to be
  `0` if not specified.
- **FW_FDT_PATH** - Path to an external flattened device tree binary file to
  be embedded in the _.rodata_ section of the final firmware. If this option
  is not provided then the firmware will expect the FDT to be passed as an
  argument by the prior booting stage.
- **FW_FDT_PADDING** - Optional zero bytes padding to the embedded flattened
  device tree binary file specified by **FW_FDT_PATH** option.

Additionally, each firmware type as a set of type specific configuration
parameters. Detailed information for each firmware type can be found in the
following documents.

- _[FW_DYNAMIC]_: The _Firmware with Dynamic Information (FW_DYNAMIC)_ is
  described in more details in the file _fw_dynamic.md_.
- _[FW_JUMP]_: The _Firmware with Jump Address (FW_JUMP)_ is described in more
  details in the file _fw_jump.md_.
- _[FW_PAYLOAD]_: The _Firmware with Payload (FW_PAYLOAD)_ is described in more
  details in the file _fw_payload.md_.

[FW_DYNAMIC]: fw_dynamic.md
[FW_JUMP]: fw_jump.md
[FW_PAYLOAD]: fw_payload.md

## Providing different payloads to OpenSBI Firmware

OpenSBI firmware can accept various payloads using a compile time option.
Typically, these payloads refer to the next stage boot loader (e.g. U-Boot)
or operating system kernel images (e.g. Linux). By default, OpenSBI
automatically provides a test payload if no specific payload is specified
at compile time.

To specify a payload at compile time, the make variable _FW_PAYLOAD_PATH_ is
used.

```
make PLATFORM=<platform_subdir> FW_PAYLOAD_PATH=<payload path>
```

The instructions to build each payload is different and the details can
be found in the
_docs/firmware/payload\_<payload_name>.md_ files.

## Options for OpenSBI Firmware behaviors

An optional compile time flag FW_OPTIONS can be used to control the OpenSBI
firmware run-time behaviors.

```
make PLATFORM=<platform_subdir> FW_OPTIONS=<options>
```

FW*OPTIONS is a bitwise or'ed value of various options, eg: \_FW_OPTIONS=0x1*
stands for disabling boot prints from the OpenSBI library.

For all supported options, please check "enum sbi*scratch_options" in the
\_include/sbi/sbi_scratch.h* header file.
