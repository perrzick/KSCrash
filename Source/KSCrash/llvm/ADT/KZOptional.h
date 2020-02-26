//===-- KZOptional.h - Simple variant for passing optional values ---*- C++ -*-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file provides KZOptional, a template class modeled in the spirit of
//  OCaml's 'opt' variant.  The idea is to strongly type whether or not
//  a value can be optional.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_ADT_KZOPTIONAL_H
#define LLVM_ADT_KZOPTIONAL_H

#include "None.h"
#include "AlignOf.h"
#include "Compiler.h"
#include <cassert>
#include <new>
#include <utility>

namespace llvm {

template<typename T>
class KZOptional {
  AlignedCharArrayUnion<T> storage;
  bool hasVal;
public:
  typedef T value_type;

  KZOptional(NoneType) : hasVal(false) {}
  explicit KZOptional() : hasVal(false) {}
  KZOptional(const T &y) : hasVal(true) {
    new (storage.buffer) T(y);
  }
  KZOptional(const KZOptional &O) : hasVal(O.hasVal) {
    if (hasVal)
      new (storage.buffer) T(*O);
  }

  KZOptional(T &&y) : hasVal(true) {
    new (storage.buffer) T(std::forward<T>(y));
  }
  KZOptional(KZOptional<T> &&O) : hasVal(O) {
    if (O) {
      new (storage.buffer) T(std::move(*O));
      O.reset();
    }
  }
  KZOptional &operator=(T &&y) {
    if (hasVal)
      **this = std::move(y);
    else {
      new (storage.buffer) T(std::move(y));
      hasVal = true;
    }
    return *this;
  }
  KZOptional &operator=(KZOptional &&O) {
    if (!O)
      reset();
    else {
      *this = std::move(*O);
      O.reset();
    }
    return *this;
  }

  /// Create a new object by constructing it in place with the given arguments.
  template<typename ...ArgTypes>
  void emplace(ArgTypes &&...Args) {
    reset();
    hasVal = true;
    new (storage.buffer) T(std::forward<ArgTypes>(Args)...);
  }

  static inline KZOptional create(const T* y) {
    return y ? KZOptional(*y) : KZOptional();
  }

  // FIXME: these assignments (& the equivalent const T&/const KZOptional& ctors)
  // could be made more efficient by passing by value, possibly unifying them
  // with the rvalue versions above - but this could place a different set of
  // requirements (notably: the existence of a default ctor) when implemented
  // in that way. Careful SFINAE to avoid such pitfalls would be required.
  KZOptional &operator=(const T &y) {
    if (hasVal)
      **this = y;
    else {
      new (storage.buffer) T(y);
      hasVal = true;
    }
    return *this;
  }

  KZOptional &operator=(const KZOptional &O) {
    if (!O)
      reset();
    else
      *this = *O;
    return *this;
  }

  void reset() {
    if (hasVal) {
      (**this).~T();
      hasVal = false;
    }
  }

  ~KZOptional() {
    reset();
  }

  const T* getPointer() const { assert(hasVal); return reinterpret_cast<const T*>(storage.buffer); }
  T* getPointer() { assert(hasVal); return reinterpret_cast<T*>(storage.buffer); }
  const T& getValue() const LLVM_LVALUE_FUNCTION { assert(hasVal); return *getPointer(); }
  T& getValue() LLVM_LVALUE_FUNCTION { assert(hasVal); return *getPointer(); }

  explicit operator bool() const { return hasVal; }
  bool hasValue() const { return hasVal; }
  const T* operator->() const { return getPointer(); }
  T* operator->() { return getPointer(); }
  const T& operator*() const LLVM_LVALUE_FUNCTION { assert(hasVal); return *getPointer(); }
  T& operator*() LLVM_LVALUE_FUNCTION { assert(hasVal); return *getPointer(); }

  template <typename U>
  LLVM_CONSTEXPR T getValueOr(U &&value) const LLVM_LVALUE_FUNCTION {
    return hasValue() ? getValue() : std::forward<U>(value);
  }

#if LLVM_HAS_RVALUE_REFERENCE_THIS
  T&& getValue() && { assert(hasVal); return std::move(*getPointer()); }
  T&& operator*() && { assert(hasVal); return std::move(*getPointer()); }

  template <typename U>
  T getValueOr(U &&value) && {
    return hasValue() ? std::move(getValue()) : std::forward<U>(value);
  }
#endif
};

template <typename T> struct isPodLike;
template <typename T> struct isPodLike<KZOptional<T> > {
  // An KZOptional<T> is pod-like if T is.
  static const bool value = isPodLike<T>::value;
};

/// \brief Poison comparison between two \c KZOptional objects. Clients needs to
/// explicitly compare the underlying values and account for empty \c KZOptional
/// objects.
///
/// This routine will never be defined. It returns \c void to help diagnose
/// errors at compile time.
template<typename T, typename U>
void operator==(const KZOptional<T> &X, const KZOptional<U> &Y);

template<typename T>
bool operator==(const KZOptional<T> &X, NoneType) {
  return !X.hasValue();
}

template<typename T>
bool operator==(NoneType, const KZOptional<T> &X) {
  return X == None;
}

template<typename T>
bool operator!=(const KZOptional<T> &X, NoneType) {
  return !(X == None);
}

template<typename T>
bool operator!=(NoneType, const KZOptional<T> &X) {
  return X != None;
}
/// \brief Poison comparison between two \c KZOptional objects. Clients needs to
/// explicitly compare the underlying values and account for empty \c KZOptional
/// objects.
///
/// This routine will never be defined. It returns \c void to help diagnose
/// errors at compile time.
template<typename T, typename U>
void operator!=(const KZOptional<T> &X, const KZOptional<U> &Y);

/// \brief Poison comparison between two \c KZOptional objects. Clients needs to
/// explicitly compare the underlying values and account for empty \c KZOptional
/// objects.
///
/// This routine will never be defined. It returns \c void to help diagnose
/// errors at compile time.
template<typename T, typename U>
void operator<(const KZOptional<T> &X, const KZOptional<U> &Y);

/// \brief Poison comparison between two \c KZOptional objects. Clients needs to
/// explicitly compare the underlying values and account for empty \c KZOptional
/// objects.
///
/// This routine will never be defined. It returns \c void to help diagnose
/// errors at compile time.
template<typename T, typename U>
void operator<=(const KZOptional<T> &X, const KZOptional<U> &Y);

/// \brief Poison comparison between two \c KZOptional objects. Clients needs to
/// explicitly compare the underlying values and account for empty \c KZOptional
/// objects.
///
/// This routine will never be defined. It returns \c void to help diagnose
/// errors at compile time.
template<typename T, typename U>
void operator>=(const KZOptional<T> &X, const KZOptional<U> &Y);

/// \brief Poison comparison between two \c KZOptional objects. Clients needs to
/// explicitly compare the underlying values and account for empty \c KZOptional
/// objects.
///
/// This routine will never be defined. It returns \c void to help diagnose
/// errors at compile time.
template<typename T, typename U>
void operator>(const KZOptional<T> &X, const KZOptional<U> &Y);

} // end llvm namespace

#endif
