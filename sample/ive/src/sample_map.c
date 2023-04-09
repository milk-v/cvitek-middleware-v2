#include "cvi_ive.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

int main(int argc, char **argv)
{
	if (argc != 4) {
		printf("Incorrect loop value. Usage: %s <w> <h> <file_name>\n",
		       argv[0]);
		printf("Example: %s 352 288 data/00_352x288_y.yuv\n", argv[0]);
		return CVI_FAILURE;
	}
	// 00_352x288_y.yuv
	const char *filename = argv[3];
	int ret = CVI_SUCCESS;
	int input_w, input_h;
	unsigned long elapsed_cpu;
	struct timeval t0, t1;

	input_w = atoi(argv[1]);
	input_h = atoi(argv[2]);
	gettimeofday(&t0, NULL);

	IVE_HANDLE handle = CVI_IVE_CreateHandle();

	// Create src image.
	IVE_IMAGE_S src;

	CVI_IVE_ReadRawImage(handle, &src, filename, IVE_IMAGE_TYPE_U8C1, input_w,
			  input_h);

	// Create dst image.
	IVE_DST_IMAGE_S dst_map;

	CVI_IVE_CreateImage(handle, &dst_map, IVE_IMAGE_TYPE_U8C1, input_w,
			    input_h);

	// Config Setting.
	IVE_DST_MEM_INFO_S dstTbl;
	CVI_U32 dstTblByteSize = 256;

	CVI_IVE_CreateMemInfo(handle, &dstTbl, dstTblByteSize);
	CVI_U8 FixMap[256] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x03,
		0x03, 0x04, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
		0x0C, 0x0D, 0x0F, 0x10, 0x11, 0x12, 0x14, 0x15, 0x17, 0x18,
		0x1A, 0x1B, 0x1D, 0x1E, 0x20, 0x21, 0x23, 0x24, 0x26, 0x27,
		0x29, 0x2A, 0x2C, 0x2D, 0x2F, 0x31, 0x32, 0x34, 0x35, 0x37,
		0x38, 0x3A, 0x3B, 0x3D, 0x3E, 0x40, 0x41, 0x43, 0x44, 0x45,
		0x47, 0x48, 0x4A, 0x4B, 0x4D, 0x4E, 0x50, 0x51, 0x52, 0x54,
		0x55, 0x56, 0x58, 0x59, 0x5A, 0x5B, 0x5D, 0x5E, 0x5F, 0x60,
		0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x69, 0x6A, 0x6B, 0x6D,
		0x6E, 0x70, 0x71, 0x73, 0x75, 0x76, 0x78, 0x7A, 0x7B, 0x7D,
		0x7E, 0x80, 0x81, 0x83, 0x84, 0x86, 0x87, 0x88, 0x89, 0x8B,
		0x8C, 0x8D, 0x8E, 0x90, 0x92, 0x94, 0x97, 0x9A, 0x9C, 0x9E,
		0xA1, 0xA3, 0xA5, 0xA6, 0xA7, 0xA9, 0xAA, 0xAB, 0xAC, 0xAC,
		0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB3, 0xB4, 0xB5, 0xB7, 0xB9,
		0xBB, 0xBD, 0xBF, 0xC1, 0xC4, 0xC7, 0xCC, 0xD1, 0xD5, 0xDA,
		0xDE, 0xE0, 0xE2, 0xE3, 0xE4, 0xE5, 0xE5, 0xE6, 0xE6, 0xE6,
		0xE6, 0xE6, 0xE7, 0xE7, 0xE7, 0xE8, 0xE8, 0xE9, 0xEA, 0xEC,
		0xED, 0xEE, 0xF0, 0xF2, 0xF4, 0xF5, 0xF7, 0xF8, 0xFA, 0xFB,
		0xFD, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
	};

	for (CVI_U32 i = 0; i < dstTblByteSize; i++) {
		((CVI_U8 *)(uintptr_t)dstTbl.u64VirAddr)[i] = FixMap[i];
	}
	IVE_MAP_CTRL_S Ctrl;

	Ctrl.enMode = IVE_MAP_MODE_U8;

	// Run IVE
	printf("Run HW IVE Map MODE_U8.\n");
	CVI_IVE_Map(handle, &src, &dstTbl, &dst_map, &Ctrl, 1);
	gettimeofday(&t1, NULL);
	elapsed_cpu =
		((t1.tv_sec - t0.tv_sec) * 1000000 + t1.tv_usec - t0.tv_usec);
	printf("%s CPU time %lu\n", __func__, elapsed_cpu);

	CVI_IVE_WriteImg(handle, "sample_Map.yuv", &dst_map);

	CVI_SYS_FreeI(handle, &src);
	CVI_SYS_FreeI(handle, &dst_map);
	CVI_SYS_FreeM(handle, &dstTbl);
	CVI_IVE_DestroyHandle(handle);

	return ret;
}
