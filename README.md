# 1 Introduction

Apollo-6.0.0 driver for zvision lidar.

# 2 Prerequisites
 Apollo version  required: [Apollo V6.0.0](https://github.com/ApolloAuto/apollo/releases/tag/v6.0.0)

# 3 Components
1. [Driver](https://github.com/ZVISION-lidar/zvision_apollo6.0.0_driver/tree/master/apollo/modules/drivers/lidar/zvision/driver): Driver receives UDP data packets from lidar sensor, and packages the data packets into a frame of scanning data in the format of ZvisionScan. ZvisionScan is defined in file below:
	```
    modules/drivers/lidar/proto/zvision.proto
	```
2. [Parser](https://github.com/ZVISION-lidar/zvision_apollo6.0.0_driver/tree/master/apollo/modules/drivers/lidar/zvision/parser): Parser takes one frame data in format of ZvisionScan as input, converts the cloud points in the frame from spherical coordinate system to Cartesian coordinates system, then sends out the point cloud as output. The pointcloud format is defined in file below:
	```
    modules/drivers/proto/pointcloud.proto
	```
3. [Compensator](https://github.com/ZVISION-lidar/zvision_apollo6.0.0_driver/tree/master/apollo/modules/drivers/lidar/zvision/compensator): Compensator takes pointcloud data and pose data as inputs. Based on the corresponding pose information for each cloud point, it converts each cloud point information aligned with the latest time in the current lidar scan frame, minimizing the motion error due the movement of the vehicle. Thus, each cloud point needs carry its own timestamp information.

**Notes:**   Compensator is not test yet, and will update soon.

# 4 Build
1. put /zvision_apollo_6.0.0_driver/apollo/modules/drivers/lidar/zvision folder into apollo/modules/drivers/lidar/

2. put /zvision_apollo_6.0.0_driver/apollo/modules/drivers/lidar/proto/zvision_config.proto & zvision.proto into apollo/modules/drivers/lidar/proto/

3. replace /apollo/modules/drivers/lidar/proto/BUILD & config.proto & lidar_parameter.proto with the files under zvision_apollo_6.0.0_driver/apollo/modules/drivers/lidar/proto/

4. replace /apollo/modules/drivers/lidar/BUILD with /zvision_apollo_6.0.0_driver/apollo/modules/drivers/lidar/BUILD


```bash
	cd /apollo
    bash apollo.sh build
 ```
   
# 5 Sample
You can run a sample dag file in the modules/drivers/zvision/dag directory. For reference, directory named [offline_calibration](https://github.com/ZVISION-lidar/zvision_apollo_driver/tree/master/apollo/modules/drivers/zvision/dag/offline_calibration) provides the samples which you can specify a local calibration file for the pointcloud, so [online_calibration](https://github.com/ZVISION-lidar/zvision_apollo_driver/tree/master/apollo/modules/drivers/zvision/dag/online_calibration) is.
1. Run zvision ML30 lidar with offline calibration.
   ```
    mainboard -d /apollo/modules/drivers/zvision/dag/offline_calibration/zvision_ml30.dag
    ```
2. Run zvision ML30SA1 lidar with online calibration.
    ```
    mainboard -d /apollo/modules/drivers/zvision/dag/online_calibration/zvision_ml30sa1.dag
    ```