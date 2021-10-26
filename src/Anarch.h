#ifndef ANARCH_ANARCH_H
#define ANARCH_ANARCH_H

#include <Arduino.h>
#include <CircuitOS.h>
#include <Input/I2cExpander.h>
#include <Input/InputI2C.h>
#include <Loop/LoopManager.h>
#include <Support/Context.h>

class Anarch : public Context, public LoopListener{
public:
	Anarch(Display* display);

	~Anarch() override;

	void draw() override;

	void start() override;

	void stop() override;

	void loop(uint micros) override;

	void drawPixel(uint16_t x, uint16_t y, uint16_t colorIndex);

private:
	Sprite *baseSprite;
};

#endif //ANARCH_ANARCH_H
