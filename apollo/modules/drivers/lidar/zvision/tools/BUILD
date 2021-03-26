load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")
load("//tools:cpplint.bzl", "cpplint")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "tools",
    srcs = [
        "tcp_client.cc",
        "tools.cc"
    ],
    hdrs = [
        "tcp_client.h",
        "tools.h"
    ],
    copts = ['-DMODULE_NAME=\\"zvision\\"'],
    deps = [
        "//cyber",
        "//modules/common/util",
        "//modules/drivers/lidar/proto:config"
    ],
)

cpplint()
