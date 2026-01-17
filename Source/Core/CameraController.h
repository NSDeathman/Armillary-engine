#pragma once
#include "CoreAPI.h"
#include "Camera.h"

namespace Core::World
{
// Èםעונפויס
class ICameraController
{
  public:
	virtual ~ICameraController() = default;
	virtual void Update(CCamera& camera, float dt) = 0;
};
} // namespace Core::World
