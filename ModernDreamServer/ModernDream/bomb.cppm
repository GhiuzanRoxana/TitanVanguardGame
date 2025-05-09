export module bomb;
#include <utility>
#include <cstdint>

export using Coordinates = std::pair<std::uint16_t, std::uint16_t>;

export class Bomb
{

public:
	Bomb(Coordinates position);
	Coordinates GetPosition() const;
	bool GetStatus() const;
	void SetStatus(bool status);

private:

	Coordinates position;
	bool isActive{ true };
};
