#include "hekky/osc/utils.hpp"

namespace hekky {
	namespace osc {
		namespace utils {
			uint64_t GetAlignedStringLength(const std::string& string) {
				uint64_t len = string.length() + (4 - string.length() % 4);
				if (len <= string.length()) len += 4;
				return len;
			}
			uint64_t GetAlignedStringLength(const std::wstring& string) {
				uint64_t len = string.length() + (4 - string.length() % 4);
				if (len <= string.length()) len += 4;
				return len;
			}

			bool IsLittleEndian() {
				union {
					uint32_t i;
					char c[4];
				} endianCheck = { 0x01020304 };

				return endianCheck.c[0] != 1;
			}

			uint32_t SwapInt32(uint32_t num) {
				return static_cast<std::uint32_t>((num << 24) | ((num << 8) & 0x00FF0000) | ((num >> 8) & 0x0000FF00) | (num >> 24));
			}

			float SwapFloat32(float num) {

				union {
					float f;
					uint32_t ui32;
				} swapper = { num };

				swapper.ui32 = SwapInt32(swapper.ui32);
				return swapper.f;
			}

			uint64_t SwapInt64(uint64_t num) {
				num = (num & 0x00000000FFFFFFFF) << 32	| (num & 0xFFFFFFFF00000000) >> 32;
				num = (num & 0x0000FFFF0000FFFF) << 16	| (num & 0xFFFF0000FFFF0000) >> 16;
				num = (num & 0x00FF00FF00FF00FF) << 8	| (num & 0xFF00FF00FF00FF00) >> 8;
				return num;
			}

			double SwapFloat64(double num) {

				union {
					double d;
					uint64_t ui64;
				} swapper = { num };

				swapper.ui64 = SwapInt64(swapper.ui64);
				return swapper.d;
			}
		}
	}
}