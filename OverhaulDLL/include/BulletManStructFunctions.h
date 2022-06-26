#pragma once
#ifndef BULLETMAN_STRUCTFUNCTIONS_H
#define BULLETMAN_STRUCTFUNCTIONS_H

#include <stdint.h>
#include "BulletManStruct.h"

void copy_BulletMan(BulletMan* to, BulletMan* from, bool to_game);
BulletMan* init_BulletMan();
void free_BulletMan(BulletMan* to);


#endif
