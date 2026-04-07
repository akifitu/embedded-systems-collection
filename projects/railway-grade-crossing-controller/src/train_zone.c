#include "train_zone.h"

train_zone_t train_zone_eval(bool approach_active, bool island_occupied) {
    if (island_occupied) {
        return TRAIN_ZONE_ISLAND;
    }
    if (approach_active) {
        return TRAIN_ZONE_APPROACH;
    }
    return TRAIN_ZONE_CLEAR;
}
