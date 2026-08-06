// --------------------------------------------------------------
//
//  Thinkpad Fan Control
//
// --------------------------------------------------------------
//
//	This program and source code is in the public domain.
//
// --------------------------------------------------------------

// Port access through PawnIO, a signed driver that runs sandboxed modules.
// Talked to over DeviceIoControl directly: PawnIOLib is 64 bit only.

#include "_prec.h"
#include "ecbackend.h"
#include <winioctl.h>   // WIN32_LEAN_AND_MEAN leaves CTL_CODE out

#define PAWNIO_DEVICE      "\\\\?\\GLOBALROOT\\Device\\PawnIO"
#define PAWNIO_DEVICE_TYPE 41394
#define PAWNIO_FN_NAME_LEN 32

#define IOCTL_PIO_LOAD_BINARY CTL_CODE(PAWNIO_DEVICE_TYPE, 0x821, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PIO_EXECUTE_FN  CTL_CODE(PAWNIO_DEVICE_TYPE, 0x841, METHOD_BUFFERED, FILE_ANY_ACCESS)

static const char* const MODULE_STOCK = "LpcACPIEC.bin";

static HANDLE g_device = INVALID_HANDLE_VALUE;
static const char* g_module = NULL;

//-------------------------------------------------------------------------
//  [32 byte NUL padded name][in_size * 8 bytes] -> [out_size * 8 bytes]
//-------------------------------------------------------------------------
static bool Execute(const char* fn, const ULONG64* in, DWORD nin, ULONG64* out, DWORD nout) {
	BYTE inbuf[PAWNIO_FN_NAME_LEN + 8 * 4];
	if (nin > 4)
		return false;

	ZeroMemory(inbuf, sizeof(inbuf));
	lstrcpynA((char*)inbuf, fn, PAWNIO_FN_NAME_LEN - 1);
	if (nin)
		memcpy(inbuf + PAWNIO_FN_NAME_LEN, in, nin * sizeof(ULONG64));

	DWORD written = 0;
	return ::DeviceIoControl(g_device, IOCTL_PIO_EXECUTE_FN,
		inbuf, PAWNIO_FN_NAME_LEN + nin * sizeof(ULONG64),
		out, nout * sizeof(ULONG64), &written, NULL) != 0;
}

//-------------------------------------------------------------------------
//  load a module blob sitting next to the exe
//-------------------------------------------------------------------------
static bool LoadModule(const char* name) {
	char path[MAX_PATH];
	if (!::GetModuleFileName(NULL, path, MAX_PATH))
		return false;

	char* slash = strrchr(path, '\\');
	if (!slash)
		return false;
	*(slash + 1) = '\0';
	if (strlen(path) + strlen(name) >= MAX_PATH)
		return false;
	strcat_s(path, sizeof(path), name);

	HANDLE file = ::CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (file == INVALID_HANDLE_VALUE)
		return false;

	DWORD size = ::GetFileSize(file, NULL);
	bool ok = false;
	BYTE* blob = (size != INVALID_FILE_SIZE && size > 0 && size < 0x100000) ? new BYTE[size] : NULL;

	if (blob) {
		DWORD got = 0;
		if (::ReadFile(file, blob, size, &got, NULL) && got == size) {
			DWORD ret = 0;
			ok = ::DeviceIoControl(g_device, IOCTL_PIO_LOAD_BINARY, blob, size, NULL, 0, &ret, NULL) != 0;
		}
		delete[] blob;
	}

	::CloseHandle(file);

	return ok;
}

bool PawnIo_Open() {
	g_device = ::CreateFile(PAWNIO_DEVICE, GENERIC_READ | GENERIC_WRITE,
		0, NULL, OPEN_EXISTING, 0, NULL);
	if (g_device == INVALID_HANDLE_VALUE)
		return false;

	if (!LoadModule(MODULE_STOCK)) {
		PawnIo_Close();
		return false;
	}

	g_module = MODULE_STOCK;

	return true;
}

void PawnIo_Close() {
	if (g_device != INVALID_HANDLE_VALUE)
		::CloseHandle(g_device);

	g_device = INVALID_HANDLE_VALUE;
	g_module = NULL;
}

const char* PawnIo_Module() {
	return g_module ? g_module : "none";
}

bool PawnIo_PortSupported(USHORT port) {
	// the stock module permits the ACPI EC and nothing else
	return g_module && (port == 0x62 || port == 0x66);
}

UCHAR PawnIo_Read(USHORT port) {
	ULONG64 in = port, out = 0;

	return Execute("ioctl_pio_read", &in, 1, &out, 1) ? (UCHAR)out : (UCHAR)0xff;
}

void PawnIo_Write(USHORT port, UCHAR value) {
	ULONG64 in[2] = { port, value };

	Execute("ioctl_pio_write", in, 2, NULL, 0);
}
