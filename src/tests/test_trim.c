#include "test_trim.h"
#include <stdio.h>
#include "../app_config/parser/ini_parser.h"
#include "test_utils.h"

typedef struct
{
    const char *input;
    const char *expected;

} trim_test_t;

bool test_trim_run(void)
{
    printf("\n=============== TRIM TEST ===============\n");

    bool result = true;

    const trim_test_t tests[] =
    {
        { "   hello",       "hello"      },
        { "hello   ",       "hello"      },
        { "   hello   ",    "hello"      },
        { "     ",          ""           },
        { "",               ""           },
        { "\thello\t",      "hello"      },
        { "\r\nhello",      "hello"      },
        { "hello",          "hello"      },
        { "  thermostat  ", "thermostat" },
        { "\tAUTO\t",       "AUTO"       },
        { "42",             "42"         },
    };

    const size_t count =
        sizeof(tests) / sizeof(tests[0]);

    for(size_t i = 0; i < count; i++)
    {
        char buffer[64];

        strcpy(buffer, tests[i].input);

        char *trimmed = trim(buffer);

        if(strcmp(trimmed, tests[i].expected) != 0)
        {
            printf("FAIL [%zu] : input=\"%s\" expected=\"%s\" got=\"%s\"\n",
                   i,
                   tests[i].input,
                   tests[i].expected,
                   trimmed);

            result = false;
        }
    }

    if(result)
    {
        ASSERT_SUCCESS("Trim");
    }

    return result;
}
