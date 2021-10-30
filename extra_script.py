Import("env")

#
# Dump build environment (for debug)
# print(env.Dump())
#

env.Append(
  CCFLAGS=[
    "-Os",  # optimize for size
    "-ffunction-sections",  # place each function in its own section
    "-fdata-sections",
    "-Wall",
    "-mthumb",
    "-mfloat-abi=hard",
    "-mfpu=fpv4-sp-d16",
    #"-mcpu=%s" % env.BoardConfig().get("build.cpu"),
    # "-nostdlib"
    ],

    LINKFLAGS=[
        "-Os",
        "-Wl,--gc-sections,--relax",
        "-mthumb",
        "-mfloat-abi=hard",
        "-mfpu=fpv4-sp-d16",
        #"-mcpu=%s" % env.BoardConfig().get("build.cpu"),
        #"--specs=nano.specs",
        #"--specs=nosys.specs"
    ],
)