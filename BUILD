# Bazel rules for ZHM5Randomizer for Hitman 2 (2018).

load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "DINPUT8.dll",
    srcs = glob([
        "**/*.cpp",
        "**/*.h",
        "**/*.hpp",
    ]),
    copts = ["/DCOMPILING_DLL"],
    linkopts = [
        "-DEFAULTLIB:user32",
        "-DEFAULTLIB:kernel32",
        "-DEFAULTLIB:user32",
        "-DEFAULTLIB:gdi32",
        "-DEFAULTLIB:winspool",
        "-DEFAULTLIB:shell32",
        "-DEFAULTLIB:ole32",
        "-DEFAULTLIB:oleaut32",
        "-DEFAULTLIB:uuid",
        "-DEFAULTLIB:comdlg32",
        "-DEFAULTLIB:advapi32",
    ],
    linkshared = 1,
    deps = [
        "//third_party/tomlpp",
        "@com_google_absl//absl/memory",
    ],
)
