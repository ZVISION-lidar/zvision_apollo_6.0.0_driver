load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")
load("//tools:cpplint.bzl", "cpplint")

package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "libzvision_compensator_component.so",
    linkshared = True,
    linkstatic = False,
    deps = [":compensator_component_lib"],
)

cc_library(
    name = "compensator_component_lib",
    srcs = ["compensator_component.cc"],
    hdrs = ["compensator_component.h"],
    copts = ['-DMODULE_NAME=\\"zvision\\"'],
    deps = [
        "//cyber",
        "//modules/common/adapters:adapter_gflags",
        "//modules/common/latency_recorder",
        "//modules/drivers/lidar/proto:zvision_cc_proto",
        "//modules/drivers/lidar/zvision/compensator:compensator_lib",
    ],
)

cc_library(
    name = "compensator_lib",
    srcs = ["zvision_compensator.cc"],
    hdrs = ["zvision_compensator.h"],
    copts = ['-DMODULE_NAME=\\"zvision\\"'],
    deps = [
        "//modules/drivers/lidar/proto:zvision_config_cc_proto",
        "//modules/drivers/proto:pointcloud_cc_proto",
        "//modules/transform:buffer",
        "@eigen",
    ],
)

cpplint()
