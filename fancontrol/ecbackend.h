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

#ifndef ECBACKEND_H
#define ECBACKEND_H

#pragma once

#include "_prec.h"

// Port access goes through a backend, so a machine that a newer driver
// cannot serve keeps working on the one it uses today.
bool EcBackend_Open();

void EcBackend_Close();

const char* EcBackend_Name();

// false means the backend will refuse this port, so don't bother probing it
bool EcBackend_PortSupported(USHORT port);

UCHAR ReadPort(USHORT port);

void WritePort(USHORT port, UCHAR value);

// backend_pawnio.cpp
bool PawnIo_Open();

void PawnIo_Close();

const char* PawnIo_Module();

bool PawnIo_PortSupported(USHORT port);

UCHAR PawnIo_Read(USHORT port);

void PawnIo_Write(USHORT port, UCHAR value);

#endif // ECBACKEND_H
