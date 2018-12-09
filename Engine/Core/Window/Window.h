#pragma once

#include <cstdint>
#include <functional>
#include <map>

#include "../input.h"

class Window
{
public: // types
	using KeyEventListener = std::function<void(Input::KeyEvent const &)>;
	using QuitEventListener = std::function<void(int)>;
	struct EventListenerHandle
	{
		uint64_t id;
		bool operator<(EventListenerHandle const &other) const { return id < other.id; }
	};

public: // methods
	virtual void Show() = 0;
	virtual void Hide() = 0;
	virtual void PumpMessages() = 0;

	uint32_t GetWidth() const { return m_width; }
	uint32_t GetHeight() const { return m_height; }

	Input &GetInput() { return m_input; }
	EventListenerHandle AddKeyEventListener(KeyEventListener const &listener);
	EventListenerHandle AddQuitEventListener(QuitEventListener const &listener);
	void RemoveEventListener(EventListenerHandle handle);

protected: // methods
	Window(uint32_t width, uint32_t height);
	virtual ~Window() = default;
	EventListenerHandle CreateEventListenerHandle();
	
protected: // members
	std::map<EventListenerHandle, KeyEventListener> m_keyEventListenerByHandle;
	std::map<EventListenerHandle, QuitEventListener> m_quitEventListenerByHandle;
	uint32_t m_width;
	uint32_t m_height;
	Input m_input;
};
