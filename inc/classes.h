/*
 * classes.h
 * 
 * Lightweight C extension for basic object-oriented functionality.
 * Features:
 * - Class declaration and definition
 * - Object instantiation and deletion with functionality akin to C++ new/delete
 * - Multi-level inheritance
 * - Method calls and method overloading
 * - Simple usage, no additional dependencies
 * 
 * Copyright (C) 2025 Benjamin Sellak
 * Released under MIT License 
 */

#ifndef CLASSES_H_
#define CLASSES_H_

#include <stddef.h>
#include <stdlib.h>

/// \brief method call on object of type *pObj
#define VCALL(pObj, func, ...) ((pObj)->pVtable->func((pObj) , ## __VA_ARGS__))

/// \brief instantiates a new object of type 'type'
#define NEW(type, pCfg) type ## _NewOuter (pCfg)

/// \brief deletes an object that was instantiated with new
#define DELETE(pInst) (((__GenericClass__*)pInst)->__DeleteClass__(pInst))

/// \brief casts an object of parent type to child type
#define GET_CHILDCLASS(ptr, type, member) ({                                   \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);                   \
        (type *)( (char *)__mptr - offsetof(type,member) );})

/// \brief declare a class of type 'type'
#define CLASSDECL(type, ...)                                                   \
typedef struct _CLASS_TYPENAME(type) _CLASS_TYPENAME(type);                    \
typedef struct _CFG_TYPENAME(type) _CFG_TYPENAME(type);                        \
typedef struct _VTABLE_TYPENAME(type) _VTABLE_TYPENAME(type);                  \
typedef struct _MEMBERS_TYPENAME(type) _MEMBERS_TYPENAME(type);                \
_DEFINE_CLASS_STRUCT(type, ## __VA_ARGS__);                                    \
type * type ## _NewOuter (const _CFG_TYPENAME(type)* pCfg );                   \
void type ## _NewInner (type * pInst, const _CFG_TYPENAME(type)* pCfg );       \
void type ## _DeleteOuter ( type * pInst);                                     \
void type ## _DeleteInner ( type * pInst);

/// \brief define a class of type 'type'
#define CLASSDEF(type, ...)                                                    \
_VTABLE_TYPENAME(type) _VTABLE_HANDLE(type);                                   \
static int type ## _IsVtableInit = 0;                                          \
_DECL_EXTERN_FUNS(0, ## __VA_ARGS__);                                          \
static void type ## _Delete(void* pInst);                                      \
static void type ## _Init(type * pInst, const _CFG_TYPENAME(type)* pCfg );     \
void type ## _Ctor( type * pInst,                                              \
                    _VTABLE_TYPENAME(type)* pVtable,                           \
const _CFG_TYPENAME(type)* pCfg )                                              \
{                                                                              \
    _CALL_BASE_CTOR(((void*) pInst),                                           \
            ((void*) pVtable),                                                 \
            (pCfg),                                                            \
            ## __VA_ARGS__);                                                   \
    pInst->pVtable = pVtable;                                                  \
    pInst->pMembers = calloc(sizeof ( _MEMBERS_TYPENAME(type)), 1);            \
    type ## _Init(pInst, pCfg);                                                \
}                                                                              \
static void type ## _VtableInit(_VTABLE_TYPENAME(type)* pVtable);              \
void type ## _VtableInitTree(_VTABLE_TYPENAME(type)* pVtable)                  \
{                                                                              \
        _CALL_BASE_FUN_SINGLE_ARG(_VtableInitTree,                             \
                (void*) pVtable,                                               \
                ## __VA_ARGS__ );                                              \
        type ## _VtableInit(pVtable);                                          \
}                                                                              \
void type ## _NewInner (type * pInst, const _CFG_TYPENAME(type)* pCfg )        \
{                                                                              \
    if (!(type ## _IsVtableInit) )                                             \
    {                                                                          \
        type ## _VtableInitTree(& _VTABLE_HANDLE(type));                       \
        (type ## _IsVtableInit) = 1;                                           \
    }                                                                          \
    type ## _Ctor( pInst, & _VTABLE_HANDLE(type), pCfg );                      \
}                                                                              \
type * type ## _NewOuter(const _CFG_TYPENAME(type)* pCfg)                      \
{                                                                              \
    type * pInst = calloc(sizeof(type), 1);                                    \
    type ## _NewInner(pInst, pCfg);                                            \
    ((__GenericClass__*)pInst)->__DeleteClass__ = type ## _Delete;             \
    return pInst;                                                              \
}                                                                              \
static void type ## _Deinit(type * pInst);                                     \
void type ## _Dtor(type * pInst)                                               \
{                                                                              \
    _CALL_BASE_FUN_SINGLE_ARG(_Dtor, (void*) pInst, ## __VA_ARGS__ );          \
    type ## _Deinit(pInst);                                                    \
    free(pInst->pMembers);                                                     \
    pInst->pMembers = 0;                                                       \
}                                                                              \
void type ## _DeleteInner(type * pInst)                                        \
{                                                                              \
    type ## _Dtor(pInst);                                                      \
}                                                                              \
void type ## _DeleteOuter(type * pInst)                                        \
{                                                                              \
    type ## _DeleteInner(pInst);                                               \
    free(pInst);                                                               \
    pInst = 0;                                                                 \
}                                                                              \
void type ## _Delete(void* pInst)                                              \
{                                                                              \
    type ## _DeleteOuter((type *) pInst);                                      \
}

#define _CLASS_TYPENAME(type) type
#define _CFG_TYPENAME(type) type ## _Cfg
#define _VTABLE_TYPENAME(type) type ## _Vtable
#define _MEMBERS_TYPENAME(type) type ## _Members

/// \brief identifier of the static class vtable variable
#define _VTABLE_HANDLE(type) type ## _VtableDefault

/// \brief defines the class struct of a derived class
#define _DEFINE_CLASS_STRUCT_DERIVED(type, base)                               \
struct _CLASS_TYPENAME(type)                                                   \
{                                                                              \
    _CLASS_TYPENAME(base) Base;                                                \
    const _VTABLE_TYPENAME(type)* pVtable;                                     \
    _MEMBERS_TYPENAME(type)* pMembers;                                         \
}

/// \brief defines the class struct of a root class
#define _DEFINE_CLASS_STRUCT_ROOT(type)                                        \
struct _CLASS_TYPENAME(type)                                                   \
{                                                                              \
    const void(*_Delete)(void* pInst);                                         \
    const _VTABLE_TYPENAME(type)* pVtable;                                     \
    _MEMBERS_TYPENAME(type)* pMembers;                                         \
}

/// \brief dispatches Macro call in dependence of vararg count
#define _GET_DEFINE_CLASS_STRUCT_MACRO(_1, _2, NAME,...) NAME

/// \brief defines the class struct
#define _DEFINE_CLASS_STRUCT(...) _GET_DEFINE_CLASS_STRUCT_MACRO(__VA_ARGS__,  \
        _DEFINE_CLASS_STRUCT_DERIVED, _DEFINE_CLASS_STRUCT_ROOT)(__VA_ARGS__)

/// \brief does nothing
#define _DO_NOTHING(...) /* do nothing */

/// \brief calls the base class function
#define _CALL_BASE_FUN(fun_suffix, arg, base) base ## fun_suffix ( arg );
/// \brief dispatches Macro call in dependence of vararg count
#define _GET_CALL_BASE_FUN_MACRO(_1, _2, _3, NAME, ...) NAME
/// \brief calls a base class function if base class is available
#define _CALL_BASE_FUN_SINGLE_ARG(...) _GET_CALL_BASE_FUN_MACRO(__VA_ARGS__,   \
        _CALL_BASE_FUN, _DO_NOTHING)(__VA_ARGS__)


/// \brief calls the constructor of the base class
#define _DO_CALL_BASE_CTOR(pInst, pVtable, pCfg, base)                         \
    base ## _Ctor(pInst, pVtable, &((pCfg)->Base))
/// \brief dispatches Macro call in dependence of vararg count
#define _GET_CALL_BASE_CTOR_MACRO(_1, _2, _3, _4, NAME, ...) NAME
/// \brief calls the constructor of the base class if available
#define _CALL_BASE_CTOR(...) _GET_CALL_BASE_CTOR_MACRO(__VA_ARGS__,            \
        _DO_CALL_BASE_CTOR, _DO_NOTHING)(__VA_ARGS__)


/// \brief declares base class functions as external
#define _DO_DECL_EXTERN_FUNS(dummy, type)                                      \
extern void type ## _Ctor(type * pInst,                                        \
                          _VTABLE_TYPENAME(type)* pVtable,                     \
                          const _CFG_TYPENAME(type)* pCfg);                    \
extern void type ## _VtableInitTree(_VTABLE_TYPENAME(type)* pVtable);          \
extern void type ## _Dtor(type * pInst);

/// \brief dispatches Macro call in dependence of vararg count
#define _GET_DECL_EXTERN_FUNCS_MACRO(_1, _2, NAME, ...) NAME
/// \brief declares base class functions as external if base class is existent
#define _DECL_EXTERN_FUNS(...) _GET_DECL_EXTERN_FUNCS_MACRO(__VA_ARGS__,       \
        _DO_DECL_EXTERN_FUNS, _DO_NOTHING)(__VA_ARGS__)

/// \brief helper struct for safe typecast on delete calls
typedef struct
{
    const void(* __DeleteClass__)(void* pInst);
} __GenericClass__;

#endif /* Classes.h */
