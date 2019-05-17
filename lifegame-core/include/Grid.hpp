#pragma once

#include <Dish.hpp>
///#include <SFML/Graphics.hpp>

// TODO Display dish as a grid
namespace lifegame
{

	/**
	* A representation of a Petri dish as a grid.
	*/
	class Grid //: public sf::Drawable, public sf::Transformable
	{
	public:
		/**
		* Create a new grid with a given dish & a given dimension for cells.
		*
		* @param dish the associated dish.
		* @param cellDimension the dimension for cells.
		*/
		Grid(const Dish& dish, const uint8_t cellDimension);

	private:
		Dish dish_;
		//sf::VertexArray vertices_; // the vertices used to draw the grid.
		//virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	};
}