#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <libwebsockets.h>
#include <jansson.h>

#include "Connection.hpp"
#include "Game.hpp"

enum protocols {
	PROTOCOL_WEB,
	PROTOCOL_POSITION,
};

static int nextid = 0;

static int
callback_web(libwebsocket_context *context,
			libwebsocket *wsi,
			enum libwebsocket_callback_reasons reason,
					       void *user, void *in, size_t len)
{
	Connection *conn = (Connection *)user;
	const char *recv = (const char *)in;

	switch (reason) {

	case LWS_CALLBACK_ESTABLISHED:
		memset(conn, 0, sizeof(Connection));
		*conn = Connection(nextid, wsi, context);
		++nextid;
		fprintf(stderr, "New Connection\n");
		break;

	case LWS_CALLBACK_SERVER_WRITEABLE:
		conn->send();
		break;

	case LWS_CALLBACK_RECEIVE:
		{
		json_error_t error;
		json_t *json = json_loads(recv, 0, &error);
		if (json == nullptr) {
			break;
		}
		const char *type = json_string_value(json_object_get(json, "type"));
		if (strcmp(type, "login") == 0) {
			g_game->addConnection(conn);

		}
		}
		break;
	case LWS_CALLBACK_CLOSED:
		fprintf(stderr, "Disconnect\n");
		g_game->removeConnection(conn);
		break;
	case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
		break;
	default:
		//printf("Web Receive%d\n", reason);
		break;
	}

	return 0;
}
static int
callback_position(libwebsocket_context *context,
			libwebsocket *wsi,
			enum libwebsocket_callback_reasons reason,
					       void *user, void *in, size_t len)
{
	Connection *conn = (Connection *)user;
	const char *recv = (const char *)in;

	switch (reason) {

	case LWS_CALLBACK_ESTABLISHED:
		memset(conn, 0, sizeof(Connection));
		*conn = Connection(nextid, wsi, context);
		++nextid;
		fprintf(stderr, "New Connection\n");
		break;

	case LWS_CALLBACK_SERVER_WRITEABLE:
		conn->send();
		break;

	case LWS_CALLBACK_RECEIVE:
	{
		std::string str = ((char *)in);
		g_game->sendToAll(str);
	}
		break;
	case LWS_CALLBACK_CLOSED:
		fprintf(stderr, "PositionDisconnect\n");
		break;
	case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
		break;
	default:
		//printf("Position Unhandled %d\n", reason);
		break;
	}

	return 0;
}

/* list of supported protocols and callbacks */

static struct libwebsocket_protocols protocols[] = {
	{
		"web",
		callback_web,
		sizeof(Connection),
		0,
		0
	},
	{
		"position",
		callback_position,
		sizeof(Connection),
		0,
		0
	},
	{
		NULL, NULL, 0		/* End of list */
	}
};

int main(int argc, char **argv)
{
	int n = 0;

	int port = 37645;
	struct libwebsocket_context *context;
	int opts = 0;
	const char *interface = NULL;


	g_game = new Game();


	lws_context_creation_info info;
	memset(&info, 0, sizeof(info));
	info.port = port;
	info.iface = interface;
	info.protocols = protocols;
	info.extensions = libwebsocket_get_internal_extensions();
	info.gid = -1;
	info.uid = -1;
	info.options = opts;

	context = libwebsocket_create_context(&info);
	if (context == NULL) {
		fprintf(stderr, "libwebsocket init failed\n");
		return -1;
	}

	while (n >= 0) {
		n = libwebsocket_service(context, 5000);
	}


	libwebsocket_context_destroy(context);

	return 0;
}
