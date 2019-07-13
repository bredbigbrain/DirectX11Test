#pragma once
#include"stdafx.h"
#include<vector>
#include<stdint.h>

namespace BMP
{
	const uint16_t MB_HEX_16{ 0x424d };
	const uint32_t SRGB_HEX_32{ 0x73524742 };

#pragma pack(push, 1)
	struct FileHeader
	{
		uint16_t signature{ MB_HEX_16 };
		uint32_t fileSize{0};
		uint32_t _reserved{0};
		uint32_t dataOffset{0};
	};

	struct InfoHeader
	{
		uint32_t size{0};
		uint32_t width{0};
		uint32_t height{0};
		uint16_t planes{ 1 };
		uint16_t bitCount{ 24 };
		uint32_t compression{0};
		uint32_t imageSize{0};
		uint32_t xPixelsPreM{0};
		uint32_t yPixelsPreM{0};
		uint32_t colorsUsed{0};
		uint32_t colorsImportant{0};
	};

	struct ColorHeader
	{
		uint32_t rMask{ 0x00ff0000 };
		uint32_t gMask{ 0x0000ff00 };
		uint32_t bMask{ 0x000000ff };
		uint32_t aMask{ 0xff000000 };
		uint32_t colorSpace{ SRGB_HEX_32 };
		uint32_t unused[16]{0};
	};
#pragma pack(pop)

	class Image
	{
	public:
		char m_szFilePath[MAX_PATH]{0};
		std::vector<uint8_t> vecData;

	protected:
		FileHeader m_fileHeader;
		InfoHeader m_infoHeader;
		ColorHeader m_colorHeader;

	private:
		uint32_t m_nRowStride{0};
		short m_nChannels{0};

	public:
		Image();
		Image(const char* szPath);
		~Image();
		
		void SetPixel(unsigned int x, unsigned int y, short r, short g, short b, short a = 255);

		bool Read();
		bool Write();
		bool Init(int32_t nWidth, int32_t nHeight, bool bHasAlpha = true);

	private:
		bool CheckColorHeader(ColorHeader& colorHeader);
		void WriteHeaders(std::ofstream& of);
		void WriteHeadersAndData(std::ofstream& of);
		uint32_t MakeStrideAligned(uint32_t nAlignStride);
	};
}