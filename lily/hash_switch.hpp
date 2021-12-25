#pragma once
#include <tuple>

#define GenerateHashSwitch(FuncName, Params, ...)\
static auto FuncName(auto Hash, Params) {switch(Hash) {__VA_ARGS__}}

#define HashCase(ElemHash, Execute, ...)\
case ElemHash : {{Execute;} return std::tuple(__VA_ARGS__);}

#define HashDefault(Execute, ...)\
default : {{Execute;} return std::tuple(__VA_ARGS__);}