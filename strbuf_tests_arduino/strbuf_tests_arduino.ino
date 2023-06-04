/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* strbuf_tests_arduino.ino */
/* Copyright (C) 2020 Eric Herman */

#include <Arduino.h>
#include <HardwareSerial.h>

#include "eembed-arduino.h"

unsigned test_append_float(void);
unsigned test_append_int(void);
unsigned test_append_uint(void);
unsigned test_append(void);
unsigned test_avail(void);
unsigned test_new_no_grow(void);
unsigned test_prepend_float(void);
unsigned test_prepend_int(void);
unsigned test_prepend_uint(void);
unsigned test_prepend(void);
unsigned test_trim(void);
unsigned test_expose_return(void);

void setup(void)
{
	Serial.begin(9600);
	// wait to ensure stable Serial initialziaiton
	delay(50);

	eembed_arduino_serial_log_init();

	Serial.println();
	Serial.println("==================================================");
	Serial.println();
	Serial.println("Begin");
	Serial.println();
	Serial.println("==================================================");
}

unsigned test_func(const char *name, unsigned (*pfunc)(void))
{
	Serial.print(name);
	Serial.print(" ...");

	unsigned fail = pfunc();

	Serial.println(fail ? " FAIL!" : " ok.");
	return fail;
}

#define Test_func(func) test_func(#func, func);

unsigned loop_count = 0;
void loop(void)
{
	++loop_count;

	Serial.println();
	Serial.println("=================================================");
	Serial.print("Starting run #");
	Serial.println(loop_count);
	Serial.println("=================================================");

	unsigned failures = 0;

	failures += Test_func(test_append_float);
	failures += Test_func(test_append_int);
	failures += Test_func(test_append_uint);
	failures += Test_func(test_append);
	failures += Test_func(test_avail);
	failures += Test_func(test_expose_return);
	failures += Test_func(test_new_no_grow);
	failures += Test_func(test_prepend_float);
	failures += Test_func(test_prepend_int);
	failures += Test_func(test_prepend_uint);
	failures += Test_func(test_prepend);
	failures += Test_func(test_trim);

	Serial.println("=================================================");
	if (failures) {
		Serial.print("failures = ");
		Serial.println(failures);
		Serial.println("FAIL");
	} else {
		Serial.println("SUCCESS");
	}
	Serial.println("=================================================");

	Serial.println();
	uint16_t loop_delay_seconds = failures ? 10 : 3;
	uint16_t loop_delay_ms = (loop_delay_seconds * 1000);
	for (int i = 0; i < 20; ++i) {
		Serial.print(".");
		delay(loop_delay_ms / 20);
	}
	Serial.println();
}
