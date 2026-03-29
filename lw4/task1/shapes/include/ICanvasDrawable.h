#pragma once

class ICanvas;

class ICanvasDrawable {
public:
	virtual ~ICanvasDrawable() = default;

	virtual void Draw(ICanvas& canvas) const = 0;
};