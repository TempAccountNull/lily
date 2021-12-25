#pragma once
#define CompileTimeRepeatMaxCount 0x100

#define CheckAndRepeatEach0(N, IndexName, ...)\
	if constexpr (N < LoopCount) { constexpr auto IndexName = N; __VA_ARGS__ } else break

#define CheckAndRepeatEach1(N, IndexName, ...)\
	CheckAndRepeatEach0(N + 0x00, IndexName, __VA_ARGS__); CheckAndRepeatEach0(N + 0x01, IndexName, __VA_ARGS__); CheckAndRepeatEach0(N + 0x02, IndexName, __VA_ARGS__); CheckAndRepeatEach0(N + 0x03, IndexName, __VA_ARGS__);\
	CheckAndRepeatEach0(N + 0x04, IndexName, __VA_ARGS__); CheckAndRepeatEach0(N + 0x05, IndexName, __VA_ARGS__); CheckAndRepeatEach0(N + 0x06, IndexName, __VA_ARGS__); CheckAndRepeatEach0(N + 0x07, IndexName, __VA_ARGS__)

#define CheckAndRepeatEach2(N, IndexName, ...)\
	CheckAndRepeatEach1(N + 0x00, IndexName, __VA_ARGS__); CheckAndRepeatEach1(N + 0x08, IndexName, __VA_ARGS__); CheckAndRepeatEach1(N + 0x10, IndexName, __VA_ARGS__); CheckAndRepeatEach1(N + 0x18, IndexName, __VA_ARGS__);\
	CheckAndRepeatEach1(N + 0x20, IndexName, __VA_ARGS__); CheckAndRepeatEach1(N + 0x28, IndexName, __VA_ARGS__); CheckAndRepeatEach1(N + 0x30, IndexName, __VA_ARGS__); CheckAndRepeatEach1(N + 0x38, IndexName, __VA_ARGS__)

#define CheckAndRepeatEach3(N, IndexName, ...)\
	CheckAndRepeatEach2(N + 0x00, IndexName, __VA_ARGS__); CheckAndRepeatEach2(N + 0x40, IndexName, __VA_ARGS__); CheckAndRepeatEach2(N + 0x80, IndexName, __VA_ARGS__); CheckAndRepeatEach2(N + 0xC0, IndexName, __VA_ARGS__)

#define CompileTimeRepeat(Count, IndexName, ...)\
do {\
	constexpr auto LoopCount = Count;\
	static_assert(LoopCount <= CompileTimeRepeatMaxCount, "Too big loopcount");\
	CheckAndRepeatEach3(0, IndexName, __VA_ARGS__);\
}while(0)


class CompileTime {
private:
	//These are constants which crt.rand use
	constexpr static unsigned Rand_Multiplier = 0x343FD;
	constexpr static unsigned Rand_Increment = 0x269EC3;
	constexpr static unsigned Rand_Modulus = 0x7FFF;
public:
	constexpr static unsigned TimeSeed =
		(__TIME__[7] - '0') * 1 + (__TIME__[6] - '0') * 10 +
		(__TIME__[4] - '0') * 60 + (__TIME__[3] - '0') * 600 +
		(__TIME__[1] - '0') * 3600 + (__TIME__[0] - '0') * 36000;

	constexpr static unsigned Rand(unsigned& Seed) {
		Seed = Seed * Rand_Multiplier + Rand_Increment;
		return (Seed >> 0x10) & Rand_Modulus;
	}

	template<class Type>
	constexpr static size_t StrLen(const Type* str) {
		static_assert(sizeof(Type) <= 2, "Type must be character.");

		size_t Len = 0;
		while (str[Len++]);
		return Len - 1;
	}

	template<class Type>
	constexpr static unsigned Hash(const Type* str) {
		static_assert(sizeof(Type) <= 2, "Type must be character.");

		unsigned Result = unsigned(5381 * TimeSeed);
		//unsigned Result = 5381;
		int i = (int)StrLen(str) - 1;
		while (i >= 0) Result = (unsigned)str[i--] + 33 * Result;
		return Result;
	}

	consteval static unsigned Rand(unsigned Count, unsigned Seed) {
		for (unsigned i = 0; i < Count; i++)
			Rand(Seed);
		return Rand(Seed);
	}

	consteval static auto ConstEval(auto Val) { return Val; }
};

consteval static unsigned operator""h(const char* str, size_t len) { return CompileTime::Hash(str); }
consteval static unsigned operator""h(const wchar_t* str, size_t len) { return CompileTime::Hash(str); }