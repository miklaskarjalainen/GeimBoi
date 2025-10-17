// SPDX-License-Identifier: GPL-2.0-only

#include <unity.h>

extern void test_opcode_bits(void);
extern void test_opcode_jp(void);
extern void test_opcode_sp(void);
extern void test_opcode_special(void);

void setUp(void)
{
	// set stuff up here
}

void tearDown(void)
{
	// clean stuff up here
}

// not needed when using generate_test_runner.rb
int main(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_opcode_bits);
	RUN_TEST(test_opcode_jp);
	RUN_TEST(test_opcode_sp);
	RUN_TEST(test_opcode_special);
	return UNITY_END();
}
