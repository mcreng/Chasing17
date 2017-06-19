/* main.cpp
 * Copyright (c) 2014-2017 HKUST SmartCar Team
 * Refer to LICENSE for details
 *
 * Author: David Mak (Derppening)
 *
 * Program entry point.
 *
 */

#include "libbase/k60/mcg.h"
#include "libsc/battery_meter.h"
#include "libsc/lcd_console.h"
#include "libsc/st7735r.h"
#include "libsc/system.h"

#include "algorithm/optimal.h"
#include "algorithm/distance.h"
#include "util/testground.h"
#include "util/util.h"

namespace libbase {
namespace k60 {
Mcg::Config Mcg::GetMcgConfig() {
  Mcg::Config config;
  config.external_oscillator_khz = 50000;
  config.core_clock_khz = 150000;
  return config;
}
}  // namespace k60
}  // namespace libbase

using libsc::BatteryMeter;
using libsc::Lcd;
using libsc::LcdConsole;
using libsc::St7735r;
using libsc::System;

enum struct Algorithm {
  kOptimal,
  kTestGround,
  kDistance
};

int main() {
  System::Init();

  BatteryMeter::Config ConfigBM;
  ConfigBM.voltage_ratio = 0.4;
  BatteryMeter bm(ConfigBM);

  // Battery Check
  {
    St7735r::Config lcd_config;
    lcd_config.is_revert = true;
    St7735r lcd(lcd_config);
    lcd.Clear();

    LcdConsole::Config console_config;
    console_config.lcd = &lcd;
    LcdConsole console(console_config);

    float voltage;
    do {
      voltage = bm.GetVoltage();

      console.SetTextColor(voltage <= 7.4 ? Lcd::kRed : Lcd::kGreen);

      char temp[32];
      sprintf(temp, " Voltage: %.2fV", voltage);
      util::ConsoleClearRow(&console, 0);
      console.WriteString(temp);

      System::DelayMs(1000);
    } while (voltage <= 7.4);
  }

  // modify next line to switch between algorithms
  constexpr Algorithm a = Algorithm::kOptimal;

  // modify next line to change which car we're working with
  CarManager::Car c = CarManager::Car::kCar2;

  switch (a) {
    case Algorithm::kOptimal:
      algorithm::optimal::main(c);
      break;
    case Algorithm::kTestGround:
      util::testground::main();
      break;
    case Algorithm::kDistance:
      algorithm::USIRDemo();
      break;
    default:
      // all cases covered
      break;
  }

  while (true);

  return 0;
}
