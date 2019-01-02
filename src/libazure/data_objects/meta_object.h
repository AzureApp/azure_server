/**
 ******************************************************************************
 * Azure : Open Source Multi-Target Memory Editor                             *
 * File  : meta_object.h                                                      *
 ******************************************************************************
 * Copyright 2018 Satori. All rights reserved.                                *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#ifndef AZURE_DATA_OBJECT_H
#define AZURE_DATA_OBJECT_H

#include <memory>
#include <msgpack.hpp>

#define AZ_MAGIC 0xABAD1DEA

namespace azure {

enum ObjectType { Meta, Search };
static const char* ObjectTypeNames[] = {"MetaObject", "SearchObject"};

static const char* object_type_to_string(ObjectType type) {
  return ObjectTypeNames[type];
}

/**
 * Base DataObject type used to define the type of the object
 */
struct MetaObject {
  MetaObject() : type(ObjectType::Meta) {}
  MetaObject(ObjectType type) : type(type) {}
  uint32_t magic = AZ_MAGIC;
  ObjectType type;

  MSGPACK_DEFINE(magic, type);
};

using MetaObjectRef = std::unique_ptr<MetaObject>;

}  // namespace azure

MSGPACK_ADD_ENUM(azure::ObjectType);

#include "search_object.h"

#endif  // AZURE_DATA_OBJECT_H
