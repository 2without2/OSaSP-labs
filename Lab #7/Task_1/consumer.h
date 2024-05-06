#pragma once

#include "buffer.h"

#ifndef TASK_1_CONSUMER_H
#define TASK_1_CONSUMER_H

void createConsumer();
void removeConsumer();
void validateHash(msg_t* msg);

#endif //TASK_1_CONSUMER_H
