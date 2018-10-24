/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: game.proto */

#ifndef PROTOBUF_C_game_2eproto__INCLUDED
#define PROTOBUF_C_game_2eproto__INCLUDED

#include "protobuf-c/protobuf-c.h"

PROTOBUF_C__BEGIN_DECLS

#if PROTOBUF_C_VERSION_NUMBER < 1000000
# error This file was generated by a newer version of protoc-c which is incompatible with your libprotobuf-c headers. Please update your headers.
#elif 1001001 < PROTOBUF_C_MIN_COMPILER_VERSION
# error This file was generated by an older version of protoc-c which is incompatible with your libprotobuf-c headers. Please regenerate this file with a newer version of protoc-c.
#endif


typedef struct _PersonalInfo PersonalInfo;
typedef struct _TeamInfo TeamInfo;


/* --- enums --- */


/* --- messages --- */

/*
 **
 * 角色的个人信息
 */
struct  _PersonalInfo
{
  ProtobufCMessage base;
  /*
   * 角色名字 
   */
  char *name;
  /*
   * 角色年龄 
   */
  int32_t year;
  /*
   * 武器 
   */
  char *weapon;
  /*
   * 防具 
   */
  size_t n_armor;
  char **armor;
  /*
   * 携带的钱 
   */
  protobuf_c_boolean has_money;
  int32_t money;
};
#define PERSONAL_INFO__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&personal_info__descriptor) \
    , NULL, 0, NULL, 0,NULL, 0,0 }


/*
 **
 * 团队信息
 */
struct  _TeamInfo
{
  ProtobufCMessage base;
  /*
   * leader 
   */
  PersonalInfo *leader;
  /*
   * 队员 
   */
  PersonalInfo *aaron;
};
#define TEAM_INFO__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&team_info__descriptor) \
    , NULL, NULL }


/* PersonalInfo methods */
void   personal_info__init
                     (PersonalInfo         *message);
size_t personal_info__get_packed_size
                     (const PersonalInfo   *message);
size_t personal_info__pack
                     (const PersonalInfo   *message,
                      uint8_t             *out);
size_t personal_info__pack_to_buffer
                     (const PersonalInfo   *message,
                      ProtobufCBuffer     *buffer);
PersonalInfo *
       personal_info__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   personal_info__free_unpacked
                     (PersonalInfo *message,
                      ProtobufCAllocator *allocator);
/* TeamInfo methods */
void   team_info__init
                     (TeamInfo         *message);
size_t team_info__get_packed_size
                     (const TeamInfo   *message);
size_t team_info__pack
                     (const TeamInfo   *message,
                      uint8_t             *out);
size_t team_info__pack_to_buffer
                     (const TeamInfo   *message,
                      ProtobufCBuffer     *buffer);
TeamInfo *
       team_info__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   team_info__free_unpacked
                     (TeamInfo *message,
                      ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*PersonalInfo_Closure)
                 (const PersonalInfo *message,
                  void *closure_data);
typedef void (*TeamInfo_Closure)
                 (const TeamInfo *message,
                  void *closure_data);

/* --- services --- */


/* --- descriptors --- */

extern const ProtobufCMessageDescriptor personal_info__descriptor;
extern const ProtobufCMessageDescriptor team_info__descriptor;

PROTOBUF_C__END_DECLS


#endif  /* PROTOBUF_C_game_2eproto__INCLUDED */
