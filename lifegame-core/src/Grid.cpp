#include <Grid.hpp>
#include <cstdint>

// TODO Display dish as a grid
namespace lifegame
{

	Grid::Grid(const Dish& dish, const uint8_t cellDimension) : dish_(dish)
	{
		uint8_t rows = dish_.rows();
		uint8_t columns = dish_.columns();

		/*
		// Create a vertex array of quads.
		vertices_.setPrimitiveType(sf::Quads);
		vertices_.resize(rows * columns * 4);

		uint8_t width = cellDimension;
		uint8_t height = cellDimension;

		for (auto r = 0; r < rows; ++r)
		{
			for (auto c = 0; c < columns; ++c)
			{
				// Get a quad from the vertex array for given coordinates.
				sf::Vertex* quad = &vertices_[(r + c * rows) * 4];

				// Define quad positions.
				quad[0].position = sf::Vector2f(r * width, c * height);
				quad[1].position = sf::Vector2f((r + 1) * width, c * height);
				quad[2].position = sf::Vector2f((r + 1) * width, (c + 1) * height);
				quad[3].position = sf::Vector2f(r * width, (c + 1) * height);

				// Define quad colours.
				sf::Color color = sf::Color::Red;
				if (dish_.alive(r, c))
				{
					color = std::move(sf::Color::Green);
				}
				quad[0].color = color;
				quad[1].color = color;
				quad[2].color = color;
				quad[3].color = color;
			}
		}
		*/
	}

	/*
	void Grid::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform *= getTransform();
		target.draw(vertices_, states);
	}
	*/

}
