#pragma once
#include <type_traits>
#include <vector>

template<typename>
class Function;

template<typename ReturnType, typename ...Args>
class Function<ReturnType(Args...)>
{
private:
    using Dispatcher = ReturnType(* const)(void*, Args...);
    Dispatcher CallBack;
    std::vector<uint8_t> Object;

    //Dispatch() is instantiated by the Function constructor, which will store a pointer to the function in pFunc.
    template<typename T>
    static ReturnType Dispatch(T&& target, Args... args) {
        return target(args...);
    }
public:
    Function() = delete;
    Function(Function& rhs) noexcept : CallBack(rhs.CallBack), Object(rhs.Object) {}
    Function& operator=(Function& rhs) noexcept {
        CallBack = rhs.CallBack;
        Object = rhs.Object;
    }

    template <typename T, std::enable_if_t<std::is_invocable_r<ReturnType, T&&, Args...>::value>* = nullptr>
    Function(T&& target) noexcept : CallBack((Dispatcher)Dispatch<T>) {
        Object.resize(sizeof(target));
        std::copy_n((uint8_t*)&target, sizeof(target), Object.begin());
    }

    //Specialize for reference-to-function, to ensure that a valid pointer is stored.
    using TargetFunctionRef = ReturnType(Args...);
    Function(TargetFunctionRef target) : CallBack((Dispatcher)Dispatch<TargetFunctionRef>) {
        static_assert(sizeof(void*) == sizeof target,
            "It will not be possible to pass functions by reference on this platform. "
            "Please use explicit function pointers i.e. foo(target) -> foo(&target)");
        Object.resize(sizeof(target));
        std::copy_n((uint8_t*)&target, sizeof(target), Object.begin());
    }

    ReturnType operator()(Args... args) const {
        return CallBack((void*)Object.data(), args...);
    }
    //pointer to the static function that will call the wrapped invokable object
    Dispatcher pFunc() const {
        return CallBack;
    }
    //pointer to the invokable object
    void* pThis() const {
        return (void*)Object.data();
    }
};