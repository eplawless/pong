#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

#include "input.h"
#include "pong-event.h"

class InputMapper
{
public: // methods
	static InputMapper LoadConfigFromFile(char const *filename);
	GameEvent MapToGameEvent(Input::KeyEvent const &keyEvent) const;
	GameEventList MapToGameEventList(Input::KeyEventList const &arrKeyEvents) const;

private: // methods
	InputMapper() = default;
	bool ParseAndAddConfigEntry(std::string const &line);

private: // types
	struct MappedEvents
	{
		MappedEvents() : onKeyUp(GameEvent::None), onKeyDown(GameEvent::None) {}
		GameEvent onKeyUp;
		GameEvent onKeyDown;
	};
	using KeyToEventsMap = std::unordered_map<uint32_t, MappedEvents>;

private: // members
	KeyToEventsMap m_keyToMappedEvents;
};