#pragma once

#include <stdint.h>

#ifdef _WIN32
#include <WinSock2.h>

inline uint64_t htobe64(uint64_t htobe64)
{
	return htonll(htobe64);
}
inline uint32_t htobe32(uint32_t host32)
{
	return htonl(host32);
}
inline uint16_t htobe16(uint16_t host16)
{
	return htons(host16);
}
inline uint64_t be64toh(uint64_t net64)
{
	return ntohll(net64);
}
inline uint32_t be32toh(uint32_t net32)
{
	return ntohl(net32);
}
inline uint16_t be16toh(uint16_t net16)
{
	return ntohs(net16);
}

#else
#include <endian.h>
#endif

namespace muduo
{
	namespace net
	{
		namespace sockets
		{
			inline uint64_t hostToNetwork64(uint64_t host64)
			{
				return htobe64(host64);
			}
			inline uint32_t hostToNetwork32(uint32_t host32)
			{
				return htobe32(host32);
			}
			inline uint16_t hostToNetwork16(uint16_t host16)
			{
				return htobe16(host16);
			}

			inline uint64_t networkToHost64(uint64_t net64)
			{
				return be64toh(net64);
			}
			inline uint32_t networkToHost32(uint32_t net32)
			{
				return be32toh(net32);
			}
			inline uint16_t networkToHost16(uint16_t net16)
			{
				return be16toh(net16);
			}
		}
	}
}