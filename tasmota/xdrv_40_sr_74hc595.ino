/*
  xdrv_40_shiftregister.ino - shift register support for Tasmota

  Copyright (C) 2020  Peter Halicky

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifdef USE_SR_74HC595
#define XDRV_40 40

void ShiftRegisterInit(void) {
  if (Settings.sr74hc595_count<1)
    Settings.sr74hc595_count=1; // set default

  if (PinUsed(GPIO_SR_OE)) { // disable output
    pinMode(Pin(GPIO_SR_OE), OUTPUT);
    digitalWrite(Pin(GPIO_SR_OE), HIGH);
  }

  // initialize output pins
  pinMode(Pin(GPIO_SR_CLOCK), OUTPUT);
  pinMode(Pin(GPIO_SR_DATA), OUTPUT);
  pinMode(Pin(GPIO_SR_LATCH), OUTPUT);
  digitalWrite(Pin(GPIO_SR_CLOCK), LOW);
  digitalWrite(Pin(GPIO_SR_DATA), LOW);
  digitalWrite(Pin(GPIO_SR_LATCH), LOW);

  // 8 outputs per shift register chip
  devices_present=Settings.sr74hc595_count*8;
}

bool ShiftRegisterSetPower(void) {
  // data out by 8 bits
  for (int i=Settings.sr74hc595_count-1; i>=0; i--) {
    uint8_t data = (uint8_t)(power>>(8*i) & 0xff);
    shiftOut(Pin(GPIO_SR_DATA), Pin(GPIO_SR_CLOCK), MSBFIRST, data);
  }

  // pulse latch to activate new values
  digitalWrite(Pin(GPIO_SR_LATCH), HIGH); 
  digitalWrite(Pin(GPIO_SR_LATCH), LOW); 

  // enable output if configured
  if (PinUsed(GPIO_SR_OE)) {
    digitalWrite(Pin(GPIO_SR_OE), LOW);
  }

  return true;
}

bool Xdrv40(uint8_t function)
{
  bool result = false;

  if (PinUsed(GPIO_SR_CLOCK) && PinUsed(GPIO_SR_DATA) && PinUsed(GPIO_SR_LATCH) && PinUsed(GPIO_SR_OE)) {
    switch (function) {
      case FUNC_PRE_INIT:
        ShiftRegisterInit();
        break;
      case FUNC_SET_DEVICE_POWER:
        result = ShiftRegisterSetPower();
        break;
    }
  }
  return result;
}

#endif // USE_SHIFT_REGISTER