#pragma once

#include <vector>

enum class GameEvent
{
	None,
	LeftPaddleMoveUp,
	LeftPaddleMoveDown,
	LeftPaddleStop,
	RightPaddleMoveUp,
	RightPaddleMoveDown,
	RightPaddleStop,
	Pause,
	Quit,
	ToggleDebugOverlay
};

using GameEventList = std::vector<GameEvent>;
