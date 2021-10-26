#include "Anarch.h"

extern uint8_t SFG_mainLoopBody();


Anarch::Anarch(Display* display) : Context(*display), baseSprite(screen.getSprite()){

}

Anarch::~Anarch(){

}

void Anarch::draw(){

}

void Anarch::start(){
	LoopManager::addListener(this);
}

void Anarch::stop(){

}

void Anarch::drawPixel(uint16_t x, uint16_t y, uint16_t colorIndex){
	baseSprite->drawPixel(x, y, colorIndex);
}

void Anarch::loop(uint micros){
	SFG_mainLoopBody();
	draw();
	screen.commit();
}
