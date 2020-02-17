#pragma once
#include <cstdint>
#include <vector>
#include <iostream>
#include <algorithm>

namespace TarkovAPI
{
	class StashHelper
	{
	public:
		StashHelper(int32_t width, int32_t height) :
      		m_nWidth(width), m_nHeight(height)
		{
			m_pkVecGrids.resize((width * height) + 1);
		}
		~StashHelper()
		{
			Reset();
		}

		void Reset()
		{
			std::fill(m_pkVecGrids.begin(), m_pkVecGrids.end(), false);
		}

		int32_t GetWidth()
		{
			return m_nWidth;
		}
		int32_t GetHeight()
		{
			return m_nHeight;
		}
		int32_t GetSize()
		{
			return m_nWidth * m_nHeight;
		}

		void Dump()
		{
			std::cout << "Grid " << m_nWidth << "x" << m_nHeight << " Information" << std::endl;

			for (auto row = 0; row < m_nHeight; ++row)
			{
				for (auto col = 0; col < m_nWidth; ++col)
				{       
					if (m_pkVecGrids[row * m_nWidth + col])
						std::cout << "\tX";
					else
						std::cout << "\tO";
				}
				std::cout << std::endl;
			}
		}

		bool IsEmpty(int32_t stash_pos, int32_t item_width, int32_t item_height)
		{
			return false;        
		}

		int32_t FindBlank(int32_t item_width, int32_t item_height)
		{
			return -1;
		}

		bool Put(int32_t stash_pos, int32_t item_width, int32_t item_height)
		{
			stash_pos--;

			auto xs = stash_pos % m_nWidth;
			auto ys = stash_pos / m_nWidth;

			for(auto x = xs; x < xs + item_width;x++)
			{
				for(auto y = ys; y < ys + item_height;y++)
				{
					m_pkVecGrids[y * m_nWidth + x] = true;
				}
			}
			return true;
		}

		void Clear(int32_t stash_pos, int32_t item_width, int32_t item_height)
		{
		}

	private:
		std::vector <bool> m_pkVecGrids;
    	int32_t m_nWidth, m_nHeight;
	};
}