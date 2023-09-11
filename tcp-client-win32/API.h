
#pragma once

enum APIDetails
{
	API_VERSION = 1,
	API_PORT = 2004,
};

enum APIResults
{
	API_OK = 1,

	API_ERROR_BASE = 0x70000000,
	API_ERROR_INVALID_PACKET,
	API_ERROR_INVALID_COMMAND,
};

enum APICommands
{
	API_CMD_TEST_CONNECTIVITY,
	API_CMD_NOTIFY,

	API_CMD_DBG_GET_PROCESS_LIST,
	API_CMD_DBG_GET_PROCESS_NAME,
	API_CMD_DBG_READ_PROCESS_MEMORY,
	API_CMD_DBG_WRITE_PROCESS_MEMORY,

	API_CMD_COUNT
};

static constexpr char PacketMagic[] = "VALIANT";

struct InitialPacket
{
	char magic[8];
	int version;
	int cmd;
};

struct NotifyPacket
{
	char icon[32];
	char text[512];
};

struct DebugProcessListPacket
{
	int pid[16];
};

struct DebugProcessNamePacket
{
	char name[32];
};

struct DebugReadWriteProcessMemoryPacket
{
	int pid;
	int address;
	int size;
};

namespace API
{
	bool TestConnectivity(const char* ip);
	bool DbgGetProcessList(const char* ip, int list[16]);
	bool DbgGetProcessName(const char* ip, int pid, char name[32]);
	bool DbgReadProcessMemory(const char* ip, int pid, int address, void* data, int size);
	bool DbgWriteProcessMemory(const char* ip, int pid, int address, const void* data, int size);
}