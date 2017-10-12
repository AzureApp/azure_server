/**
 ******************************************************************************
 * Azure : Open Source iOS Memory Editor                                      *
 * File : search_object.h                                                     *
 ******************************************************************************
 * Copyright 2017 Satori. All rights reserved.                                *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#ifndef AZURE_SEARCHOBJECT_H
#define AZURE_SEARCHOBJECT_H

#include "data_object.h"

namespace azure {

struct SearchObject : DataObject {
    SearchObject() {}
    SearchObject(int address) :
            DataObject(DataObject::Type::Search),
            addr(address),
            data({0x70, 0x47}) {}

    int addr;
    std::vector<uint8_t> data;

    MSGPACK_DEFINE(type, addr, data);
};

}

#endif //AZURE_SEARCHOBJECT_H
