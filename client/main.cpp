#include <stdint.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cstdio>
#include <libwebsockets.h>
#include <string>
#include <algorithm>


struct LinkedMem {
	UINT32	uiVersion;
	DWORD	uiTick;
	float	fAvatarPosition[3];
	float	fAvatarFront[3];
	float	fAvatarTop[3];
	wchar_t	name[256];
	float	fCameraPosition[3];
	float	fCameraFront[3];
	float	fCameraTop[3];
	wchar_t	identity[256];
	UINT32	context_len;
	unsigned char context[256];
	wchar_t description[2048];
};

LinkedMem *lm = NULL;

void initMumble() {

	HANDLE hMapObject = CreateFileMapping (INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE | SEC_COMMIT, 0, sizeof(LinkedMem), "MumbleLink");
	if (hMapObject == NULL)
		return;

	lm = (LinkedMem *) MapViewOfFile(hMapObject, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(LinkedMem));
	if (lm == NULL) {
		CloseHandle(hMapObject);
		hMapObject = NULL;
		return;
	}

}

float lastPos[3];

void updateMumble(libwebsocket *wsi) {
	if (! lm)
		return;

	bool send = false;
	for (int i = 0; i < 3; ++i) {
		if (abs(lm->fAvatarPosition[i] - lastPos[i]) > 0.3f) {
			send = true;
			break;
		}
	}
	if (!send) {
		return;
	}

	char url[1024];
	char buff[1024];
	sprintf(buff, "%ls", lm->identity);
	int len = strlen(buff);
	if (len != 0) {
		buff[len - 1] = '\0';
	}
	sprintf(url, "%s,\"x\":%f,\"y\":%f,\"z\":%f}", buff, lm->fAvatarPosition[0], lm->fAvatarPosition[1], lm->fAvatarPosition[2]);
	std::string u = url;
	u.erase(std::remove_if(u.begin(), u.end(), isspace), u.end());
	for (int i = 0; i < 3; ++i) {
		lastPos[i] = lm->fAvatarPosition[i];
	}

	unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + 2048 + LWS_SEND_BUFFER_POST_PADDING];
	unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];
	int size = u.length();
	memcpy(p, u.c_str(), size);
	int res = libwebsocket_write(wsi, p, size, LWS_WRITE_TEXT);
	if (res < 0 || res < size) {
		printf("Bad %d\n", res);
	}
	
	printf("%f %f %f\n", lm->fAvatarPosition[0], lm->fAvatarPosition[1], lm->fAvatarPosition[2]);
}

void runSocket();

int main() {
	for (int i = 0; i < 3; ++i) {
		lastPos[i] = 0;
	}
	initMumble();
	runSocket();

}

