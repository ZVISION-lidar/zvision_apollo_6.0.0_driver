load("//tools/install:install.bzl", "install")

package(default_visibility = ["//visibility:public"])

install(
    name = "install",
    data = [
        ":runtime_data",
    ],
    targets = [
        "//modules/drivers/lidar/zvision/compensator:libzvision_compensator_component.so",
        "//modules/drivers/lidar/zvision/driver:libzvision_driver_component.so",
        # "//modules/drivers/lidar/zvision/fusion:libzvision_fusion_component.so",
        "//modules/drivers/lidar/zvision/parser:libzvision_convert_component.so",
    ],
)

filegroup(
    name = "runtime_data",
    srcs = glob([
        "conf/*.txt",
        "conf/*.conf",
        "dag/*.dag",
        "launch/*.launch",
        "params/*.yaml",
    ]),
)
