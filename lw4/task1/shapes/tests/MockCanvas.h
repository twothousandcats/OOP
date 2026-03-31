#pragma once
#include "ICanvas.h"
#include "fakeit/fakeit.h"

struct MockCanvas final : fakeit::Mock<ICanvas>
{
	using Mock::Mock;
};