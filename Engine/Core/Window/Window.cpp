#include "Window.h"

static uint32_t g_nextEventListenerHandleId = 1;

Window::EventListenerHandle 
Window::AddKeyEventListener(KeyEventListener const &listener)
{
	EventListenerHandle handle = CreateEventListenerHandle();
	m_keyEventListenerByHandle[handle] = listener;
	return handle;
}

Window::EventListenerHandle 
Window::AddQuitEventListener(QuitEventListener const &listener)
{
	EventListenerHandle handle = CreateEventListenerHandle();
	m_quitEventListenerByHandle[handle] = listener;
	return handle;
}

void 
Window::RemoveEventListener(EventListenerHandle handle)
{
	m_keyEventListenerByHandle.erase(handle);
	m_quitEventListenerByHandle.erase(handle);
}

Window::Window(uint32_t width, uint32_t height)
	: m_width(width)
	, m_height(height)
{
}

Window::EventListenerHandle Window::CreateEventListenerHandle()
{
	return EventListenerHandle{ g_nextEventListenerHandleId++ };
}
