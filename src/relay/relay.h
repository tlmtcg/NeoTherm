#ifndef RELAY_H
#define RELAY_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


/*==========================================================
 * Initialisation
 *=========================================================*/

bool relay_init(void);


/*==========================================================
 * Commande
 *=========================================================*/

bool relay_set(bool state);

bool relay_toggle(void);


/*==========================================================
 * Lecture
 *=========================================================*/

bool relay_get(void);


#ifdef __cplusplus
}
#endif

#endif /* RELAY_H */