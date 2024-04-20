#pragma once

#include <stdint.h>

namespace hekky {
	namespace osc {
		struct OscPacket {

		public:

		private:
			virtual char* GetBytes(int& size) = 0;

			friend class UdpSender;
		};

		namespace constants {
			const static uint64_t OSC_MINIMUM_PACKET_BYTES = 8;
		}
	}
}