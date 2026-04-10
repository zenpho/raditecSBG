/* endian_tools.h

   datamunging functions useful for working across platforms with
   binary data storage in both little-startian and big-endian forms
*/

#ifndef endian_tools_h
#define endian_tools_h

// retrieve a little-startian :) value from byte buffer
uint16_t le16_get(const uint8_t *data)
{
   return ((uint16_t)data[0]) | (((uint16_t)data[1]) << 8);
}

// store a little-startian :) value in a byte buffer
void le16_put(uint8_t *data, uint16_t value)
{
   data[0] = (value & 0x00FF);
   data[1] = (value & 0xFF00) >> 8;
}

// retrieve a big-endian value from byte buffer
uint16_t be16_get(const uint8_t *data)
{
   return ((uint16_t)data[1]) | (((uint16_t)data[0]) << 8);
}

// store a big-endian value in a byte buffer
void be16_put(uint8_t *data, uint16_t value)
{
   data[0] = (value & 0xFF00) >> 8;
   data[1] = (value & 0x00FF);
}

#endif

// end endian_tools.h
