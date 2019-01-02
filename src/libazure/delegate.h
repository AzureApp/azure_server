/**
 ******************************************************************************
 * Azure : Open Source Multi-Target Memory Editor                             *
 * File  : delegate.h                                                         *
 ******************************************************************************
 * Copyright 2018 Satori. All rights reserved.                                *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#ifndef AZ_DELEGATE_H
#define AZ_DELEGATE_H

#include <functional>
#include <mutex>
#include <vector>

namespace azure {

template <typename... Args>
class Delegate {
 public:
  typedef std::function<void(Args...)> Listener;

  // add new listener function
  void AddListener(const Listener& listener) {
    std::lock_guard<std::mutex> guard(lock_);
    listeners_.push_back(listener);
  }

  // remove all listeners
  void RemoveAllListeners() {
    std::lock_guard<std::mutex> guard(lock_);
    listeners_.clear();
  }

  // call all registered listeners
  void operator()(Args... args) {
    std::lock_guard<std::mutex> guard(lock_);
    for (auto& listener : listeners_) {
      listener(args...);
    }
  }

 private:
  std::mutex lock_;
  std::vector<Listener> listeners_;
};

template <>
class Delegate<void> {
 public:
  typedef std::function<void()> Listener;

  void AddListener(const Listener& listener) {
    std::lock_guard<std::mutex> guard(lock_);
    listeners_.push_back(listener);
  }

  void RemoveAllListeners() {
    std::lock_guard<std::mutex> guard(lock_);
    listeners_.clear();
  }

  void operator()() {
    std::lock_guard<std::mutex> guard(lock_);
    for (auto& listener : listeners_) {
      listener();
    }
  }

 private:
  std::mutex lock_;
  std::vector<Listener> listeners_;
};

}  // namespace azure

#endif