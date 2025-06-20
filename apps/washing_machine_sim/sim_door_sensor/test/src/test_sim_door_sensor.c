#include <zephyr/ztest.h>
#include "sim_door_sensor.h"

/**
 * @brief A one-time setup function that runs once before the entire suite.
 *
 * This is the correct place for one-time initialization.
 */
static void *suite_setup(void)
{
	int ret;

	/* Initialize the component under test. */
	ret = door_sensor_sim_init();
	zassert_ok(ret, "Failed to initialize door sensor simulation in setup");

	return NULL;
}

/**
 * @brief A function that runs before each individual test case.
 *
 * This is the correct place to reset the state for each test, preventing
 * one test from affecting another.
 */
static void test_before(void *data)
{
	ARG_UNUSED(data);
	int ret;

	/* Set a known starting state before every test. */
	ret = door_sensor_sim_set_state(false); /* Start with door open */
	zassert_ok(ret, "Failed to set initial state in test_before");
}


/**
 * @brief Test case for the door sensor in the 'open' state.
 */
ZTEST(wachine_sim_suite, test_door_sensor_open)
{
	/* The test_before() function ensures the sensor starts in the OPEN state. */
	zassert_false(door_sensor_sim_get_state(), "Sensor should be in OPEN state");
}

/**
 * @brief Test case for the door sensor in the 'closed' state.
 */
ZTEST(wachine_sim_suite, test_door_sensor_closed)
{
	int ret;

	/* Action: Set the sensor state to closed. */
	ret = door_sensor_sim_set_state(true);
	zassert_ok(ret, "Failed to set door sensor state to closed");

	/* Assert: Verify the state is now closed. */
	zassert_true(door_sensor_sim_get_state(), "Sensor should be in CLOSED state");
}


/**
 * @brief Define the test suite, using both suite_setup and test_before.
 *
 * ZTEST_SUITE(name, predicate, suite_setup, test_before, test_after, suite_teardown);
 */
ZTEST_SUITE(wachine_sim_suite, NULL, suite_setup, test_before, NULL, NULL);

