#include <sstream>
#include <ctime>
#include <algorithm>
#include "Game.hpp"
#include "Connection.hpp"

Game *g_game = nullptr;

Game::Game() {
}


void Game::addConnection(Connection *connection) {
	_connections.push_back(connection);
}

void Game::removeConnection(Connection *connection) {
	auto it = std::find(_connections.begin(), _connections.end(), connection);
	_connections.erase(it);
}

void Game::sendToAll(const std::string &str) {
	for (int i = 0; i < _connections.size(); ++i) {
		_connections[i]->queue(str);
	}
}

