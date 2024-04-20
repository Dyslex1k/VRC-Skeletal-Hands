#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "hekky/osc/asserts.hpp"
#include "hekky/osc/oscpacket.hpp"

namespace hekky {
	namespace osc {
		struct OscMessage : OscPacket {
		public:
			OscMessage(const std::string& address);
			~OscMessage();

			// Explicit Push functions
			OscMessage PushBlob(char* data, size_t size);

			OscMessage PushFloat32(float data);
			OscMessage PushFloat64(double data);
			OscMessage PushInt32(int data);
			OscMessage PushInt64(long long data);

			OscMessage PushBoolean(bool data);

			OscMessage PushString(std::string data);
			OscMessage PushStringRef(const std::string& data);
			OscMessage PushCStyleString(char* data);
			OscMessage PushCStyleStringRef(const char* data);

			OscMessage PushWString(std::wstring data);
			OscMessage PushWStringRef(const std::wstring& data);
			OscMessage PushCStyleWString(wchar_t* data);
			OscMessage PushCStyleWStringRef(const wchar_t* data);

			// Aliases
			OscMessage PushFloat(float data);
			OscMessage PushDouble(double data);
			OscMessage PushInt(int data);
			OscMessage PushLongLong(long long data);
			OscMessage PushBool(bool data);

			// Binary blobs
			OscMessage Push(char* data, size_t size);

			// Floating point number
			OscMessage Push(float data);
			OscMessage Push(double data);

			// Integers
			OscMessage Push(int data);
			OscMessage Push(long long data);

			// ASCII Strings
			OscMessage Push(std::string data);
			OscMessage Push(const std::string& data);
			OscMessage Push(char* data);
			OscMessage Push(const char* data);
			
			// Wide strings
			OscMessage Push(std::wstring data);
			OscMessage Push(const std::wstring& data);
			OscMessage Push(wchar_t* data);
			OscMessage Push(const wchar_t* data);

			template<typename T>
			OscMessage Push(T data) {
				HEKKYOSC_ASSERT(m_readonly == false, "Cannot write to a message packet once sent to the network! Construct a new message instead.");

				return PushBlob(data, sizeof(data));
			}

			inline const std::string& GetAddress() const {
				return m_address;
			}

		private:
			char* GetBytes(int& size);

		private:
			bool m_readonly;
			std::string m_address;
			std::string m_type;
			std::vector<char> m_data;
		};
	}
}