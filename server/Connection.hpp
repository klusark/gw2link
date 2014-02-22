#pragma once

#include <string>

struct libwebsocket;
struct libwebsocket_context;


class Connection {
public:
	Connection(int id, libwebsocket *wsi, libwebsocket_context *context);
	void queue(const std::string &data);
	void send();
	libwebsocket *_wsi;
	libwebsocket_context *_context;
	int _id;
	char _buffer[1024];
	bool _dataToSend;
};
