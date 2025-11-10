/*
 * si4432_reg.h
 *
 *  Created on: Dec 24, 2022
 *      Author: mkpk
 */

#ifndef __SI4432_REG_H__
#define __SI4432_REG_H__

#define SI4432_WRITE 0x80
#define SI4432_READ  0x00

#define SI4432_R_DEVICE_TYPE                                0x00
#define SI4432_R_DEVICE_VERSION                             0x01
#define SI4432_R_DEVICE_STATUS                              0x02
#define SI4432_R_INTERRUPT_STATUS_1                         0x03
#define SI4432_R_INTERRUPT_STATUS_2                         0x04
#define SI4432_R_INTERRUPT_ENABLE_1                         0x05
#define SI4432_R_INTERRUPT_ENABLE_2                         0x06
#define SI4432_R_OPERATING_FUNCTION_CONTROL_1               0x07
#define SI4432_R_OPERATING_FUNCTION_CONTROL_2               0x08
#define SI4432_R_CRYSTAL_OSCILLATOR_LOAD_CAPACITANCE        0x09
#define SI4432_R_MICROCONTROLLER_OUTPUT_CLOCK               0x0A
#define SI4432_R_GPIO0_CONFIGURATION                        0x0B
#define SI4432_R_GPIO1_CONFIGURATION                        0x0C
#define SI4432_R_GPIO2_CONFIGURATION                        0x0D
#define SI4432_R_I                                          / O_PORT_CONFIGURATION 0x0E
#define SI4432_R_ADC_CONFIGURATION                          0x0F
#define SI4432_R_ADC_SENSOR_AMPLIFIER_OFFSET                0x10
#define SI4432_R_ADC_VALUE                                  0x11
#define SI4432_R_TEMPERATURE_SENSOR_CONTROL                 0x12
#define SI4432_R_TEMPERATURE_VALUE_OFFSET                   0x13
#define SI4432_R_WAKE                                       -UP_TIMER_PERIOD_1 0x14
#define SI4432_R_WAKE                                       -UP_TIMER_PERIOD_2 0x15
#define SI4432_R_WAKE                                       -UP_TIMER_PERIOD_3 0x16
#define SI4432_R_WAKE                                       -UP_TIMER_VALUE_1 0x17
#define SI4432_R_WAKE                                       -UP_TIMER_VALUE_2 0x18
#define SI4432_R_LOW                                        -DUTY_CYCLE_MODE_DURATION 0x19
#define SI4432_R_LOW_BATTERY_DETECTOR_THRESHOLD             0x1A
#define SI4432_R_BATTERY_VOLTAGE_LEVEL                      0x1B
#define SI4432_R_IF_FILTER_BANDWIDTH                        0x1C
#define SI4432_R_AFC_LOOP_GEARSHIFT_OVERRIDE                0x1D
#define SI4432_R_AFC_TIMING_CONTROL                         0x1E
#define SI4432_R_CLOCK_RECOVERY_GEARSHIFT_OVERRIDE          0x1F
#define SI4432_R_CLOCK_RECOVERY_OVERSAMPLING_RATIO          0x20
#define SI4432_R_CLOCK_RECOVERY_OFFSET_2                    0x21
#define SI4432_R_CLOCK_RECOVERY_OFFSET_1                    0x22
#define SI4432_R_CLOCK_RECOVERY_OFFSET_0                    0x23
#define SI4432_R_CLOCK_RECOVERY_TIMING_LOOP_GAIN_1          0x24
#define SI4432_R_CLOCK_RECOVERY_TIMING_LOOP_GAIN_0          0x25
#define SI4432_R_RECEIVED_SIGNAL_STRENGTH_INDICATOR         0x26
#define SI4432_R_RSSI_THRESHOLD_FOR_CLEAR_CHANNEL_INDICATOR 0x27
#define SI4432_R_ANTENNA_DIVERSITY_REGISTER_1               0x28
#define SI4432_R_ANTENNA_DIVERSITY_REGISTER_2               0x29
#define SI4432_R_AFC_LIMITER                                0x2A
#define SI4432_R_AFC_CORRECTION_READ                        0x2B
#define SI4432_R_OOK_COUNTER_VALUE_1                        0x2C
#define SI4432_R_OOK_COUNTER_VALUE_2                        0x2D
#define SI4432_R_SLICER_PEAK_HOLD                           0x2E
#define SI4432_R_DATA_ACCESS_CONTROL                        0x30
#define SI4432_R_EZMAC_STATUS                               0x31
#define SI4432_R_HEADER_CONTROL_1                           0x32
#define SI4432_R_HEADER_CONTROL_2                           0x33
#define SI4432_R_PREAMBLE_LENGTH                            0x34
#define SI4432_R_PREAMBLE_DETECTION_CONTROL                 0x35
#define SI4432_R_SYNC_WORD_3                                0x36
#define SI4432_R_SYNC_WORD_2                                0x37
#define SI4432_R_SYNC_WORD_1                                0x38
#define SI4432_R_SYNC_WORD_0                                0x39
#define SI4432_R_TRANSMIT_HEADER_3                          0x3A
#define SI4432_R_TRANSMIT_HEADER_2                          0x3B
#define SI4432_R_TRANSMIT_HEADER_1                          0x3C
#define SI4432_R_TRANSMIT_HEADER_0                          0x3D
#define SI4432_R_TRANSMIT_PACKET_LENGTH                     0x3E
#define SI4432_R_CHECK_HEADER_3                             0x3F
#define SI4432_R_CHECK_HEADER_2                             0x40
#define SI4432_R_CHECK_HEADER_1                             0x41
#define SI4432_R_CHECK_HEADER_0                             0x42
#define SI4432_R_HEADER_ENABLE_3                            0x43
#define SI4432_R_HEADER_ENABLE_2                            0x44
#define SI4432_R_HEADER_ENABLE_1                            0x45
#define SI4432_R_HEADER_ENABLE_0                            0x46
#define SI4432_R_RECEIVED_HEADER_3                          0x47
#define SI4432_R_RECEIVED_HEADER_2                          0x48
#define SI4432_R_RECEIVED_HEADER_1                          0x49
#define SI4432_R_RECEIVED_HEADER_0                          0x4A
#define SI4432_R_RECEIVED_PACKET_LENGTH                     0x4B
#define SI4432_R_ADC8_CONTROL                               0x4F

#define SI4432_R_ANALOG_TEST_BUS_SELECT       0x50
#define SI4432_R_DIGITAL_TEST_BUS_SELECT      0x51
#define SI4432_R_TX_RAMP_CONTROL              0x52
#define SI4432_R_PLL_TUNE_TIME                0x53
#define SI4432_R_CALIBRATION_CONTROL          0x55
#define SI4432_R_MODEM_TEST                   0x56
#define SI4432_R_CHARGE_PUMP_TEST             0x57
#define SI4432_R_CHARGE_PUMP_CURRENT_TRIMMING 0x58
#define SI4432_R_DIVIDER_CURRENT_TRIMMING     0x59
#define SI4432_R_VCO_CURRENT_TRIMMING         0x5A
#define SI4432_R_VCO_CALIBRATION              0x5B
#define SI4432_R_SYNTHESIZER_TEST             0x5C
#define SI4432_R_BLOCK_ENABLE_OVERRIDE1       0x5D
#define SI4432_R_BLOCK_ENABLE_OVERRIDE2       0x5E
#define SI4432_R_BLOCK_ENABLE_OVERRIDE3       0x5F

#define SI4432_R_CHANNEL_FILTER_COEFFICIENT_ADDRESS 0x60
#define SI4432_R_CRYSTAL_OSCILLATOR                 / CONTROL_TEST 0x62
#define SI4432_R_AGC_OVERRIDE_1                     0x69
#define SI4432_R_TX_POWER                           0x6D
#define SI4432_R_TX_DATA_RATE_1                     0x6E
#define SI4432_R_TX_DATA_RATE_0                     0x6F
#define SI4432_R_MODULATION_MODE_CONTROL_1          0x70
#define SI4432_R_MODULATION_MODE_CONTROL_2          0x71
#define SI4432_R_FREQUENCY_DEVIATION                0x72
#define SI4432_R_FREQUENCY_OFFSET_1                 0x73
#define SI4432_R_FREQUENCY_OFFSET_2                 0x74
#define SI4432_R_FREQUENCY_BAND_SELECT              0x75
#define SI4432_R_NOMINAL_CARRIER_FREQUENCY_1        0x76
#define SI4432_R_NOMINAL_CARRIER_FREQUENCY_0        0x77
#define SI4432_R_FREQUENCY_HOPPING_CHANNEL_SELECT   0x79
#define SI4432_R_FREQUENCY_HOPPING_STEP_SIZE        0x7A
#define SI4432_R_TX_FIFO_CONTROL_1                  0x7C
#define SI4432_R_TX_FIFO_CONTROL_2                  0x7D
#define SI4432_R_RX_FIFO_CONTROL                    0x7E
#define SI4432_R_FIFO_ACCESS                        0x7F

#endif /* __SI4432_REG_H__ */
