# Protocol design

## Frame structure

```
0    1  0x3B                 Start of frame (0x3A, 0x3B)
1    1  0x16                 Address (static)
2    1  0x2A                 Frame type
3    1  0x18                 Data length
.    .  ...
.    .  ...                  24 bytes data
.    .  ...
28   1  0x6F                 CRC
29   1  0x03                 CRC
30   1  0x0D                 End of frame
31   1  0x0A                 End of frame
```

## Frame types

```
BASEN_FRAME_TYPE_CELL_VOLTAGES_1_12 = 0x24;
BASEN_FRAME_TYPE_CELL_VOLTAGES_13_24 = 0x25;
BASEN_FRAME_TYPE_CELL_VOLTAGES_25_34 = 0x26;
BASEN_FRAME_TYPE_PROTECT_IC = 0x27;
BASEN_FRAME_TYPE_STATUS = 0x2A;
BASEN_FRAME_TYPE_GENERAL_INFO = 0x2B;
BASEN_FRAME_TYPE_SETTINGS = 0xE8;
BASEN_FRAME_TYPE_SETTINGS_ALTERNATIVE = 0xEA;
BASEN_FRAME_TYPE_BALANCING = 0xFE;
```


## Status frame

```
Request: 0x3B 0x16 0x2A 0x01 0x00 0x41 0x00 0x0D 0x0A
Response: 0x3B 0x16 0x2A 0x18 0x00 0x00 0x00 0x00 0xCE 0x61 0x00 0x00 0x12 0x14 0x19 0x19 0x63 0x23 0x00 0x00 0x80 0x80 0x00 0x00 0x08 0x02 0x00 0x00 0x6F 0x03 0x0D 0x0A

Byte Len Payload              Description                      Unit  Precision
 0    1  0x3B                 Start of frame
 1    1  0x16                 Address
 2    1  0x2A                 Frame type
 3    1  0x18                 Data length
 4    4  0x00 0x00 0x00 0x00  Current (without calibration)    A     0.001f (signed)
 8    4  0xCE 0x61 0x00 0x00  Total voltage                    V     0.001f
 12   1  0x12                 Temperature 1                    °C    1.0f
 13   1  0x14                 Temperature 2                    °C    1.0f
 14   1  0x19                 Temperature 3                    °C    1.0f
 15   1  0x19                 Temperature 4                    °C    1.0f
 16   4  0x63 0x23 0x00 0x00  Capacity remaining               Ah    0.001f
 20   1  0x80                 Charging states (Bitmask)
 21   1  0x80                 Discharging states (Bitmask)
 22   1  0x00                 Charging warnings (Bitmask)
 23   1  0x00                 Discharging warnings (Bitmask)
 24   1  0x08                 State of charge                  %     1.0f
 25   1  0x19                 Unused
 26   1  0x00                 Unused
 27   1  0x00                 Unused
 28   1  0x6F                 CRC
 29   1  0x03                 CRC
 30   1  0x0D                 End of frame
 31   1  0x0A                 End of frame
```


## General info frame

```
Request: 0x3A 0x16 0x2B 0x01 0x00 0x42 0x00 0x0D 0x0A
Response: 0x3A 0x16 0x2B 0x18 0xA0 0x86 0x01 0x00 0x00 0x64 0x00 0x00 0x91 0xA0 0x01 0x00 0x00 0x00 0x00 0x00 0x30 0x75 0x00 0x00 0x71 0x53 0x07 0x00 0x86 0x04 0x0D 0x0A

Byte Len Payload              Description                      Unit  Precision
 0    1  0x3B                 Start of frame
 1    1  0x16                 Address
 2    1  0x2B                 Frame type
 3    1  0x18                 Data length
 4    4  0xA0 0x86 0x01 0x00  Nominal capacity                 Ah    0.001f
 8    4  0x00 0x64 0x00 0x00  Nominal voltage                  V     0.001f
 12   4  0x91 0xA0 0x01 0x00  Real capacity                    Ah    0.001f
 16   1  0x00                 Unused
 17   1  0x00                 Unused
 18   1  0x00                 Unused
 19   1  0x00                 Unused
 20   1  0x30                 Unused
 21   1  0x75                 Unused
 22   2  0x00 0x00            Serial number
 24   2  0x71 0x53            Manufacturing date
 26   2  0x07 0x00            Charging cycles
 28   1  0x86                 CRC
 29   1  0x04                 CRC
 30   1  0x0D                 End of frame
 31   1  0x0A                 End of frame
```

## Cell voltages frame (3 chunks)

```
Request: 0x3a 0x16 0x24 0x01 0x00 0x3b 0x00 0x0d 0x0a
Response: 0x3a 0x16 0x24 0x18 0x96 0x0c 0x97 0x0c 0x98 0x0c 0x96 0x0c 0x96 0x0c 0x98 0x0c 0x98 0x0c 0x97 0x0c 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x6a 0x05 0x0d 0x0a

Byte Len Payload              Description                      Unit  Precision
 0    1  0x3B                 Start of frame
 1    1  0x16                 Address
 2    1  0x24                 Frame type
 3    1  0x18                 Data length
 4    2  0x96 0x0C            Cell voltage 1
 6    2  0x97 0x0C            Cell voltage 2
 8    2  0x98 0x0C            Cell voltage 3
 10   2  0x96 0x0C            Cell voltage 4
 12   2  0x96 0x0C            Cell voltage 5
 14   2  0x98 0x0C            Cell voltage 6
 16   2  0x98 0x0C            Cell voltage 7
 18   2  0x97 0x0C            Cell voltage 8
 20   2  0x00 0x00            Cell voltage 9
 22   1  0x00 0x00            Cell voltage 10
 24   1  0x00 0x00            Cell voltage 11
 26   1  0x00 0x00            Cell voltage 12
 28   1  0x6A                 CRC
 29   1  0x05                 CRC
 30   1  0x0D                 End of frame
 31   1  0x0A                 End of frame
```

## Balancing frame

```
Request: 0x3A 0x16 0xFE 0x01 0x00 0x15 0x01 0x0D 0x0A
Response: 0x3A 0x16 0xFE 0x13 0x01 0x75 0x08 0x34 0x80 0x80 0x00 0x00 0x80 0x00 0x00 0x00 0x00 0x00 0x00 0x02 0x76 0x53 0x61 0x85 0x04 0x0D 0x0A

Byte Len Payload              Description                      Unit  Precision
 0    1  0x3B                 Start of frame
 1    1  0x16                 Address
 2    1  0xFE                 Frame type
 3    1  0x13                 Data length
 4    1  0x01
 5    1  0x75
 6    1  0x08
 7    1  0x34
 8    1  0x80
 9    1  0x80
 10   1  0x00
 11   1  0x00
 12   1  0x80
 13   1  0x00
 14   1  0x00
 15   1  0x00
 16   1  0x00
 17   1  0x00
 18   1  0x00
 19   1  0x02
 20   1  0x76
 21   1  0x53
 22   1  0x61
 23   1  0x85                 CRC
 24   1  0x04                 CRC
 25   1  0x0D                 End of frame
 26   1  0x0A                 End of frame
```

## Protect IC frame

```
Request: 0x3A 0x16 0x26 ...
Response: ?

Byte Len Payload              Description                      Unit  Precision
 0    1  0x3B                 Start of frame
 1    1  0x16                 Address
 2    1  0x27                 Frame type
 3    1                       Data length
 4    1
 5    1
 6    1
 7    1
 8    1
 9    1
 10   1
 11   1
 12   1
 13   1                       Temperature 3 (if SOF is 0x3B)
 14   1
 15   1
 16   1
 17   1                       Temperature 4 (if SOF is 0x3B)
 18   1
 19   1
 20   1
 21   1
 22   1
 23   1  0x85                 CRC
 24   1  0x04                 CRC
 25   1  0x0D                 End of frame
 26   1  0x0A                 End of frame
```

## Checksum

Sum of bytes (`1...data_len+3`) excluding the start of frame.

```
uint16_t chksum_(const uint8_t data[], const uint16_t len) {
  uint16_t checksum = 0x00;
  for (uint16_t i = 0; i < len; i++) {
    checksum = checksum + data[i];
  }
  return checksum;
}
```
