# Third party components

TPFanControl itself is public domain, see the Unlicense in `LICENSE`. It is distributed with one
component that is not, listed here.

## LpcACPIEC.bin

A PawnIO module. It is what lets the program reach the embedded controller, by permitting access to
the ACPI EC ports 0x62 and 0x66.

- Part of PawnIO Modules, Copyright (C) 2023 namazso <admin@namazso.eu>
- Licensed under the GNU Lesser General Public License, version 2.1 or later
- `SPDX-License-Identifier: LGPL-2.1-or-later`
- Full licence text: `fancontrol/LICENSE.LGPL-2.1.txt`
- Source: <https://github.com/namazso/PawnIO.Modules> (`LpcACPIEC.p`)

The file shipped here is upstream's signed release, unmodified. Its signature is what the PawnIO
driver checks before loading it, so it cannot be altered without breaking it.

Being LGPL-2.1 does not affect this program's own licence. The exception in PawnIO's licence covers
"independent modules that communicate with PawnIO solely through the device IO control interface",
which is all this program does, so nothing propagates into TPFanControl's public domain source.

## PawnIO

The driver itself is not distributed here. Install it from <https://pawnio.eu>.
