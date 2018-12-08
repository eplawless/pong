#pragma once

#include <vector>

enum class PongEvent
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

using PongEventList = std::vector<PongEvent>;
