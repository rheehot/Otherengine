#include "actors/ship.h"
#include <array>
#include "components/sprite_component.h"
#include "components/input_component.h"
#include "components/pawn_move_comp.h"

namespace game
{
	static constexpr std::array ship_png{"Assets/Ship.png", "Assets/ShipWithThrust.png"};
	
	struct input_forward : input_axis
	{
		[[nodiscard]] std::vector<axis_t> keys() const noexcept override
		{
			return {{'w', 1}, {'s', -1}};
		}
	};
	
	struct input_rotate : input_axis
	{
		[[nodiscard]] std::vector<axis_t> keys() const noexcept override
		{
			return {{'a', -1}, {'d', 1}};
		}
	};

	ship::ship(application& app)
		:actor{app}
	{
		auto& sprite = add_component<sprite_component>();
		sprite.set_texture(ship_png[0]);

		auto& movement = add_component<pawn_move_comp>();

		auto& input = add_component<input_component>();
		input.bind_axis(input_forward{}, [&](float f)
		{
			const auto should_move = !math::is_nearly_zero(f);
			if (should_move) movement.add_input(get_forward() * f);

			if (should_move != is_moving_)
			{
				sprite.set_texture(ship_png[should_move]);
				is_moving_ = should_move;
			}
		});

		input.bind_axis(input_rotate{}, [&](float f)
		{
			movement.add_rotation_input(f);
		});
	}
}
