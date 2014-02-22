/*
 * libwebsockets-test-client - libwebsockets test implementation
 *
 * Copyright (C) 2011 Andy Green <andy@warmcat.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation:
 *  version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA  02110-1301  USA
 */

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <signal.h>

#include <libwebsockets.h>

static unsigned int opts;
static int was_closed;
static int deny_deflate;
static int deny_mux;
static struct libwebsocket *wsi_mirror;
static int mirror_lifetime = 0;
static volatile int force_exit = 0;
static int longlived = 0;

enum demo_protocols {

	PROTOCOL_POSITION,
};

void updateMumble(libwebsocket *wsi);

static int
callback_position(struct libwebsocket_context *context,
			struct libwebsocket *wsi,
			enum libwebsocket_callback_reasons reason,
					       void *user, void *in, size_t len)
{
	switch (reason) {

	case LWS_CALLBACK_CLIENT_ESTABLISHED:
		fprintf(stderr, "callback_dumb_increment: LWS_CALLBACK_CLIENT_ESTABLISHED\n");
		libwebsocket_callback_on_writable(context, wsi);
		break;

	case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
		fprintf(stderr, "LWS_CALLBACK_CLIENT_CONNECTION_ERROR\n");
		was_closed = 1;
		break;

	case LWS_CALLBACK_CLOSED:
		fprintf(stderr, "LWS_CALLBACK_CLOSED\n");
		was_closed = 1;
		break;

	case LWS_CALLBACK_CLIENT_WRITEABLE:
		updateMumble(wsi);
		libwebsocket_callback_on_writable(context, wsi);
		break;

	case LWS_CALLBACK_CLIENT_RECEIVE:
		((char *)in)[len] = '\0';
		fprintf(stderr, "rx %d '%s'\n", (int)len, (char *)in);
		break;

	case LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED:


		break;

	default:
		break;
	}

	return 0;
}




/* list of supported protocols and callbacks */

static struct libwebsocket_protocols protocols[] = {
	{
		"position",
		callback_position,
		0,
	},
	{ NULL, NULL, 0, 0 } /* end */
};


void runSocket() {
	int n = 0;
	int ret = 0;
	int port = 37645;
	int use_ssl = 0;
	struct libwebsocket_context *context;
//	const char *address;
	struct libwebsocket *wsi;
	int ietf_version = -1; /* latest */
	struct lws_context_creation_info info;

	memset(&info, 0, sizeof info);

	/*
	 * create the websockets context.  This tracks open connections and
	 * knows how to route any traffic and which protocol version to use,
	 * and if each connection is client or server side.
	 *
	 * For this client-only demo, we tell it to not listen on any port.
	 */

	info.port = CONTEXT_PORT_NO_LISTEN;
	info.protocols = protocols;

	info.gid = -1;
	info.uid = -1;



	/* create a client websocket using dumb increment protocol */


	while (1) {
		context = libwebsocket_create_context(&info);
		if (context == NULL) {
			fprintf(stderr, "Creating libwebsocket context failed\n");
			return;
		}
		wsi = libwebsocket_client_connect(context, "gw2api.teichroeb.net", port, 0,
			"/", "gw2api.teichroeb.net", "gw2api.teichroeb.net",
				protocols[PROTOCOL_POSITION].name, ietf_version);

		fprintf(stderr, "Waiting for connect...\n");

		n = 0;
		while (n >= 0) {
			n = libwebsocket_service(context, 10);

		}
		libwebsocket_context_destroy(context);
	}


	

}
