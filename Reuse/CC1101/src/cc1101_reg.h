#ifndef __CC1101_REG_H__
#define __CC1101_REG_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
+------+-----------------------+------------------------------+
|      |         Write         |             Read             |
+------+-------------+---------+-------------+----------------+
|      | Single Byte |  Burst  | Single Byte |      Burst     |
+------+-------------+---------+-------------+----------------+
|      |    +0x00    |  +0x40  |    +0x80    |      +0xC0     |
+------+-------------+---------+-------------+----------------+
| 0x00 |                        IOCFG2                        |
+------+------------------------------------------------------+
| 0x01 |                        IOCFG1                        |
+------+------------------------------------------------------+
| 0x02 |                        IOCFG0                        |
+------+------------------------------------------------------+
| 0x03 |                        FIFOTHR                       |
+------+------------------------------------------------------+
| 0x04 |                         SYNC1                        |
+------+------------------------------------------------------+
| 0x05 |                         SYNC0                        |
+------+------------------------------------------------------+
| 0x06 |                        PKTLEN                        |
+------+------------------------------------------------------+
| 0x07 |                       PKTCTRL1                       |
+------+------------------------------------------------------+
| 0x08 |                       PKTCTRL0                       |
+------+------------------------------------------------------+
| 0x09 |                         ADDR                         |
+------+------------------------------------------------------+
| 0x0A |                        CHANNR                        |
+------+------------------------------------------------------+
| 0x0B |                        FSCTRL1                       |
+------+------------------------------------------------------+
| 0x0C |                        FSCTRL0                       |
+------+------------------------------------------------------+
| 0x0D |                         FREQ2                        |
+------+------------------------------------------------------+
| 0x0E |                         FREQ1                        |
+------+------------------------------------------------------+
| 0x0F |                         FREQ0                        |
+------+------------------------------------------------------+
| 0x10 |                        MDMCFG4                       |
+------+------------------------------------------------------+
| 0x11 |                        MDMCFG3                       |
+------+------------------------------------------------------+
| 0x12 |                        MDMCFG2                       |
+------+------------------------------------------------------+
| 0x13 |                        MDMCFG1                       |
+------+------------------------------------------------------+
| 0x14 |                        MDMCFG0                       |
+------+------------------------------------------------------+
| 0x15 |                        DEVIATN                       |
+------+------------------------------------------------------+
| 0x16 |                         MCSM2                        |
+------+------------------------------------------------------+
| 0x17 |                         MCSM1                        |
+------+------------------------------------------------------+
| 0x18 |                         MCSM0                        |
+------+------------------------------------------------------+
| 0x19 |                        FOCCFG                        |
+------+------------------------------------------------------+
| 0x1A |                         BSCFG                        |
+------+------------------------------------------------------+
| 0x1B |                       AGCCTRL2                       |
+------+------------------------------------------------------+
| 0x1C |                       AGCCTRL1                       |
+------+------------------------------------------------------+
| 0x1D |                       AGCCTRL0                       |
+------+------------------------------------------------------+
| 0x1E |                        WOREVT1                       |
+------+------------------------------------------------------+
| 0x1F |                        WOREVT0                       |
+------+------------------------------------------------------+
| 0x20 |                        WORCTRL                       |
+------+------------------------------------------------------+
| 0x21 |                        FREND1                        |
+------+------------------------------------------------------+
| 0x22 |                        FREND0                        |
+------+------------------------------------------------------+
| 0x23 |                        FSCAL3                        |
+------+------------------------------------------------------+
| 0x24 |                        FSCAL2                        |
+------+------------------------------------------------------+
| 0x25 |                        FSCAL1                        |
+------+------------------------------------------------------+
| 0x26 |                        FSCAL0                        |
+------+------------------------------------------------------+
| 0x27 |                        RCCTRL1                       |
+------+------------------------------------------------------+
| 0x28 |                        RCCTRL0                       |
+------+------------------------------------------------------+
| 0x29 |                        FSTEST                        |
+------+------------------------------------------------------+
| 0x2A |                         PTEST                        |
+------+------------------------------------------------------+
| 0x2B |                        AGCTEST                       |
+------+------------------------------------------------------+
| 0x2C |                         TEST2                        |
+------+------------------------------------------------------+
| 0x2D |                         TEST1                        |
+------+------------------------------------------------------+
| 0x2E |                         TEST0                        |
+------+------------------------------------------------------+
| 0x2F |                                                      |
+------+-------------+---------+-------------+----------------+
| 0x30 | SRES        |         | SRES        | PARTNUM        |
+------+-------------+---------+-------------+----------------+
| 0x31 | SFSTXON     |         | SFSTXON     | VERSION        |
+------+-------------+---------+-------------+----------------+
| 0x32 | SXOFF       |         | SXOFF       | FREQEST        |
+------+-------------+---------+-------------+----------------+
| 0x33 | SCAL        |         | SCAL        | LQI            |
+------+-------------+---------+-------------+----------------+
| 0x34 | SRX         |         | SRX         | RSSI           |
+------+-------------+---------+-------------+----------------+
| 0x35 | STX         |         | STX         | MARCSTATE      |
+------+-------------+---------+-------------+----------------+
| 0x36 | SIDLE       |         | SIDLE       | WORTIME1       |
+------+-------------+---------+-------------+----------------+
| 0x37 |             |         |             | WORTIME0       |
+------+-------------+---------+-------------+----------------+
| 0x38 | SWOR        |         | SWOR        | PKTSTATUS      |
+------+-------------+---------+-------------+----------------+
| 0x39 | SPWD        |         | SPWD        | VCO_VC_DAC     |
+------+-------------+---------+-------------+----------------+
| 0x3A | SFRX        |         | SFRX        | TXBYTES        |
+------+-------------+---------+-------------+----------------+
| 0x3B | SFTX        |         | SFTX        | RXBYTES        |
+------+-------------+---------+-------------+----------------+
| 0x3C | SWORRST     |         | SWORRST     | RCCTRL1_STATUS |
+------+-------------+---------+-------------+----------------+
| 0x3D | SNOP        |         | SNOP        | RCCTRL0_STATUS |
+------+-------------+---------+-------------+----------------+
| 0x3E | PATABLE     | PATABLE | PATABLE     | PATABLE        |
+------+-------------+---------+-------------+----------------+
| 0x3F | TX FIFO     | TX FIFO | RX FIFO     | RX FIFO        |
+------+-------------+---------+-------------+----------------+
 */
#define CC1101_WRITE_SINGLE_BYTE 0x00
#define CC1101_WRITE_BURST       0x40
#define CC1101_READ_SINGLE_BYTE  0x80
#define CC1101_READ_BURST        0xC0

#define CC1101_BYTES_IN_RXFIFO   0x7F
#define CC1101_CRC_OK            0x80

#define CC1101_R_IOCFG2         0x00  //GDO2 output pin configuration
#define CC1101_R_IOCFG1         0x01  //GDO1 output pin configuration
#define CC1101_R_IOCFG0         0x02  //GDO0 output pin configuration
#define CC1101_R_FIFOTHR        0x03  //RX FIFO and TX FIFO thresholds
#define CC1101_R_SYNC1          0x04  //Sync word, high byte
#define CC1101_R_SYNC0          0x05  //Sync word, low byte
#define CC1101_R_PKTLEN         0x06  //Packet length
#define CC1101_R_PKTCTRL1       0x07  //Packet automation control
#define CC1101_R_PKTCTRL0       0x08  //Packet automation control
#define CC1101_R_ADDR           0x09  //Device address
#define CC1101_R_CHANNR         0x0A  //Channel number
#define CC1101_R_FSCTRL1        0x0B  //Frequency synthesizer control
#define CC1101_R_FSCTRL0        0x0C  //Frequency synthesizer control
#define CC1101_R_FREQ2          0x0D  //Frequency control word, high byte
#define CC1101_R_FREQ1          0x0E  //Frequency control word, middle byte
#define CC1101_R_FREQ0          0x0F  //Frequency control word, low byte
#define CC1101_R_MDMCFG4        0x10  //Modem configuration
#define CC1101_R_MDMCFG3        0x11  //Modem configuration
#define CC1101_R_MDMCFG2        0x12  //Modem configuration
#define CC1101_R_MDMCFG1        0x13  //Modem configuration
#define CC1101_R_MDMCFG0        0x14  //Modem configuration
#define CC1101_R_DEVIATN        0x15  //Modem deviation setting
#define CC1101_R_MCSM2          0x16  //Main Radio Control State Machine configuration
#define CC1101_R_MCSM1          0x17  //Main Radio Control State Machine configuration
#define CC1101_R_MCSM0          0x18  //Main Radio Control State Machine configuration
#define CC1101_R_FOCCFG         0x19  //Frequency Offset Compensation configuration
#define CC1101_R_BSCFG          0x1A  //Bit Synchronization configuration
#define CC1101_R_AGCTRL2        0x1B  //AGC control
#define CC1101_R_AGCTRL1        0x1C  //AGC control
#define CC1101_R_AGCTRL0        0x1D  //AGC control
#define CC1101_R_WOREVT1        0x1E  //High byte Event 0 timeout
#define CC1101_R_WOREVT0        0x1F  //Low byte Event 0 timeout
#define CC1101_R_WORCTRL        0x20  //Wake On Radio control
#define CC1101_R_FREND1         0x21  //Front end RX configuration
#define CC1101_R_FREND0         0x22  //Front end TX configuration
#define CC1101_R_FSCAL3         0x23  //Frequency synthesizer calibration
#define CC1101_R_FSCAL2         0x24  //Frequency synthesizer calibration
#define CC1101_R_FSCAL1         0x25  //Frequency synthesizer calibration
#define CC1101_R_FSCAL0         0x26  //Frequency synthesizer calibration
#define CC1101_R_RCCTRL1        0x27  //RC oscillator configuration
#define CC1101_R_RCCTRL0        0x28  //RC oscillator configuration
#define CC1101_R_FSTEST         0x29  //Frequency synthesizer calibration control
#define CC1101_R_PTEST          0x2A  //Production test
#define CC1101_R_AGCTEST        0x2B  //AGC test
#define CC1101_R_TEST2          0x2C  //Various test settings
#define CC1101_R_TEST1          0x2D  //Various test settings
#define CC1101_R_TEST0          0x2E  //Various test settings

// burst (page 70 in data sheet)
#define CC1101_R_PARTNUM        0x30  //Part number for CC1101
#define CC1101_R_VERSION        0x31  //Current version number
#define CC1101_R_FREQEST        0x32  //Frequency Offset Estimate
#define CC1101_R_LQI            0x33  //Demodulator estimate for Link Quality
#define CC1101_R_RSSI           0x34  //Received signal strength indication
#define CC1101_R_MARCSTATE      0x35  //Control state machine state
#define CC1101_R_WORTIME1       0x36  //High byte of WOR timer
#define CC1101_R_WORTIME0       0x37  //Low byte of WOR timer
#define CC1101_R_PKTSTATUS      0x38  //Current GDOx status and packet status
#define CC1101_R_VCO_VC_DAC     0x39  //Current setting from PLL calibration module
#define CC1101_R_TXBYTES        0x3A  //Underflow and number of bytes in the TX FIFO
#define CC1101_R_RXBYTES        0x3B  //Overflow and number of bytes in the RX FIFO
#define CC1101_R_RCCTRL1_STATUS 0x3C  //Last RC oscillator calibration result
#define CC1101_R_RCCTRL0_STATUS 0x3D  //Last RC oscillator calibration result

#define CC1101_R_PATABLE        0x3E
#define CC1101_R_TX_FIFO        0x3F
#define CC1101_R_RX_FIFO        0x3F

// Commands
#define CC1101_C_SRES      0x30
#define CC1101_C_SFSTXON   0x31
#define CC1101_C_SXOFF     0x32
#define CC1101_C_SCAL      0x33
#define CC1101_C_SRX       0x34
#define CC1101_C_STX       0x35
#define CC1101_C_SIDLE     0x36
#define CC1101_C_SWOR      0x38
#define CC1101_C_SPWD      0x39
#define CC1101_C_SFRX      0x3A
#define CC1101_C_SFTX      0x3B
#define CC1101_C_SWORRST   0x3C
#define CC1101_C_SNOP      0x3D
#define CC1101_C_PATABLE   0x3E
#define CC1101_C_TX_FIFO   0x3F

#ifdef __cplusplus
}
#endif
#endif /* __CC1101_REG_H__ */
