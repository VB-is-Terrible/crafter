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
    data = ["//data:recipes/import.yaml"],
)

cc_library(
    name = "graph",
    hdrs = ["graph.h", "graph.tpp"],
    deps = [],
)

cc_binary(
    name = "g",
    srcs = ["graph-test.cpp"],
    deps = [":graph"],
    data = [],
)

cc_binary(
    name = "test",
    srcs = ["test.cpp"],
    deps = ["//yaml-cpp:yaml-cpp"],
    data = ["test.yaml"],
)

cc_binary(
    name = "client",
    srcs = ["graph-construct.cpp"],
    deps = [":graph", ":importer"],
    data = ["//data:recipes"],
    linkopts = ['-lstdc++fs'],
)
