#pragma once
#ifndef QInputMgrWindows_STRUCTFUNCTIONS_H
#define QInputMgrWindows_STRUCTFUNCTIONS_H

#include <stdint.h>
#include "QInputMgrWindowsStruct.h"

void copy_QInputMgrWindows(QInputMgrWindows* to, QInputMgrWindows* from);
QInputMgrWindows* init_QInputMgrWindows();
void free_QInputMgrWindows(QInputMgrWindows* to);

void copy_QInputMgr(QInputMgr* to, QInputMgr* from);
QInputMgr* init_QInputMgr();
void free_QInputMgr(QInputMgr* to, bool free_self);

void copy_QMouse(QMouse* to, QMouse* from);
QMouse* init_QMouse();
void free_QMouse(QMouse* to);

void copy_QKeyboard(QKeyboard* to, QKeyboard* from);
QKeyboard* init_QKeyboard();
void free_QKeyboard(QKeyboard* to);

void copy_QXInputPad(QXInputPad* to, QXInputPad* from);
QXInputPad* init_QXInputPad();
void free_QXInputPad(QXInputPad* to);

void copy_QDirectInputPad(QDirectInputPad* to, QDirectInputPad* from);
QDirectInputPad* init_QDirectInputPad();
void free_QDirectInputPad(QDirectInputPad* to);

void copy_QInputStates_substruct_BDQ(QInputStates_substruct_BDQ* to, QInputStates_substruct_BDQ* from);
QInputStates_substruct_BDQ* init_QInputStates_substruct_BDQ();
void free_QInputStates_substruct_BDQ(QInputStates_substruct_BDQ* to, bool free_self);

#endif
