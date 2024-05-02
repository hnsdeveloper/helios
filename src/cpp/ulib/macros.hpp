#ifndef _MACROS_HPP_
#define _MACROS_HPP_

#define SET_USING(t, newname)                               \
using newname = t;                                          \
using newname##_const = const newname;                      \
using newname##_ptr = newname *;                            \
using newname##_const_ptr = const newname *;                \
using newname##_reference = newname &;                      \
using newname##_const_reference = const newname &;          \
using newname##_rvalue_reference = newname &&;

#define SET_USING_CLASS(t, newname)                         \
public:                                                     \
SET_USING(t, newname)                                       \
private:

#define EXTRACT_SUB_USING_T(t, u, v)                    \
using v = t::u;                                         \
using v##_const = t:: u##_const;                        \
using v##_ptr = t:: u##_ptr;                            \
using v##_const_ptr = t:: u##_const_ptr;                \
using v##_reference = t:: u##_reference;                \
using v##_const_reference = t:: u##_const_reference;    \
using v##_rvalue_reference = t:: u##_rvalue_reference;

#define EXTRACT_SUB_USING_T_CLASS(t, u, v)         \
public:                                            \
EXTRACT_SUB_USING_T(t, u, v)                       \
private:

#define PANIC(msg)                                  



#endif