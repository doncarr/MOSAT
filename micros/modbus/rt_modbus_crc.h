

void add_CRC(uint8_t *buf, int size, uint16_t start_mask);
int check_CRC(uint8_t *buf, int size, uint16_t start_mask);
uint16_t CRC_16(uint8_t *buf, int n);
void swap16(uint16_t *word);
int hexascii_to_binary(uint8_t bin_buf[], uint8_t hex_buf[], int n_hex);
int binary_to_hexascii(uint8_t hex_buf[], uint8_t bin_buf[], int n_bin);


