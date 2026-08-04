#ifndef EVENT_DISPATCHER_H
#define EVENT_DISPATCHER_H

#include "event.h"

/*
 * Initialisation
 */
bool event_dispatcher_init(void);

/*
 * Traite un événement
 */
void event_dispatcher_dispatch(
    const event_t *event);

#endif /* EVENT_DISPATCHER_H */