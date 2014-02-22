#pragma once

#include <string>

struct libwebsocket;


class Connection {
public:
	Connection();
	void send(const std::string &data);
	libwebsocket *wsi;
	int id;
};
