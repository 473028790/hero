#include "crc.h"

uint8_t ref_get_crc8(uint8_t *p_msg, uint32_t len, uint8_t crc8)
{
    uint8_t uc_index;

    while (len--)
    {
        uc_index = crc8 ^ (*p_msg++);
        crc8  = ref_crc8_tab[uc_index];
    }

    return (crc8);
}


/*
**  Descriptions: CRC8 Verify function
**  Input:        Data to Verify,Stream length = Data + checksum
**  Output:       True or False (CRC Verify Result)
*/
uint32_t ref_verify_crc8(uint8_t *p_msg, uint32_t len)
{
    uint8_t uc_expected = 0;

    if ((p_msg == 0) || (len <= 2))
    {
        return 0;
    }

    uc_expected = ref_get_crc8(p_msg, len - 1, ref_crc8_init);

    return (uc_expected == p_msg[len - 1]);

}

/*
**  Descriptions: append CRC8 to the end of data
**  Input:        Data to CRC and append,Stream length = Data + checksum
**  Output:       True or False (CRC Verify Result)
*/
void ref_append_crc8(uint8_t *p_msg, uint32_t len)
{
    uint8_t crc8 = 0;

    if ((p_msg == 0) || (len <= 2))
    {
        return;
    }

    crc8 = ref_get_crc8((uint8_t *)p_msg, len - 1, ref_crc8_init);

    p_msg[len - 1] = crc8;
}
/*crc16*/





/*
**  Descriptions: CRC16 checksum function
**  Input:        Data to check,Stream length, initialized checksum
**  Output:       CRC checksum
*/
uint16_t ref_get_crc16(uint8_t *p_msg, uint16_t len, uint16_t crc16)
{
    uint8_t data;

    if (p_msg == NULL)
    {
        return 0xFFFF;
    }

    while (len--)
    {
        data = *p_msg++;
        (crc16) = ((uint16_t)(crc16) >> 8)  ^ ref_crc16_tab[((uint16_t)(crc16) ^ (uint16_t)(data)) & 0x00ff];
    }

    return crc16;
}

/*
**  Descriptions: CRC16 Verify function
**  Input:        Data to Verify,Stream length = Data + checksum
**  Output:       True or False (CRC Verify Result)
*/
uint32_t ref_verify_crc16(uint8_t *p_msg, uint32_t len)
{
    uint16_t w_expected = 0;

    if ((p_msg == NULL) || (len <= 2))
    {
        return 1;
    }
    w_expected = ref_get_crc16(p_msg, len - 2, ref_crc16_init);

    return ((w_expected & 0xff) == p_msg[len - 2] && ((w_expected >> 8) & 0xff) == p_msg[len - 1]);
}

/*
**  Descriptions: append CRC16 to the end of data
**  Input:        Data to CRC and append,Stream length = Data + checksum
**  Output:       True or False (CRC Verify Result)
*/
void ref_append_crc16(uint8_t *p_msg, uint16_t len)
{
    uint16_t crc16 = 0;

    if ((p_msg == NULL) || (len <= 2))
    {
        return;
    }
    crc16 = ref_get_crc16((uint8_t *)p_msg, len - 2, ref_crc16_init);

    p_msg[len - 2] = (uint8_t)(crc16 & 0x00ff);
    p_msg[len - 1] = (uint8_t)((crc16 >> 8) & 0x00ff);
}
