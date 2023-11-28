# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(sensorConnect)

target_sources(app 
                PRIVATE 
                    src/main.c
                    src/scd4x_i2c.c
                    src/svm41_i2c.c
                    src/sensirion_i2c.c
                    src/sensirion_i2c_hal.c
                    src/sensirion_common.c)
