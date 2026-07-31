#include "test_relay.h"

#include "relay.h"
#include "test_utils.h"
#include "clock.h"

bool test_relay_run(void)
{
    printf("\n=============== RELAY TEST ===============\n");

    relay_test_reset();

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

    relay_test_reset();

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

    /*
     * --------------------------------
     * Anti-cycle protection
     * --------------------------------
     */

    /* CORRECTION : Initialiser l'horloge AVANT d'initialiser le relais */
    clock_init();

    clock_time_t t =
        {
            .year = 2026,
            .month = 1,
            .day = 1,
            .hour = 12,
            .minute = 0,
            .second = 0};

    clock_set_time(&t);
           clock_sync_to_runtime();
    clock_dump();

    relay_init();
    relay_set_min_switch_delay(180);

    /* Première commutation autorisée */
    ASSERT_TRUE(relay_set(true));

    /* Immédiatement après : interdit */
    ASSERT_FALSE(relay_can_switch());

    clock_add_seconds(100);
    clock_dump();
    ASSERT_FALSE(relay_can_switch());

    clock_add_seconds(80);
    clock_dump();
    ASSERT_TRUE(relay_can_switch());

    /* La commutation OFF est maintenant autorisée */
    ASSERT_TRUE(relay_set(false));

    printf("PASS : Anti-cycle\n");

    return true;
}
