#include <Dish.hpp>
#include <assert.h>
#include <iostream>
#include <random>

using namespace std;

namespace lifegame
{

	/**
	* Create a new dish with given sizes & a given ratio of alive cells.
	*
	* 2 units of height & 2 units of width are added to the dish. They will contains immutable cells used for transition state computation.
	**/
	Dish::Dish(const uint8_t rows, const uint8_t columns, const uint8_t aliveCellsRatio) : rows_(rows + 2), columns_(columns + 2)
	{
		dish_ = new Cell * [rows_]; // dynamic `array of pointers to cells`
		for (auto r = 0; r < rows_; ++r)
		{
			dish_[r] = new Cell[columns_]; // each i-th pointer is now pointing to dynamic array of cells
			for (auto c = 0; c < columns_; ++c)
			{
				Cell cell;
				cell.alive = 0;
				dish_[r][c] = cell;
			}
		}

		random_device rd;     // used to initialise (seed) engine
		mt19937 rng(rd());    // random-number engine used (Mersenne-Twister here)
		uniform_int_distribution<int> rDistrib(0, rows_ - 1);
		uniform_int_distribution<int> cDistrib(0, columns_ - 1);

		int aliveCells = rows_ * columns_ * aliveCellsRatio / 100;
		while (aliveCells > 0)
		{
			int r = rDistrib(rng);
			int c = cDistrib(rng);
			Cell& cell = dish_[r][c];
			if (cell.alive != 1)
			{
				cell.alive = 1;
				--aliveCells;
			}
		}
	}

	Dish::Dish(const Dish& other)
	{
		rows_ = other.rows();
		columns_ = other.columns();
		dish_ = new Cell * [rows_]; // dynamic `array of pointers to cells`
		for (auto r = 0; r < rows_; ++r)
		{
			dish_[r] = new Cell[columns_]; // each i-th pointer is now pointing to dynamic array of cells
			for (auto c = 0; c < columns_; ++c)
			{
				Cell cell = other.cell(r, c);
				dish_[r][c] = cell;
			}
		}
	}

	Dish& Dish::operator=(const Dish& other)
	{
		// 1. Special case : same object.
		if (this == &other)
		{
			return *this;
		}

		// 2. Special case : height & width remain unmodified.
		if (rows_ == other.rows() && columns_ == other.columns())
		{
			for (auto r = 0; r < rows_; ++r)
			{
				for (auto c = 0; c < columns_; ++c)
				{
					dish_[r][c] = other.cell(r, c);
				}
			}
			return *this;
		}

		// 3. `By default` case.

		// Deallocate
		for (auto r = 0; r < rows_; ++r)
		{
			delete[] dish_[r];
		}
		delete[] dish_;

		// Allocate
		uint8_t rows = other.rows();
		uint8_t columns = other.columns();
		Cell** dish = new Cell * [rows]; // dynamic `array of pointers to cells`
		for (auto r = 0; r < rows; ++r)
		{
			dish[r] = new Cell[columns]; // each i-th pointer is now pointing to dynamic array of cells
			for (auto c = 0; c < columns; ++c)
			{
				Cell cell = other.cell(r, c);
				dish[r][c] = cell;
			}
		}

		// Assign
		rows_ = rows;
		columns_ = columns_;
		dish_ = dish;

		return *this;
	}

	Dish::Dish(Dish&& other) noexcept
	{
		// Assign
		rows_ = other.rows_;
		columns_ = other.columns_;
		dish_ = other.dish_;

		// Reset
		other.rows_ = 0;
		other.columns_ = 0;
		other.dish_ = nullptr;
	}

	Dish& Dish::operator=(Dish&& other) noexcept
	{
		// 1. Special case : same object.
		if (this == &other)
		{
			return *this;
		}

		// 2. `Not-so-special` case : height & width remain unmodified.
		// 3. `By default` case.

		// Deallocate
		for (auto r = 0; r < rows_; ++r)
		{
			delete[] dish_[r];
		}
		delete[] dish_;

		// Assign
		rows_ = other.rows_;
		columns_ = other.columns_;
		dish_ = other.dish_;

		// Reset
		other.rows_ = 0;
		other.columns_ = 0;
		other.dish_ = nullptr;

		return *this;
	}

	Dish::~Dish()
	{
		for (auto r = 0; r < rows_; ++r)
		{
			delete[] dish_[r];
		}
		delete[] dish_;
	}

	uint8_t Dish::rows() const
	{
		return rows_;
	}

	uint8_t Dish::columns() const
	{
		return columns_;
	}

	void Dish::cells(std::vector<uint8_t>& cells) const
	{
		for (auto r = 0; r < rows_; ++r)
		{
			for (auto c = 0; c < columns_; ++c)
			{
				uint8_t living = alive(r, c);
				cells.push_back(r);
				cells.push_back(c);
				cells.push_back(living);
			}
		}
	}

	Cell Dish::cell(const uint8_t row, const uint8_t column) const
	{
		assert(row < rows_);
		assert(column < columns_);
		return dish_[row][column];
	}

	bool Dish::cell(const uint8_t row, const uint8_t column, const uint8_t state)
	{
		assert(row < rows_);
		assert(column < columns_);

		// Cells outside of the living grid are immutable.
		if (row == 0 || row == rows_ - 1 || column == 0 || column == columns_ - 1)
		{
			return false;
		}

		Cell& cell = dish_[row][column];
		if (cell.alive != state)
		{
			cell.alive = state;
			return true;
		}

		return false;
	}

	void Dish::live(std::vector<uint8_t>& cells)
	{
		for (auto r = 0; r < rows_; ++r)
		{
			for (auto c = 0; c < columns_; ++c)
			{
				uint8_t lived = alive(r, c);
				uint8_t living = live(r, c);
				if (lived != living) {
					cells.push_back(r);
					cells.push_back(c);
					cells.push_back(living);
				}
				cell(r, c, living);
			}
		}
	}

	void Dish::modify(const std::vector<uint8_t>& cells)
	{
		assert(cells.size() % 3 == 0);
		for (unsigned int i = 0; i < cells.size(); i = i + 3)
		{
			cell(cells.at(i), cells.at(i + 1), cells.at(i + 2));
		}
	}

	uint8_t Dish::live(const uint8_t row, const uint8_t column)
	{
		assert(row < rows_);
		assert(column < columns_);

		// Cells outside of the living grid are immutable.
		if (row == 0 || row == rows_ - 1 || column == 0 || column == columns_ - 1)
		{
			return Dish::alive(row, column);
		}

		Cell topleftCell = Dish::cell(row - 1, column - 1);
		Cell topCell = Dish::cell(row - 1, column);
		Cell toprightCell = Dish::cell(row - 1, column + 1);

		Cell leftCell = Dish::cell(row, column - 1);
		Cell rightCell = Dish::cell(row, column + 1);

		Cell botleftCell = Dish::cell(row + 1, column - 1);
		Cell botCell = Dish::cell(row + 1, column);
		Cell botrightCell = Dish::cell(row + 1, column + 1);

		uint8_t aliveCells = topleftCell.alive + topCell.alive + toprightCell.alive + leftCell.alive + rightCell.alive + botleftCell.alive + botCell.alive + botrightCell.alive;

		Cell cell = Dish::cell(row, column);
		if (aliveCells == 3 || (cell.alive == 1 && aliveCells == 2))
		{
			return 1; // 1 stands for "alive cell".
		}
		else
		{
			return 0; // 0 stands for "dead cell".
		}
	}

	uint8_t Dish::alive(const uint8_t row, const uint8_t column) const
	{
		return Dish::cell(row, column).alive;
	}

}
