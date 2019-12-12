cc_library(
    name = "importer",
    srcs = ["import.cpp"],
    deps = ["//yaml-cpp:yaml-cpp"],
    hdrs = ["import.h"],
)
cc_binary(
    name = "import",
    srcs = ["import-test.cpp"],
    deps = [":importer"],
    data = ["import.yaml"],
)
