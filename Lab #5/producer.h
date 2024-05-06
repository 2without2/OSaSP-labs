//
// Created by user on 5/17/23.
//

#ifndef LAB_5_PRODUCER_H
#define LAB_5_PRODUCER_H

#include "buffer.h"
_Noreturn void producerHandler();
void createProducer();
void removeProducer();
void createMessage(msg_t* msg);

#endif //LAB_5_PRODUCER_H
