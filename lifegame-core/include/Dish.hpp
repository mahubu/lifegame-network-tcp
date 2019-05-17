#pragma once

#include <Cell.hpp>
#include <cstdint>
#include <vector>

namespace lifegame
{

	/**
	* A Petri dish, filled with alive & dead cells.
	**/
	class Dish
	{

	public:
		/**
		* Create a new dish with given sizes & a given ratio of alive cells.
		*
		* @param rows the dish height.
		* @param columns the dish width.
		* @param aliveCellsRatio the dish ratio of alive cells.
		*/
		Dish(const uint8_t rows, const uint8_t columns, const uint8_t aliveCellsRatio);
		Dish(const Dish& dish);
		Dish& operator=(const Dish& dish);
		Dish(Dish&& dish) noexcept;
		Dish& operator=(Dish&& dish) noexcept;
		~Dish();

		uint8_t rows() const;
		uint8_t columns() const;
		void cells(std::vector<uint8_t>& cells) const;
		/**
		* Retrieve a copy of a cell in the dish.
		*
		* @param row the "row" coordinate (= y) of the wanted cell.
		* @param column the "column" coordinate (= x) of the wanted cell.
		* @return a copy of the wanted cell.
		*/
		Cell cell(const uint8_t row, const uint8_t column) const;
		/**
		* Retrieve the state of a cell in the dish.
		* A value > 0 stands for an alive cell, a value = 0 stand for a dead cell.
		*
		* @param row the "row" coordinate (= y) of the wanted cell.
		* @param column the "column" coordinate (= x) of the wanted cell.
		* @return the state of the wanted cell.
		*/
		uint8_t alive(const uint8_t row, const uint8_t column) const;
		/**
		* Let the dish live & cells become alive or dead.
		* 
		* @param the cells whom state has changed : first 'row' coordinate, second 'column' coordinate, third cell state.
		*/
		void live(std::vector<uint8_t>& cells);
		/**
		* Modify the dish with given cells.
		*
		* @param the cells to apply to the dish : first 'row' coordinate, second 'column' coordinate, third cell state.
		*/
		void modify(const std::vector<uint8_t>& cells);

	private:
		uint8_t rows_;
		uint8_t columns_;
		// TODO use std::vector instead ?
		Cell** dish_;

		/**
		* Set the state of a cell in the dish.
		* A value > 0 stands for an alive cell, a value = 0 stand for a dead cell.
		*
		* @param row the "row" coordinate (= y) of the cell.
		* @param column the "column" coordinate (= x) of the cell.
		* @param state the state of the cell.
		* @return whether the cell state has changed.
		*/
		bool cell(const uint8_t row, const uint8_t column, const uint8_t state);
		/**
		* Let a cell live & become alive or dead.
		*
		* @param row the "row" coordinate (= y) of the cell.
		* @param column the "column" coordinate (= x) of the cell.
		* @param state the new state of the cell.
		*/
		uint8_t live(const uint8_t row, const uint8_t column);

	};

}