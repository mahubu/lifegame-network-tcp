#pragma once

#include <cstdint>

namespace lifegame
{
	struct Cell
	{
		/**
		* Describes the cell alive (or dead) state.
		*
		* A value > 0 stands for an alive cell, a value = 0 stand for a dead cell.
		*/
		uint8_t alive;
	};
}