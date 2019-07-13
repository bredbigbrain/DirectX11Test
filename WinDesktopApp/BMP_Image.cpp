#include "BMP_Image.h"
#include<fstream>


namespace BMP
{
	Image::Image()
	{
	}

	Image::Image(const char* szPath)
	{
		strcpy_s(m_szFilePath, szPath);
	}

	Image::~Image()
	{
	}

	bool Image::Init(int32_t nWidth, int32_t nHeight, bool bHasAlpha /*= true*/)
	{
		if (nWidth < 0 || nHeight < 0)
			return false;

		m_infoHeader.width = nWidth;
		m_infoHeader.height = nHeight;

		if (bHasAlpha)
		{
			m_infoHeader.size = sizeof(InfoHeader) + sizeof(ColorHeader);
			m_fileHeader.dataOffset = m_infoHeader.size + sizeof(FileHeader);

			m_infoHeader.bitCount = 32;
			m_infoHeader.compression = 3;
			m_nRowStride = 4;
			vecData.resize(m_nRowStride * nHeight);
			m_fileHeader.fileSize = m_fileHeader.dataOffset + vecData.size();
		}
		else
		{
			m_infoHeader.size = sizeof(InfoHeader);
			m_fileHeader.dataOffset = m_infoHeader.size + sizeof(ColorHeader);
		
			m_infoHeader.bitCount = 24;
			m_infoHeader.compression = 0;
			m_nRowStride = nWidth * 3;
			vecData.resize(m_nRowStride * nHeight);
			auto nNewStride = MakeStrideAligned(4);
			m_fileHeader.fileSize = m_fileHeader.dataOffset + vecData.size() + m_infoHeader.height * (nNewStride - m_nRowStride);
		}

		m_nChannels = m_infoHeader.bitCount / 8;
	}

	void Image::SetPixel(unsigned int x, unsigned int y, short r, short g, short b, short a /*= 255*/)
	{
		if (x > m_infoHeader.width || y > m_infoHeader.height)
			return;

		unsigned int val1 = m_nChannels * (y * m_infoHeader.width + x) + 0;
		unsigned int val2 = m_nChannels * (y * m_infoHeader.width + x) + 1;
		unsigned int val3 = m_nChannels * (y * m_infoHeader.width + x) + 2;
		unsigned int val4 = m_nChannels * (y * m_infoHeader.width + x) + 3;
		if (val1 >= vecData.size() || val2 >= vecData.size() || val3 >= vecData.size() || val4 >= vecData.size())
			return;

		vecData[val1] = b;
		vecData[val2] = g;
		vecData[val3] = r;
		if(m_nChannels == 4)
			vecData[val4] = a;
	}

	bool Image::Read()
	{
		if (m_szFilePath[0] != 0)
		{
			std::ifstream inp(m_szFilePath, std::ios_base::binary);
			if (!inp)
				return false;

			inp.read((char*)&m_fileHeader, sizeof(m_fileHeader));
			if (m_fileHeader.signature != MB_HEX_16)
				return false;

			inp.read((char*)&m_infoHeader, sizeof(m_infoHeader));
			if (m_infoHeader.height < 0)
				return false;

			if (m_infoHeader.bitCount == 32)
			{
				if (m_infoHeader.size >= sizeof(InfoHeader) + sizeof(ColorHeader))
				{
					inp.read((char*)&m_colorHeader, sizeof(m_colorHeader));
					if (!CheckColorHeader(m_colorHeader))
						return false;
				}
				else
					return false;
			}

			inp.seekg(m_fileHeader.dataOffset, inp.beg);

			if (m_infoHeader.bitCount == 32)
			{
				m_infoHeader.size = sizeof(InfoHeader) + sizeof(ColorHeader);
				m_fileHeader.dataOffset = m_infoHeader.size + sizeof(FileHeader);
			}
			else
			{
				m_infoHeader.size = sizeof(InfoHeader);
				m_fileHeader.dataOffset = m_infoHeader.size + sizeof(ColorHeader);
			}

			vecData.resize(m_infoHeader.width * m_infoHeader.height * m_infoHeader.bitCount / 8);

			if (m_infoHeader.width % 4 == 0)
			{
				inp.read((char*)vecData.data(), vecData.size());
				m_fileHeader.fileSize = m_fileHeader.dataOffset + vecData.size();
			}
			else
			{
				m_nRowStride = m_infoHeader.width * m_infoHeader.bitCount / 8;
				auto nNewStride = MakeStrideAligned(4);
				std::vector<uint8_t> vecPaddingRow(nNewStride - m_nRowStride);

				for (size_t y = 0; y < m_infoHeader.height; ++y)
				{
					inp.read((char*)(vecData.data() + m_nRowStride * y), m_nRowStride);
					inp.read((char*)vecPaddingRow.data(), vecPaddingRow.size());
				}
				m_fileHeader.fileSize = m_fileHeader.dataOffset + vecData.size() + m_infoHeader.height * vecPaddingRow.size();
			}
			return true;
		}
		return false;
	}

	bool Image::CheckColorHeader(ColorHeader& colorHeader)
	{
		ColorHeader expectedHeader;
		if (expectedHeader.colorSpace != colorHeader.colorSpace)
			return false;	//unsupported color space
		else if (expectedHeader.rMask != colorHeader.rMask
			|| expectedHeader.gMask != colorHeader.gMask
			|| expectedHeader.bMask != colorHeader.bMask
			|| expectedHeader.aMask != colorHeader.aMask)
		{
			return false;	//unsupported color format
		}
		return true;
	}

	bool Image::Write()
	{
		std::ofstream of(m_szFilePath, std::ios_base::binary);
		if (!of)
			return false;

		if (m_infoHeader.bitCount == 32)
		{
			WriteHeadersAndData(of);
		}
		else if (m_infoHeader.bitCount == 24)
		{
			if (m_infoHeader.width % 4 == 0)
				WriteHeadersAndData(of);
			else
			{
				auto nNewStride = MakeStrideAligned(4);
				std::vector<uint8_t> vecPaddingRow(nNewStride - m_nRowStride);

				WriteHeaders(of);

				for (size_t y = 0; y < m_infoHeader.height; ++y)
				{
					of.write((const char*)vecData.data() + m_nRowStride * y, m_nRowStride);
					of.write((const char*)vecPaddingRow.data(), vecPaddingRow.size());
				}
			}
		}
		else
			return false;

		return true;
	}

	void Image::WriteHeaders(std::ofstream& of)
	{
		of.write((const char*)&m_fileHeader, sizeof(m_fileHeader));
		of.write((const char*)&m_infoHeader, sizeof(m_infoHeader));
		if (m_infoHeader.bitCount == 32)
			of.write((const char*)&m_colorHeader, sizeof(m_colorHeader));
	}

	void Image::WriteHeadersAndData(std::ofstream& of)
	{
		WriteHeaders(of);
		of.write((const char*)vecData.data(), vecData.size());
	}

	uint32_t Image::MakeStrideAligned(uint32_t nAlignStride)
	{
		uint32_t nNewStride = m_nRowStride;
		while (nNewStride % nAlignStride != 0)
		{
			++nNewStride;
		}
		return nNewStride;
	}
}