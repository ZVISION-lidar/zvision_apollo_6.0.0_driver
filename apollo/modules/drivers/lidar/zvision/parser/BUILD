load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")
load("//tools:cpplint.bzl", "cpplint")

package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "libzvision_convert_component.so",
    linkshared = True,
    linkstatic = False,
    deps = [":zvision_convert_component_lib"],
)

cc_library(
    name = "zvision_convert_component_lib",
    srcs = ["zvision_convert_component.cc"],
    hdrs = ["zvision_convert_component.h"],
    copts = ['-DMODULE_NAME=\\"zvision\\"'],
    deps = [
        "//cyber",
        "//modules/drivers/lidar/zvision/parser:convert",
    ],
)

cc_library(
    name = "convert",
    srcs = [
        "convert.cc",
        "ml30_parser.cc",
        "ml30sa1_parser.cc",
        "mlx_parser.cc",
        "parser.cc",
    ],
    hdrs = [
        "convert.h",
        "parser.h",
    ],
    copts = ['-DMODULE_NAME=\\"zvision\\"'],
    deps = [
        "//cyber",
        "//modules/drivers/lidar/proto:config",
        "//modules/drivers/lidar/proto:zvision_config_cc_proto",
        "//modules/drivers/proto:pointcloud_cc_proto",
        "//modules/drivers/lidar/zvision/tools:tools",
        "@boost",
        "@eigen",
    ],
)

cpplint()
