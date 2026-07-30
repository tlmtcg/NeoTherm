#include "test_relay.h"

#include "relay.h"
#include "test_utils.h"

bool test_relay_run(void)
{
    printf("\n=============== RELAY TEST ===============\n");

    relay_init();

    /*
     * Etat initial
     */

    ASSERT_FALSE(relay_get());

    /*
     * ON
     */

    relay_set(true);

    ASSERT_TRUE(relay_get());

    /*
     * OFF
     */

    relay_set(false);

    ASSERT_FALSE(relay_get());

    printf("Relay OFF : OFF\n");

    /*
     * Plusieurs basculements
     */

    for (int i = 0; i < 5; i++)
    {
        relay_set(true);
        ASSERT_TRUE(relay_get());

        relay_set(false);
        ASSERT_FALSE(relay_get());
    }

    /*
     * Idempotence
     * Deux commandes identiques ne doivent rien casser.
     */

    relay_set(false);
    ASSERT_FALSE(relay_get());

    relay_set(false);
    ASSERT_FALSE(relay_get());

    relay_set(true);
    ASSERT_TRUE(relay_get());

    relay_set(true);
    ASSERT_TRUE(relay_get());

    ASSERT_EQ_UINT32(13, relay_get_switch_count());

    relay_init();

    /*
     * OFF -> ON
     */

    relay_set(true);

    ASSERT_TRUE(relay_get());
    ASSERT_EQ_UINT32(1, relay_get_switch_count());

    /*
     * ON -> ON
     */

    relay_set(true);

    ASSERT_TRUE(relay_get());
    ASSERT_EQ_UINT32(1, relay_get_switch_count());

    /*
     * ON -> OFF
     */

    relay_set(false);

    ASSERT_FALSE(relay_get());
    ASSERT_EQ_UINT32(2, relay_get_switch_count());

    /*
     * OFF -> OFF
     */

    relay_set(false);

    ASSERT_FALSE(relay_get());
    ASSERT_EQ_UINT32(2, relay_get_switch_count());

    /*
     * Toggle
     */

    relay_toggle();

    ASSERT_TRUE(relay_get());
    ASSERT_EQ_UINT32(3, relay_get_switch_count());

    relay_toggle();

    ASSERT_FALSE(relay_get());
    ASSERT_EQ_UINT32(4, relay_get_switch_count());

    printf("\nPASS : Relay\n");

    return true;
}
