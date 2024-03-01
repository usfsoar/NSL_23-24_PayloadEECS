#include <EEPROM.h>
#include "deploy_module_backup.ino"

#define EEPROM_SIZE 20 // allocate max memory - this is 100 byte

int EEPROM_ADDRESS = 0;
byte value;

/* // this is the CRC algorithm to check whether data has changed or become corrupted
 unsigned long eeprom_crc(void)
{

    const unsigned long crc_table[16] = {

        0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,

        0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,

        0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,

        0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c

    };

    unsigned long crc = ~0L;

    for (int index = 0; index < EEPROM.length(); ++index)
    {

        crc = crc_table[(crc ^ EEPROM[index]) & 0x0f] ^ (crc >> 4);

        crc = crc_table[(crc ^ (EEPROM[index] >> 4)) & 0x0f] ^ (crc >> 4);

        crc = ~crc;
    }

    return crc;
}*/

void setup()
{
    EEPROM.begin(EEPROM_SIZE);

    EEPROM.write(EEPROM_ADDRESS, 1); // write data to EEPROM
                                     // address is the current address in EEPROM, which byte we are going write next
}

void loop()
{
    value = EEPROM.read(0); // read at address 0

    EEPROM_ADDRESS = EEPROM_ADDRESS + 1;
    if (EEPROM_ADDRESS == EEPRROM.length())
    {
        EEPROM_ADDRESS = 0;
    }

    delay(100);
}