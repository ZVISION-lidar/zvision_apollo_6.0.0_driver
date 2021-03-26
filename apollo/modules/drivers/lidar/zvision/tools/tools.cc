/******************************************************************************
 * Copyright 2020 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#include "tcp_client.h"
#include "tools.h"
#include <set>
#include <math.h>
#include <cstring>
#include <iostream>
#include <functional>
#include <fstream>
#include <string>
#include <sstream>
#include <thread>

#include "modules/drivers/lidar/zvision/tools/tcp_client.h"
#include "modules/drivers/lidar/proto/zvision.pb.h"

namespace apollo {
namespace drivers {
namespace zvision {


    bool LidarTools::CheckDeviceRet(std::string ret)
    {
        return (0x00 == ret[2]) && (0x00 == ret[3]);
    }

    int LidarTools::GetOnlineCalibrationData(std::string ip, CalibrationData& cal)
    {
        //std::string ec;
        //const int ppf = 256000; // points per frame, 256000 reserved
        const int ppk = 128; // points per cal udp packet
        //std::unique_ptr<float> angle_data(new float[ppf * 2]); // points( azimuh, elevation);
        //int packet_buffer_size = 1040 * (ppf / ppk) + 4; // 128 points in one packet, buffer reserved for ppf points.
        //std::unique_ptr<unsigned char> packet_data(new unsigned char[packet_buffer_size]);
        const int send_len = 4;
        char cal_cmd[send_len] = { (char)0xBA, (char)0x07, (char)0x00, (char)0x00 };
        std::string cmd(cal_cmd, send_len);

        const int recv_len = 4;
        std::string recv(recv_len, 'x');

        cal.model = UNKNOWN;
        TcpClient client(1000, 1000, 1000);
        int ret = client.Connect(ip);
        if (ret)
        {
            AERROR << "Connect error: " << client.GetSysErrorCode();
            return -1;
        }


        if (client.SyncSend(cmd, send_len))
        {
            client.Close();
            return -1;
        }

        if (client.SyncRecv(recv, recv_len))
        {
            client.Close();
            return -1;
        }

        if (!CheckDeviceRet(recv))
        {
            client.Close();
            return -1;
        }

        const int cal_pkt_len = 1040;
        std::string cal_recv(cal_pkt_len, 'x');

        //receive first packet to identify the device type
        if (client.SyncRecv(cal_recv, cal_pkt_len))
        {
            client.Close();
            return -1;
        }

        int total_packet = 0;

        std::string dev_code((char *)cal_recv.c_str() + 3, 6);
        uint32_t data_size = 0;
        if (0 == dev_code.compare("30_B1 "))
        {
            total_packet = 235;
            //data_size = (10000 * 3 * 2);
            data_size = ppk * total_packet * 2;// packet must give a full
            cal.model = ML30B1;
        }
        else if (0 == dev_code.compare("30S_A1"))
        {
            total_packet = 400;
            data_size = (6400 * 8 * 2);
            cal.model = ML30SA1;
        }
        else
        {
            AERROR << "Calibration packet identify error";
            client.Close();
            return -1;
        }

        //check the data
        unsigned char* check_data = (unsigned char *)cal_recv.c_str();
        unsigned char check_all_00 = 0x00;
        unsigned char check_all_ff = 0xFF;
        for (int i = 0; i < 1040 - 16; i++)
        {
            check_all_00 |= check_data[i];
            check_all_ff &= check_data[i];
        }
        if (0x00 == check_all_00)
        {
            AERROR << ("Check calibration data error, data is all 0x00.\n");
            client.Close();
            return -1;
        }
        if (0xFF == check_all_ff)
        {
            AERROR << ("Check calibration data error, data is all 0xFF.\n");
            client.Close();
            return -1;
        }
        //printf("Check data ok.");
        std::vector<float>& data = cal.data;
        {
            int network_data = 0;
            int host_data = 0;
            float* pfloat_data = reinterpret_cast<float *>(&host_data);
            for (int i = 0; i < 128 * 2; i++)
            {
                memcpy(&network_data, check_data + i * 4 + 16, 4); // 4 bytes per data, azimuth, elevation, 16 bytes header
                host_data = ntohl(network_data);
                data.push_back(*pfloat_data);
            }
        }


        for (int i = 0; i < total_packet - 1; i++)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(110));
            int ret = client.SyncRecv(cal_recv, cal_pkt_len);
            if (ret)
            {
                AERROR << ("Receive calibration data error\n");
                client.Close();
                return -1;
            }
            check_data = (unsigned char *)cal_recv.c_str();
            {
                int network_data = 0;
                int host_data = 0;
                float* pfloat_data = reinterpret_cast<float *>(&host_data);
                for (int i = 0; i < 128 * 2; i++)
                {
                    memcpy(&network_data, check_data + i * 4 + 16, 4); // 4 bytes per data, azimuth, elevation, 16 bytes header
                    host_data = ntohl(network_data);
                    data.push_back(*pfloat_data);
                }
            }
        }

        if (client.SyncRecv(recv, recv_len))
        {
            printf("Recv ack error.\n");
            client.Close();
            return -1;
        }
        //printf("recv ack ok.\n");
        if (!CheckDeviceRet(recv))
        {
            printf("Check ack error.\n");
            client.Close();
            return -1;
        }

        if(data_size != data.size())
        {
            printf("Calbration data size [%6lu] is not valid, [%6u] wanted.\n", data.size(), data_size);
            return -1;
        }
        //printf("check ack ok, cal type is %d, first data is %.3f %.3f.\n", cal.model, data[0], cal.data[1]);
        AINFO << ("Get calibration data ok.\n");
        return 0;
    }

    int LidarTools::ReadCalibrationFile(std::string filename, CalibrationData& cal)
    {
        std::ifstream file;
        file.open(filename, std::ios::in);
        std::string line;
        int ret = 0;
        cal.model = UNKNOWN;
        if (file.is_open())
        {
            std::vector<std::vector<std::string>> lines;
            while (std::getline(file, line))
            {
                if (line.size() > 0)
                {
                    std::istringstream iss(line);
                    std::vector<std::string> datas;
                    std::string data;
                    while (iss >> data)
                    {
                        datas.push_back(data);
                    }
                    lines.push_back(datas);
                }
            }
            file.close();
            if (10000 == lines.size())
            {
                cal.data.resize(60000);
                for (int i = 0; i < 10000; i++)
                {
                    const int column = 7;

                    std::vector<std::string>& datas = lines[i];
                    if (datas.size() != column)
                    {
                        ret = -1;
                        AERROR << "Resolve calibration file data error.";
                        break;
                    }
                    for (int j = 1; j < column; j++)
                    {
                        int fov = (j - 1) % 3;
                        if (0 == ((j - 1) / 3))//azimuth
                        {
                            cal.data[i * 6 + fov * 2] = static_cast<float>(std::atof(datas[j].c_str()));
                        }
                        else//elevation
                        {
                            cal.data[i * 6 + fov * 2 + 1] = static_cast<float>(std::atof(datas[j].c_str()));
                        }
                    }
                }
                cal.model = ML30B1;
            }
            else if (6400 == lines.size())
            {
                cal.data.resize(6400 * 8 * 2);
                for (int i = 0; i < 6400; i++)
                {
                    const int column = 17;

                    std::vector<std::string>& datas = lines[i];
                    if (datas.size() != column)
                    {
                        ret = -1;
                        AERROR << "Resolve calibration file data error.";
                        break;
                    }
                    for (int j = 1; j < column; j++)
                    {
                        cal.data[i * 16 + j - 1] = static_cast<float>(std::atof(datas[j].c_str()));
                    }
                }
                cal.model = ML30SA1;
            }
            else if (32000 == lines.size())
            {
                cal.data.resize(32000 * 3 * 2);
                for (int i = 0; i < 32000; i++)
                {
                    const int column = 7;

                    std::vector<std::string>& datas = lines[i];
                    if (datas.size() != column)
                    {
                        ret = -1;
                        AERROR << "Resolve calibration file data error.";
                        break;
                    }
                    for (int j = 1; j < column; j++)
                    {
                        cal.data[i * 6 + j - 1] = static_cast<float>(std::atof(datas[j].c_str()));
                    }
                }
                cal.model = MLX;
            }
            else
            {
                cal.model = UNKNOWN;
                ret = -1;
                AERROR << "Invalid calibration file length.";
            }

            return ret;
        }
        else
        {
            AERROR << "Open calibration file error.";
            return -1;
        }
    }

    void LidarTools::ComputeCalibrationData(CalibrationData& cal, PointCalibrationTable& cal_lut)
    {
        cal_lut.data.resize(cal.data.size() / 2);
        cal_lut.model = cal.model;
        if (ML30B1 == cal.model)
        {
            for (unsigned int i = 0; i < cal.data.size() / 2; ++i)
            {
                float azi = static_cast<float>(cal.data[i * 2] / 180.0 * 3.1416);
                float ele = static_cast<float>(cal.data[i * 2 + 1] / 180.0 * 3.1416);

                PointCalibrationData& point_cal = cal_lut.data[i];
                point_cal.ele = ele;
                point_cal.azi = azi;
                point_cal.cos_ele = std::cos(ele);
                point_cal.sin_ele = std::sin(ele);
                point_cal.cos_azi = std::cos(azi);
                point_cal.sin_azi = std::sin(azi);
            }
        }
        else if (ML30SA1 == cal.model)
        {
            const int start = 8;
            int fov_index[start] = { 0, 6, 1, 7, 2, 4, 3, 5 };
            for (unsigned int i = 0; i < cal.data.size() / 2; ++i)
            {
                int start_number = i % start;
                int group_number = i / start;
                int point_numer = group_number * start + fov_index[start_number];
                float azi = static_cast<float>(cal.data[point_numer * 2] / 180.0 * 3.1416);
                float ele = static_cast<float>(cal.data[point_numer * 2 + 1] / 180.0 * 3.1416);

                PointCalibrationData& point_cal = cal_lut.data[i];
                point_cal.ele = ele;
                point_cal.azi = azi;
                point_cal.cos_ele = std::cos(ele);
                point_cal.sin_ele = std::sin(ele);
                point_cal.cos_azi = std::cos(azi);
                point_cal.sin_azi = std::sin(azi);

            }
        }
        else if (MLX == cal.model)
        {
            const int start = 3;
            int fov_index[start] = { 2, 1, 0 };
            for (unsigned int i = 0; i < cal.data.size() / 2; ++i)
            {
                int start_number = i % start;
                int group_number = i / start;
                int point_numer = group_number * start + fov_index[start_number];
                float azi = static_cast<float>(cal.data[point_numer * 2] / 180.0 * 3.1416);
                float ele = static_cast<float>(cal.data[point_numer * 2 + 1] / 180.0 * 3.1416);

                PointCalibrationData& point_cal = cal_lut.data[i];
                point_cal.ele = ele;
                point_cal.azi = azi;
                point_cal.cos_ele = std::cos(ele);
                point_cal.sin_ele = std::sin(ele);
                point_cal.cos_azi = std::cos(azi);
                point_cal.sin_azi = std::sin(azi);
            }
        }
        else
        {

        }
    }

}
}
}
