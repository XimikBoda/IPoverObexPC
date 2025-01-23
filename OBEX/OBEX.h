#pragma once

// https://btprodspecificationrefs.blob.core.windows.net/ext-ref/IrDA/OBEX15.pdf

enum OBEX_Operations {
	Connect		= 0x80,
	Disconnect	= 0x81,
	Put			= 0x82,
	Get			= 0x83,
	SetPath		= 0x85,
	Action		= 0x86,
	Session		= 0x87,
	Abort		= 0xFF
}; // high bit = final or request

enum OBEX_Responses {
	Continue					= 0x90,
	Success						= 0xA0,
	Created						= 0xA1,
	Accepted					= 0xA2,
	NonAuthoritativeInformation	= 0xA3,
	NoContent					= 0xA4,
	ResetContent				= 0xA5,
	PartialContent				= 0xA6
};

enum OBEX_Headers {
	Name	= 0x01,
	Length	= 0xC3,
	Body	= 0x48,
};

enum OBEX_HeadersTypes {
	TextH	= 0b00,
	BytesH	= 0b01,
	ByteH	= 0b10,
	IntH	= 0b11
};

