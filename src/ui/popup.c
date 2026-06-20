#include "popup.h"
#include "data/definitions.h"

Popup* GenerateEmptyPopup() {
    return EZ_ALLOC(1, sizeof(Popup));
}

void CleanPopup(Popup* popup) {
    if (popup->options != 0)
        for (size_t i = 0; i < popup->options; i++)
            CleanPopup(((Popup**)popup->results)[i]);
    if (popup->options > 0) EZ_FREE(popup->results);
    EZ_FREE(popup);
}
