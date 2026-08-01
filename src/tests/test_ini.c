#include "test_ini.h"

#include <stdio.h>

#include "ini.h"
#include "test_utils.h"
#include "logger.h"

bool test_ini_run(void)
{
    printf("\n=============== INI TEST ===============\n");

    /*
     * Chargement
     */

    ASSERT_TRUE(
        ini_init("../test.ini"));

    const ini_runtime_t *rt =
        ini_get_runtime();

    ASSERT_TRUE(rt != NULL);

    ASSERT_TRUE(rt->initialized);

    ASSERT_TRUE(rt->count > 0);

    /*
     * String
     */

    char str[64];

    ASSERT_TRUE(
        ini_get_string(
            "general",
            "name",
            str,
            sizeof(str)));

    ASSERT_EQ_STRING(
        "NeoTherm",
        str);

    /*
     * Int
     */

    int version;

    ASSERT_TRUE(
        ini_get_int(
            "general",
            "version",
            &version));

    ASSERT_EQ_INT(
        6,
        version);

    /*
     * Float
     */

    float temperature;

    ASSERT_TRUE(
        ini_get_float(
            "float",
            "temperature",
            &temperature));

    ASSERT_EQ_FLOAT(
        20.5f,
        temperature);

    /*
     * Bool true
     */

    bool enabled;

    ASSERT_TRUE(
        ini_get_bool(
            "bool",
            "enabled",
            &enabled));

    ASSERT_TRUE(enabled);

    /*
     * Bool false
     */

    bool disabled;

    ASSERT_TRUE(
        ini_get_bool(
            "bool",
            "disabled",
            &disabled));

    ASSERT_FALSE(disabled);

    /*
     * Clé inexistante
     */

    ASSERT_FALSE(
        ini_get_string(
            "general",
            "unknown",
            str,
            sizeof(str)));

    ASSERT_FALSE(
        ini_get_int(
            "general",
            "unknown",
            &version));

    ASSERT_FALSE(
        ini_get_float(
            "general",
            "unknown",
            &temperature));

    ASSERT_FALSE(
        ini_get_bool(
            "general",
            "unknown",
            &enabled));

    /*
     * Section inexistante
     */

    ASSERT_FALSE(
        ini_get_string(
            "unknown",
            "name",
            str,
            sizeof(str)));

    /*
     * Paramètres NULL
     */

    ASSERT_FALSE(
        ini_get_string(
            NULL,
            "name",
            str,
            sizeof(str)));

    ASSERT_FALSE(
        ini_get_string(
            "general",
            NULL,
            str,
            sizeof(str)));

    ASSERT_FALSE(
        ini_get_string(
            "general",
            "name",
            NULL,
            sizeof(str)));

    ASSERT_FALSE(
        ini_get_string(
            "general",
            "name",
            str,
            0));

    ASSERT_FALSE(
        ini_get_int(
            "general",
            "version",
            NULL));

    ASSERT_FALSE(
        ini_get_float(
            "float",
            "temperature",
            NULL));

    ASSERT_FALSE(
        ini_get_bool(
            "bool",
            "enabled",
            NULL));

    /*
     * Ecriture String
     */

    ASSERT_TRUE(
        ini_set_string(
            "general",
            "name",
            "NeoTherm V6"));

    ASSERT_TRUE(
        ini_get_string(
            "general",
            "name",
            str,
            sizeof(str)));

    ASSERT_EQ_STRING(
        "NeoTherm V6",
        str);

    /*
     * Ecriture Int
     */

    ASSERT_TRUE(
        ini_set_int(
            "general",
            "version",
            7));

    ASSERT_TRUE(
        ini_get_int(
            "general",
            "version",
            &version));

    ASSERT_EQ_INT(
        7,
        version);

    /*
     * Ecriture Float
     */

    ASSERT_TRUE(
        ini_set_float(
            "float",
            "temperature",
            18.75f));

    ASSERT_TRUE(
        ini_get_float(
            "float",
            "temperature",
            &temperature));

    ASSERT_EQ_FLOAT(
        18.75f,
        temperature);

    /*
     * Ecriture Bool
     */

    ASSERT_TRUE(
        ini_set_bool(
            "bool",
            "enabled",
            false));

    ASSERT_TRUE(
        ini_get_bool(
            "bool",
            "enabled",
            &enabled));

    ASSERT_FALSE(
        enabled);

    /*
     * Création d'une nouvelle clé
     */

    ASSERT_TRUE(
        ini_set_string(
            "general",
            "author",
            "Raymonde"));

    ASSERT_TRUE(
        ini_get_string(
            "general",
            "author",
            str,
            sizeof(str)));

    ASSERT_EQ_STRING(
        "Raymonde",
        str);

    /*
     * Paramètres NULL
     */

    ASSERT_FALSE(
        ini_set_string(
            NULL,
            "key",
            "value"));

    ASSERT_FALSE(
        ini_set_string(
            "general",
            NULL,
            "value"));

    ASSERT_FALSE(
        ini_set_string(
            "general",
            "key",
            NULL));

    /*
     * Fichier inexistant
     */

    ASSERT_FALSE(
        ini_init(
            "../file_not_found.ini"));

    /*
     * Vérifie que le runtime est bien réinitialisé
     */

    ASSERT_TRUE(
        ini_init(
            "../test2.ini"));

    ASSERT_FALSE(
        ini_get_string(
            "general",
            "name",
            str,
            sizeof(str)));

    ASSERT_TRUE(
        ini_get_string(
            "new",
            "value",
            str,
            sizeof(str)));

    ASSERT_EQ_STRING(
        "OK",
        str);

    /*
     * Vérifie la sauvegarde
     */

    ASSERT_TRUE(
        ini_init(
            "../test.ini"));

    ASSERT_TRUE(
        ini_get_int(
            "general",
            "version",
            &version));

    ASSERT_EQ_INT(
        7,
        version);

    printf("\nPASS : INI\n");

    // remet les valeurs d'origine

    ASSERT_TRUE(
        ini_set_string(
            "general",
            "name",
            "NeoTherm"));

    ASSERT_TRUE(
        ini_set_int(
            "general",
            "version",
            6));

    ASSERT_TRUE(
        ini_set_float(
            "float",
            "temperature",
            20.5f));

    ASSERT_TRUE(
        ini_set_bool(
            "bool",
            "enabled",
            true));

    ASSERT_TRUE(
        ini_remove_key(
            "general",
            "author"));

    ASSERT_FALSE(
        ini_get_string(
            "general",
            "author",
            str,
            sizeof(str)));

    /* Test existance */

    ASSERT_TRUE(
        ini_key_exists(
            "general",
            "name"));

    ASSERT_FALSE(
        ini_key_exists(
            "general",
            "unknown"));

    ASSERT_FALSE(
        ini_key_exists(
            NULL,
            "name"));

    ASSERT_FALSE(
        ini_key_exists(
            "general",
            NULL));

    return true;
}
