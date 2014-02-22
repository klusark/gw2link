#pragma once

#include <string>
#include <vector>

class Connection;

class Game {
public:
	Game();
	void addConnection(Connection *connection);
	void removeConnection(Connection *connection);
	void sendToAll(const std::string &str);
private:
	std::vector<Connection *> _connections;
};

extern Game *g_game;
