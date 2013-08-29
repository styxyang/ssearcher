#include "config.h"
#include "debug.h"
#include "magic.h"
#include "trie.h"

#define M_3GPP_FTYP        0x0000001466747970
#define M_MP4_FTYP         0x0000001866747970
#define M_3GPP2_FTYP       0x0000002066747970
#define M_ICO              0x00000100
#define M_VAX_COFF_EX      0x0178
#define M_TAR_ZIP_LZW      0x1F9D
#define M_TAR_ZIP_LZH      0x1FA0
#define M_PDF              0x25504446
#define M_PSD              0x38425053
#define M_BMP              0x424D
#define M_ISO9660          0x4344303031
#define M_GIF0             0x474946383761
#define M_GIF1             0x474946383961
#define M_MP3_V2           0x494433
#define M_TIF              0x49492A00
#define M_TIFF             0x4D4D002A
#define M_EXE              0x4D5A
#define M_OGG              0x4F676753
#define M_ZIP              0x504B0304
#define M_ZIP_EMPTY        0x504B0506
#define M_ZIP_SPANNED      0x504B0708
#define M_RAR15            0x526172211A0700
#define M_RAR50            0x526172211A070100
#define M_FLAC             0x664C6143
#define M_ELF              0x7F454C46
#define M_PNG              0x89504E470D0A1A0A
#define M_MACH_O_FAT_CLASS 0xCAFEBABE
#define M_MACH32_O_BE      0xCEFAEDFE
#define M_MACH64_O_BE      0xCFFAEDFE
#define M_MS_OFFICE        0xD0CF11E0
#define M_MACH32_O_LE      0xFEEDFACE
#define M_MACH64_O_LE      0xFEEDFACF
#define M_MP3_V01          0xFFFB

static uint8_t magic_index[][MENTRY_LEN + 1] = {
    {0x00, 0x00, 0x00, 0x14, 0x66, 0x74, 0x79, 0x70, 8},
    {0x00, 0x00, 0x00, 0x18, 0x66, 0x74, 0x79, 0x70, 8},
    {0x00, 0x00, 0x00, 0x20, 0x66, 0x74, 0x79, 0x70, 8},
    {0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 4},
    {0x01, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 2},
    {0x1F, 0x9D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 2},
    {0x1F, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 2},
    {0x25, 0x50, 0x44, 0x46, 0x00, 0x00, 0x00, 0x00, 4},
    {0x38, 0x42, 0x50, 0x53, 0x00, 0x00, 0x00, 0x00, 4},
    {0x42, 0x4D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 2},
    {0x43, 0x44, 0x30, 0x30, 0x31, 0x00, 0x00, 0x00, 5},
    {0x47, 0x49, 0x46, 0x38, 0x37, 0x61, 0x00, 0x00, 6},
    {0x47, 0x49, 0x46, 0x38, 0x39, 0x61, 0x00, 0x00, 6},
    {0x49, 0x44, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 3},
    {0x49, 0x49, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x00, 4},
    {0x4D, 0x4D, 0x00, 0x2A, 0x00, 0x00, 0x00, 0x00, 4},
    {0x4D, 0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 2},
    {0x4F, 0x67, 0x67, 0x53, 0x00, 0x00, 0x00, 0x00, 4},
    {0x50, 0x4B, 0x03, 0x04, 0x00, 0x00, 0x00, 0x00, 4},
    {0x50, 0x4B, 0x05, 0x06, 0x00, 0x00, 0x00, 0x00, 4},
    {0x50, 0x4B, 0x07, 0x08, 0x00, 0x00, 0x00, 0x00, 4},
    {0x52, 0x61, 0x72, 0x21, 0x1A, 0x07, 0x00, 0x00, 7},
    {0x52, 0x61, 0x72, 0x21, 0x1A, 0x07, 0x01, 0x00, 8},
    {0x66, 0x4C, 0x61, 0x43, 0x00, 0x00, 0x00, 0x00, 4},
    {0x7F, 0x45, 0x4C, 0x46, 0x00, 0x00, 0x00, 0x00, 4},
    {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 8},
    {0xCA, 0xFE, 0xBA, 0xBE, 0x00, 0x00, 0x00, 0x00, 4},
    {0xCE, 0xFA, 0xED, 0xFE, 0x00, 0x00, 0x00, 0x00, 4},
    {0xCF, 0xFA, 0xED, 0xFE, 0x00, 0x00, 0x00, 0x00, 4},
    {0xD0, 0xCF, 0x11, 0xE0, 0x00, 0x00, 0x00, 0x00, 4},
    {0xFE, 0xED, 0xFA, 0xCE, 0x00, 0x00, 0x00, 0x00, 4},
    {0xFE, 0xED, 0xFA, 0xCF, 0x00, 0x00, 0x00, 0x00, 4},
    {0xFF, 0xFB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 2}
};

#include <stdio.h>
#include <unistd.h>

static struct trie *tr;

void magic_init()
{
    int i;

    trie_init(&tr);
    for (i = 0; i < sizeof(magic_index) / (MENTRY_LEN + 1) / sizeof(uint8_t); ++i) {
        trie_insert(tr, &magic_index[i][0], magic_index[i][8]);
    }
}

void magic_fini()
{
    trie_destroy(tr);
}

/* bool magic_scan(uint8_t *buf, int len) */
bool magic_scan(int fd)
{
    uint8_t magic_buf[16];
    read(fd, magic_buf, sizeof(magic_buf));

    return trie_scan(tr, magic_buf, 16)?true:false;
}
