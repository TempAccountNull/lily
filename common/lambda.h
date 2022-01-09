#pragma once
#include <type_traits>

class Lambda {
public:
    template<class LambdaType, class... Types>
    using ReturnType = std::invoke_result_t<LambdaType, Types...>;

    template<class LambdaType, class... Types>
    using LambdaFunctionPtr = ReturnType<LambdaType, Types...>(*)(void*, Types...);

    template<class LambdaType, class... Types>
    static LambdaFunctionPtr<LambdaType, Types...> pFunc(LambdaType lambda, Types... Args) {
        return
            (LambdaFunctionPtr<LambdaType, Types...>)
            (ReturnType<LambdaType, Types...>(*)(LambdaType, Types...))
            [](auto lambda, auto... Args) { return lambda(Args...); };
    }

    static void* pThis(auto lambda) { return &lambda; }
};