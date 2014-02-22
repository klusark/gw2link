#include <libwebsockets.h>
#include <cstring>
#include <cstdio>

#include "Connection.hpp"


Connection::Connection(int id, libwebsocket *wsi, libwebsocket_context *context) {
	_id = id;
	_wsi = wsi;
	_context = context;
	_dataToSend = false;
	memset(_buffer, 0, 1024);
}

void Connection::queue(const std::string &data) {
	_dataToSend = true;
	//_buffer = data;
	strcpy(_buffer, data.c_str());
	libwebsocket_callback_on_writable(_context, _wsi);
}

void Connection::send() {
	if (!_dataToSend) {
		return;
	}
	_dataToSend = false;
	unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + 2048 + LWS_SEND_BUFFER_POST_PADDING];
	unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];
	size_t size = strlen(_buffer);
	memcpy(p, _buffer, size);
	int res = libwebsocket_write(_wsi, p, size, LWS_WRITE_TEXT);
	if (res < 0 || res < size) {
		printf("Bad %d\n", res);
	}
}
