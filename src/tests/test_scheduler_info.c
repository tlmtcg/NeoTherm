#include "test_scheduler_info.h"

#include <stdio.h>

#include "scheduler.h"
#include "test_utils.h"

static void task1(void)
{
}

static void task2(void)
{
}

bool test_scheduler_info_run(void)
{
    printf("\n=============== SCHEDULER INFO TEST ===============\n");

    scheduler_init();

    ASSERT_TRUE(
        scheduler_register(
            "Task1",
            task1,
            5));

    ASSERT_TRUE(
        scheduler_register(
            "Task2",
            task2,
            10));

    scheduler_info_t info;

    /*
     * Première tâche
     */
    ASSERT_TRUE(
        scheduler_get_info(
            0,
            &info));

    ASSERT_EQ_STRING(
        "Task1",
        info.name);

    ASSERT_EQ_UINT32(
        5,
        info.period);

    ASSERT_EQ_UINT32(
        0,
        info.counter);

    ASSERT_TRUE(
        info.enabled);

    /*
     * Deuxième tâche
     */
    ASSERT_TRUE(
        scheduler_get_info(
            1,
            &info));

    ASSERT_EQ_STRING(
        "Task2",
        info.name);

    ASSERT_EQ_UINT32(
        10,
        info.period);

    ASSERT_EQ_UINT32(
        0,
        info.counter);

    ASSERT_TRUE(
        info.enabled);

    /*
     * Vérifie que le compteur évolue
     */
    scheduler_update();

    ASSERT_TRUE(
        scheduler_get_info(
            0,
            &info));

    ASSERT_EQ_UINT32(
        1,
        info.counter);

    ASSERT_TRUE(
        scheduler_get_info(
            1,
            &info));

    ASSERT_EQ_UINT32(
        1,
        info.counter);

    /*
     * Index invalide
     */
    ASSERT_FALSE(
        scheduler_get_info(
            2,
            &info));

    /*
     * Pointeur NULL
     */
    ASSERT_FALSE(
        scheduler_get_info(
            0,
            NULL));

    printf("SCHEDULER INFO TEST PASS\n");

    return true;
}