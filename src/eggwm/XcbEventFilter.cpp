#include "XcbEventFilter.h"

bool XcbEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *)
{
    (void)eventType;

    if (eventFactory == NULL)
        return false;

    auto event = static_cast<xcb_generic_event_t*>(message);

    EventHandler* handler = eventFactory->getEventHandler(event->response_type & ~0x80);
    if (handler)
        handler->processEvent(event);

    return false;                                                              
}

