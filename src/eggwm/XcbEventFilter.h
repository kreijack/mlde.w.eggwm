#include "src/eggwm/events/factory/EventFactory.h"

class XcbEventFilter : public QAbstractNativeEventFilter                         
{                                                                                  
public:
    XcbEventFilter(EventFactory* eventFactory_) : eventFactory(eventFactory_ ){};
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *) Q_DECL_OVERRIDE;

private:
    EventFactory* eventFactory = nullptr;
};   
