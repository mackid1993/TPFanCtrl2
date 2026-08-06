// --------------------------------------------------------------
//
//  Thinkpad Fan Control
//
// --------------------------------------------------------------
//
//	This program and source code is in the public domain.
//
//	The author claims no copyright, copyleft, license or
//	whatsoever for the program itself (with exception of
//	WinIO driver).  You may use, reuse or distribute it's
//	binaries or source code in any desired way or form,
//	Useage of binaries or source shall be entirely and
//	without exception at your own risk.
//
// --------------------------------------------------------------

#include "_prec.h"
#include "ecbackend.h"

static enum {
	BACKEND_NONE,
	BACKEND_PAWNIO
} g_backend = BACKEND_NONE;

bool EcBackend_Open() {
	if (PawnIo_Open()) {
		g_backend = BACKEND_PAWNIO;
		return true;
	}

	return false;
}

void EcBackend_Close() {
	if (g_backend == BACKEND_PAWNIO)
		PawnIo_Close();

	g_backend = BACKEND_NONE;
}

const char* EcBackend_Name() {
	return g_backend == BACKEND_PAWNIO ? PawnIo_Module() : "none";
}

bool EcBackend_PortSupported(USHORT port) {
	return g_backend == BACKEND_PAWNIO && PawnIo_PortSupported(port);
}

UCHAR ReadPort(USHORT port) {
	return g_backend == BACKEND_PAWNIO ? PawnIo_Read(port) : (UCHAR)0xff;
}

void WritePort(USHORT port, UCHAR value) {
	if (g_backend == BACKEND_PAWNIO)
		PawnIo_Write(port, value);
}
