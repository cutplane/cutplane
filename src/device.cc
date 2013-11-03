
#include "device.h"

#include <queue>

#include <platform_glut.h>

#include <cstdio>
#include <iostream>

using namespace std;

class InputEvent
{
public:
  Device dev;
  short data;
};


class DeviceState
{
public:
  DeviceState() : dragging(false), mouseX(0), mouseY(0),
		  leftMouse(0), middleMouse(0), rightMouse(0)
  {}
  bool dragging;
  int mouseX, mouseY;
  GLboolean leftMouse, middleMouse, rightMouse;
};

static queue<InputEvent> eventQueue;
static DeviceState deviceState;

extern "C"
void
mouse_button_event_handler(int button, int button_state, int x, int y)
{
  InputEvent newEvent;

  printf("Button %d event (x, y) = (%d, %d), state = %d\n",
	 button, x, y, button_state);

  if ((button == 0) && (button_state == GLUT_DOWN)) // drag started
    deviceState.dragging = true;
  else if ((button == 0) && (button_state == GLUT_UP)) // drag ended
    deviceState.dragging = false;

  newEvent.data = short(button_state == GLUT_DOWN);
  switch (button)
  {
  case 0:
    newEvent.dev = LEFTMOUSE;
    deviceState.leftMouse = newEvent.data;
    break;
  case 1:
    newEvent.dev = MIDDLEMOUSE;
    deviceState.middleMouse = newEvent.data;
    break;
  case 2:
    newEvent.dev = RIGHTMOUSE;
    deviceState.rightMouse = newEvent.data;
    break;
  }

  deviceState.mouseX = x;
  deviceState.mouseY = y;

  eventQueue.push(newEvent);

  glutPostRedisplay();
}

extern "C"
void
keyboard_event_handler(unsigned char key, int x, int y)
{
  // glutGetModifiers() may be called to determine the state of modifier
  // keys when the keystroke generating the callback occurred.
  int modifiers = glutGetModifiers();
  printf("Key %d event (x, y) = (%d, %d), shift state = %d, "
	 "ctrl state = %d, alt state = %d\n",
	 key, x, y,
	 (modifiers & GLUT_ACTIVE_SHIFT) != 0,
	 (modifiers & GLUT_ACTIVE_CTRL) != 0,
	 (modifiers & GLUT_ACTIVE_ALT) != 0);

  deviceState.mouseX = x;
  deviceState.mouseY = y;

  glutPostRedisplay();
}

extern "C"
void
motion_event_handler(int x, int y)
{
  // Warps the window system's pointer to a new location relative to
  // the origin of the current window:
  // glutWarpPointer(int x , int y);

  printf("Motion event (x, y) = (%d, %d)\n", x, y);

  deviceState.dragging = false;
  deviceState.mouseX = x;
  deviceState.mouseY = y;

  glutPostRedisplay();
}

extern "C"
void
drag_event_handler(int x, int y)
{
  printf("Drag event (x, y) = (%d, %d)\n", x, y);

  deviceState.dragging = true;
  deviceState.mouseX = x;
  deviceState.mouseY = y;

  glutPostRedisplay();
}


extern "C"
Device
getqentry()
{
  short data;
  int qentry;

  cout << "getqentry()" << endl;

  if (qtest())
  {
    qentry = qread(&data);
    if (data)
      return (qentry);
    return (-qentry);
  }
  return (NULLDEV);
}

extern "C"
void
qreset(void)
{
  while (!eventQueue.empty())
    eventQueue.pop();
}

extern "C"
long
qread(short *data)
{
  Device device = eventQueue.front().dev;
  *data = eventQueue.front().data;
  eventQueue.pop();
  
  return device;
}

extern "C"
GLboolean
getbutton(Device num)
{
  switch (num)
  {
  case LEFTMOUSE:
    return deviceState.leftMouse;
  case MIDDLEMOUSE:
    return deviceState.middleMouse;
  case RIGHTMOUSE:
    return deviceState.rightMouse;
  }
  return 0;
}

extern "C"
long
getvaluator(Device val)
{
  switch (val)
  {
  case MOUSEX:
    cout << "getvaluator(MOUSEX) = " << deviceState.mouseX << endl;
    return deviceState.mouseX;
  case MOUSEY:
    cout << "getvaluator(MOUSEY) = " << deviceState.mouseY << endl;
    return (glutGet(GLUT_WINDOW_HEIGHT) - 1) - deviceState.mouseY;
  }
  return 0;
}

extern "C"
void
setvaluator(Device val, short init, short min, short max)
{
}

extern "C"
long
qtest(void)
{
  if (eventQueue.empty())
    return 0;

  return eventQueue.front().dev;
}

static void
qdevice(Device dev)
{

}

static void
unqdevice(Device dev)
{

}

static GLboolean
isqueued(short dev)
{

}

static void
qenter(short qtype, short val)
{

}

static void
tie(Device b, Device v1, Device v2)
{

}
