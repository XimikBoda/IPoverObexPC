#pragma once

// https://btprodspecificationrefs.blob.core.windows.net/ext-ref/IrDA/OBEX15.pdf

enum OBEX_Operations {
	Connect = 0x80,
	Disconnect = 0x81,
	Put = 0x82,
	Get = 0x83,
	SetPath = 0x85,
	Action = 0x86,
	Session = 0x87,
	Abort = 0xFF
}; // high bit = final or request