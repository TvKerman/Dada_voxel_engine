#pragma once
#ifndef WINDOW_WINDOW_H_
#define WINDOW_WINDOW_H_

struct GLFWwindow;

class Window {
public:
	static int width;
	static int height;
	static GLFWwindow* window; // �� ������ ������� ������ window ���������
	static int initialize(int width, int height, const char* title);
	static void terminate();

	static void viewport(int x, int y, int width, int height);
	static void setCursorMode(int mode);
	static bool isShouldClose();
	static void setShouldClose(bool flag);
	static void swapBuffers();
	static void swapInterval(int interval);
};

#endif /* WINDOW_WINDOW_H_ */