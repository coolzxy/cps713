// Justin Bull 500355958
// Jonathan Kwan 500342079

#ifndef HTPA_H
#define HTPA_H

#include <stdio.h>
#include <limits.h>

// file buffer in bytes
#define CHUNK 1024

// In bits
#define BLOCK_LEN 128
#define KEY_LEN 72
#define ROUND_KEY_LEN 64
#define BLOCK_HALF_LEN ((int) BLOCK_LEN / 2)
#define ROUND_KEY_HALF_LEN ((int) ROUND_KEY_LEN / 2)

#define BLOCK_BYTE_LEN ((int) BLOCK_LEN / CHAR_BIT)
#define KEY_BYTE_LEN ((int) KEY_LEN / CHAR_BIT)
#define ROUND_BYTE_KEY_LEN ((int) ROUND_KEY_LEN / CHAR_BIT)
#define BLOCK_BYTE_HALF_LEN ((int) BLOCK_HALF_LEN / CHAR_BIT)
#define ROUND_BYTE_KEY_HALF_LEN ((int) ROUND_BYTE_KEY_LEN / 2)

#define MODE_NOT_CHOSEN 0
#define MODE_HTPA 1
#define MODE_AES_CBC 2

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL 1
#endif

#ifndef DISABLE_STRING_PRINT
#define DISABLE_STRING_PRINT 0
#endif

const unsigned char sbox[256] = {
  0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
  0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
  0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
  0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
  0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
  0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
  0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
  0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
  0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
  0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
  0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
  0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
  0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
  0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
  0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
  0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};

const unsigned int pbox[64] = {
  57, 49, 41, 33, 25, 17,  9, 1,
  58, 50, 42, 34, 26, 18, 10, 2,
  59, 51, 43, 35, 27, 19, 11, 3,
  60, 52, 44, 36, 28, 20, 12, 4,
  64, 59, 48, 40, 32, 24, 16, 8,
  63, 55, 47, 39, 31, 23, 15, 7,
  62, 54, 46, 38, 30, 22, 14, 6,
  61, 53, 45, 37, 29, 21, 13, 5
};

typedef struct htpa_bytes_tag {
  int len;
  unsigned char *bytes;
} htpa_bytes;

typedef struct htpa_block_array_tag {
  int size;
  htpa_bytes **blocks;
} htpa_blocks_array;


htpa_bytes * htpa_algorithm(htpa_bytes *ciphertext, htpa_bytes *plaintext, htpa_bytes *key, int rounds); // performs the HTPA algorithm and returns encrypted bytes (in HTPA struct)

void printf_blocks_array(htpa_blocks_array *array_ptr);
void fprint_bytes_hex(FILE *stream, htpa_bytes *bytes);
void fprint_bytes_str(FILE *stream, htpa_bytes *bytes);
char * get_bytes_hex(htpa_bytes *bytes); // string of bytes represented hex, takes pointer to htpa_bytes struct
char * get_bytes_str(htpa_bytes *bytes); // string of bytes represented ASCII, takes pointer to htpa_bytes struct

int calc_bits(htpa_bytes *bytes); // Returns number of bits of byte array
int calc_blocks_for_bytes(htpa_bytes *bytes); // Returns number of blocks needed in a byte array

htpa_blocks_array * split_into_blocks(htpa_bytes *byte_stream); // breaks a byte array of any size into blocks (array of htpa_bytes)
void pad_bytes(htpa_bytes *byte_stream); // reallocates space of byte stream to fit modulo BLOCK_BYTE_LEN
void free_blocks_array(htpa_blocks_array *array); // Frees up memory of the blocks' byte arrays, the blocks themselves, and their array

unsigned char subbyte(unsigned char); // uses sbox[256] to substitute a byte
void htpa_round(htpa_bytes *block, unsigned char *key_schedule); // performs the HTPA iteration on the block
void htpa_final_round(htpa_bytes *block, unsigned char *key_schedule); // final iteration does not swap the block halves
void htpa_round_function(htpa_bytes *block_half, unsigned char *round_key); // performs the HTPA round function on the block_half
void htpa_compute_round_key(unsigned char *key); // computes the round key from the key schedule


// helper functions for permutation mapping...
int get_bit(unsigned char byte, int position);
void get_bits_on_bytes(int *bits, int *positions, unsigned char *bytes, int bits_len);
void set_bits_on_bytes(unsigned char *bytes, int *bits, int bits_len);

#define debug_print(level, fmt, ...) \
        do { if (DEBUG && level <= DEBUG_LEVEL) fprintf(stderr, "%s:%d:%s(): [DEBUG %i] " fmt, __FILE__, \
                                __LINE__, __func__, level, __VA_ARGS__); fflush(stderr); } while (0)


void print_help_message(char *name);
void print_version_message();

#define BYTETOBINARYPATTERN "%d%d%d%d%d%d%d%d"
#define BYTETOBINARY(byte)  \
  (byte & 0x80 ? 1 : 0), \
  (byte & 0x40 ? 1 : 0), \
  (byte & 0x20 ? 1 : 0), \
  (byte & 0x10 ? 1 : 0), \
  (byte & 0x08 ? 1 : 0), \
  (byte & 0x04 ? 1 : 0), \
  (byte & 0x02 ? 1 : 0), \
  (byte & 0x01 ? 1 : 0)

#endif /* HTPA_H */
