#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <regex>

#include "input.h"
#include "../Libraries/nonstd/optional.hpp"

template <typename GameEvent>
class InputMapper
{
public: // types
	using EventNameToGameEventMap = std::unordered_map<std::string, GameEvent>;
	using MaybeGameEvent = nonstd::optional<GameEvent>;
	using GameEventList = std::vector<GameEvent>;

public: // static methods
	static InputMapper LoadConfigFromFile(
		char const *filename,
		EventNameToGameEventMap const &eventNameToGameEvent);

public: // methods
	MaybeGameEvent MapToGameEvent(Input::KeyEvent const &keyEvent) const;
	GameEventList MapToGameEventList(Input::KeyEventList const &arrKeyEvents) const;

private: // types
	struct MappedEvents
	{
		MaybeGameEvent onKeyUp;
		MaybeGameEvent onKeyDown;
	};
	using KeyToEventsMap = std::unordered_map<uint32_t, MappedEvents>;

private: // methods
	InputMapper() = default;
	bool ParseAndAddConfigEntry(std::string const &line);

private: // static members
	static std::regex s_reConfigFileEntry;
	static std::unordered_map<std::string, uint32_t> const s_keyNameToKey;
	static std::unordered_map<std::string, Input::KeyEvent::Type> s_actionNameToAction;

private: // members
	KeyToEventsMap m_keyToMappedEvents;
	EventNameToGameEventMap m_eventNameToGameEvent;
};

// ----------------------

template <typename GameEvent>
InputMapper<GameEvent> 
InputMapper<GameEvent>::LoadConfigFromFile(
	char const *filename,
	EventNameToGameEventMap const &eventNameToGameEvent)
{
	InputMapper<GameEvent> result;
	result.m_eventNameToGameEvent = eventNameToGameEvent;
	std::fstream configFile;
	configFile.open(filename, std::fstream::in);
	while (configFile.good())
	{
		std::string line;
		std::getline(configFile, line);
		if (configFile.bad()) { break; }
		result.ParseAndAddConfigEntry(line);
	}
	return result;
}

template <typename GameEvent>
typename InputMapper<GameEvent>::MaybeGameEvent 
InputMapper<GameEvent>::MapToGameEvent(
	Input::KeyEvent const &keyEvent
) const
{
	auto it = m_keyToMappedEvents.find(keyEvent.key);
	if (it == m_keyToMappedEvents.end())
	{
		return MaybeGameEvent();
	}
	MappedEvents const &events = it->second;
	return keyEvent.type == Input::KeyEvent::KeyUp ? events.onKeyUp : events.onKeyDown;
}

template <typename GameEvent>
typename InputMapper<GameEvent>::GameEventList
InputMapper<GameEvent>::MapToGameEventList(
	Input::KeyEventList const &arrKeyEvents
) const
{
	GameEventList result;
	for (Input::KeyEvent const &keyEvent : arrKeyEvents)
	{
		MaybeGameEvent maybeEvent = MapToGameEvent(keyEvent);
		if (maybeEvent.has_value()) 
		{ 
			result.push_back(maybeEvent.value()); 
		}
	}
	return result;
}

template <typename GameEvent>
bool 
InputMapper<GameEvent>::ParseAndAddConfigEntry(std::string const &line)
{
	std::smatch matches;
	if (!std::regex_match(line, matches, s_reConfigFileEntry))
	{
		return false;
	}
	// get key
	std::string keyName = matches[1].str();
	std::transform(std::begin(keyName), std::end(keyName), std::begin(keyName), ::tolower);
	auto itKey = s_keyNameToKey.find(keyName);
	if (itKey == s_keyNameToKey.end())
	{
		return false;
	}
	uint32_t key = itKey->second;

	// get action
	std::string actionName = matches[2].str();
	std::transform(std::begin(actionName), std::end(actionName), std::begin(actionName), ::tolower);
	auto itAction = s_actionNameToAction.find(actionName);
	if (itAction == s_actionNameToAction.end())
	{
		return false;
	}
	Input::KeyEvent::Type action = itAction->second;

	// get event
	std::string eventName = matches[3].str();
	auto itEvent = m_eventNameToGameEvent.find(eventName);
	if (itEvent == m_eventNameToGameEvent.end())
	{
		return false;
	}
	GameEvent event = itEvent->second;

	// create entry
	MappedEvents &mappedEvents = m_keyToMappedEvents[key];
	if (action == Input::KeyEvent::KeyDown)
	{
		mappedEvents.onKeyDown = event;
	}
	else if (action == Input::KeyEvent::KeyUp)
	{
		mappedEvents.onKeyUp = event;
	}
	return true;
}

// ----------------------

template <typename GameEvent>
std::regex 
InputMapper<GameEvent>::s_reConfigFileEntry(
	"\\s*(\\w+)" // key name
	"\\s+(\\w+)" // action
	"\\s+(\\w+)" // event
	"\\s*(#.*)?", // optional comment
	std::regex_constants::icase);

// TODO: allow entries like "0x37 keydown LeftPaddleMoveUp"
// https://docs.microsoft.com/en-us/windows/desktop/inputdev/virtual-key-codes
template <typename GameEvent>
std::unordered_map<std::string, uint32_t> const 
InputMapper<GameEvent>::s_keyNameToKey = {
	{ "mouse1", 0x01 },
	{ "mouse2", 0x02 },
	{ "cancel", 0x03 },
	{ "mouse3", 0x04 },
	{ "mouse4", 0x05 },
	{ "mouse5", 0x06 },
	{ "backspace", 0x08 },
	{ "tab", 0x09 },
	{ "clear", 0x0c },
	{ "enter", 0x0d },
	{ "return", 0x0d },
	{ "shift", 0x10 },
	{ "control", 0x11 },
	{ "alt", 0x12 },
	{ "pause", 0x13 },
	{ "capslock", 0x14 },
	{ "escape", 0x1b },
	{ "esc", 0x1b },
	{ "space", 0x20 },
	{ "pageup", 0x21 },
	{ "pagedown", 0x22 },
	{ "end", 0x23 },
	{ "home", 0x24 },
	{ "left", 0x25 },
	{ "up", 0x26 },
	{ "right", 0x27 },
	{ "down", 0x28 },
	{ "select", 0x29 },
	{ "print", 0x2a },
	{ "execute", 0x2b },
	{ "printscreen", 0x2c },
	{ "insert", 0x2d },
	{ "delete", 0x2e },
	{ "help", 0x2f },
	{ "0", 0x30 },
	{ "1", 0x31 },
	{ "2", 0x32 },
	{ "3", 0x33 },
	{ "4", 0x34 },
	{ "5", 0x35 },
	{ "6", 0x36 },
	{ "7", 0x37 },
	{ "8", 0x38 },
	{ "9", 0x39 },
	{ "a", 0x41 },
	{ "b", 0x42 },
	{ "c", 0x43 },
	{ "d", 0x44 },
	{ "e", 0x45 },
	{ "f", 0x46 },
	{ "g", 0x47 },
	{ "h", 0x48 },
	{ "i", 0x49 },
	{ "j", 0x4a },
	{ "k", 0x4b },
	{ "l", 0x4c },
	{ "m", 0x4d },
	{ "n", 0x4e },
	{ "o", 0x4f },
	{ "p", 0x50 },
	{ "q", 0x51 },
	{ "r", 0x52 },
	{ "s", 0x53 },
	{ "t", 0x54 },
	{ "u", 0x55 },
	{ "v", 0x56 },
	{ "w", 0x57 },
	{ "x", 0x58 },
	{ "y", 0x59 },
	{ "z", 0x5a },
	{ "num0", 0x60 },
	{ "num1", 0x61 },
	{ "num2", 0x62 },
	{ "num3", 0x63 },
	{ "num4", 0x64 },
	{ "num5", 0x65 },
	{ "num6", 0x66 },
	{ "num7", 0x67 },
	{ "num8", 0x68 },
	{ "num9", 0x69 },
	{ "num*", 0x6a },
	{ "num+", 0x6b },
	{ "num-", 0x6d },
	{ "num.", 0x6e },
	{ "num/", 0x6f },
	{ "f1", 0x70 },
	{ "f2", 0x71 },
	{ "f3", 0x72 },
	{ "f4", 0x73 },
	{ "f5", 0x74 },
	{ "f6", 0x75 },
	{ "f7", 0x76 },
	{ "f8", 0x77 },
	{ "f9", 0x78 },
	{ "f10", 0x79 },
	{ "f11", 0x7a },
	{ "f12", 0x7b },
	{ "f13", 0x7c },
	{ "f14", 0x7d },
	{ "f15", 0x7e },
	{ "f16", 0x7f },
	{ "f17", 0x80 },
	{ "f18", 0x81 },
	{ "f19", 0x82 },
	{ "f20", 0x83 },
	{ "f21", 0x84 },
	{ "f22", 0x85 },
	{ "f23", 0x86 },
	{ "f24", 0x87 },
	{ "numlock", 0x90 },
	{ "scrolllock", 0x91 },
	{ "left_shift", 0xa0 },
	{ "right_shift", 0xa1 },
	{ "left_ctrl", 0xa2 },
	{ "right_ctrl", 0xa3 },
	{ "left_menu", 0xa4 },
	{ "right_menu", 0xa5 },
	{ ";", 0xba },
	{ ",", 0xbc },
	{ "-", 0xbd },
	{ ".", 0xbe },
	{ "/", 0xbf },
	{ "`", 0xc0 },
	{ "[", 0xdb },
	{ "\\", 0xdc },
	{ "]", 0xdd },
	{ "'", 0xde }
	// TODO: extra bullshit keys
};

template <typename GameEvent>
std::unordered_map<std::string, Input::KeyEvent::Type> 
InputMapper<GameEvent>::s_actionNameToAction = {
	{ "keydown", Input::KeyEvent::KeyDown },
	{ "keyup", Input::KeyEvent::KeyUp }
};
