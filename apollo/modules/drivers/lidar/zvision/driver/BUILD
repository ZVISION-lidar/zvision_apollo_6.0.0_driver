load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")
load("//tools:cpplint.bzl", "cpplint")

package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "libzvision_driver_component.so",
    linkshared = True,
    linkstatic = False,
    deps = [":zvision_driver_component_lib"],
)

cc_library(
    name = "zvision_driver_component_lib",
    srcs = ["zvision_driver_component.cc"],
    hdrs = ["zvision_driver_component.h"],
    copts = ['-DMODULE_NAME=\\"zvision\\"'],
    deps = [
        "//cyber",
        "//modules/common/util:message_util",
        "//modules/drivers/lidar/proto:config",
        "//modules/drivers/lidar/zvision/driver",
    ],
)

cc_library(
    name = "driver",
    srcs = [
        "zvision_driver.cc",
        "socket_input.cc",
    ],
    hdrs = [
        "zvision_driver.h",
        "input.h",
        "socket_input.h",
    ],
    copts = ['-DMODULE_NAME=\\"zvision\\"'],
    deps = [
        "//cyber",
        "//modules/common/util:message_util",
        "//modules/drivers/lidar/common/driver_factory:driver_base",
        "//modules/drivers/lidar/proto:config",
        "//modules/drivers/lidar/proto:zvision_config_cc_proto",
    ],
)

cpplint()
