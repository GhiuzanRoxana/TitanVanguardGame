module bomb;

Bomb::Bomb(Coordinates position)
	: position(position){}

Coordinates Bomb::GetPosition() const
{
	return position;
}

bool Bomb::GetStatus() const
{
	return isActive;
}

void Bomb::SetStatus(bool status)
{
	isActive = status;
}
