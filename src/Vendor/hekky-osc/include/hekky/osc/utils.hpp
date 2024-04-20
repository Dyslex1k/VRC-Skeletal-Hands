#pragma once

#include <string>

namespace hekky {
	namespace osc {
		namespace utils {
			/// <summary>
			/// Returns the length of a string, rounded to the nearest 32 bytes, to conform with the OSC protocol.
			/// </summary>
			/// <param name="string"></param>
			/// <returns></returns>
			uint64_t GetAlignedStringLength(const std::string& string);
			/// <summary>
			/// Returns the length of a string, rounded to the nearest 32 bytes, to conform with the OSC protocol.
			/// </summary>
			/// <param name="string"></param>
			/// <returns></returns>
			uint64_t GetAlignedStringLength(const std::wstring& string);

			/// <summary>
			/// Returns whether the current system is using Big Endian or Little-Endian
			/// </summary>
			/// <returns>System Endianness</returns>
			bool IsLittleEndian();

			/// <summary>
			/// Swaps 4 bytes
			/// </summary>
			/// <param name="num">A 32-bit unsigned integer</param>
			/// <returns>The same 32-bit unsigned integer, with the inverse endianness</returns>
			uint32_t SwapInt32(uint32_t num);

			/// <summary>
			/// Swaps 8 bytes
			/// </summary>
			/// <param name="num">A 64-bit unsigned integer</param>
			/// <returns>The same 64-bit unsigned integer, with the inverse endianness</returns>
			uint64_t SwapInt64(uint64_t num);

			/// <summary>
			/// Swaps the order of bytes in a 32-bit floating point number
			/// </summary>
			/// <param name="num">A 32-bit floating point number</param>
			/// <returns>The same 32-bit floating point number, with the inverse endianness</returns>
			float SwapFloat32(float num);

			/// <summary>
			/// Swaps the order of bytes in a 64-bit floating point number
			/// </summary>
			/// <param name="num">A 64-bit floating point number</param>
			/// <returns>The same 64-bit floating point number, with the inverse endianness</returns>
			double SwapFloat64(double num);
		}
	}
}