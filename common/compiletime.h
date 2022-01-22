#pragma once
#include <utility>

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
		size_t Len = 0;
		while (str[Len++]);
		return Len - 1;
	}

	template<class Type>
	constexpr static unsigned Hash(const Type* Data, size_t Size) {
		unsigned Result = unsigned(5381 + TimeSeed);
		for (auto i = 0; i < Size; i++)
			Result = Data[i] + 33 * Result;
		return Result;
	}

	template<class Type>
	constexpr static unsigned StrHash(const Type* Str) {
		return Hash(Str, StrLen(Str) + 1);
	}

	consteval static unsigned Rand(unsigned Count, unsigned Seed) {
		for (unsigned i = 0; i < Count; i++)
			Rand(Seed);
		return Rand(Seed);
	}

	consteval static auto ConstEval(auto Val) { return Val; }

	template <size_t N>
	constexpr static void Repeat(auto f) {
		[f] <auto... Index>(std::index_sequence<Index...>) [[msvc::forceinline]] {
			(f.operator() < Index > (), ...);
		}(std::make_index_sequence<N>());
	}
};

#pragma warning(disable : 4455)
consteval static unsigned operator""h(const char* str, size_t len) { return CompileTime::StrHash(str); }
consteval static unsigned operator""h(const wchar_t* str, size_t len) { return CompileTime::StrHash(str); }
consteval static unsigned operator""h(const char8_t* str, size_t len) { return CompileTime::StrHash(str); }
consteval static unsigned operator""h(const char16_t* str, size_t len) { return CompileTime::StrHash(str); }
consteval static unsigned operator""h(const char32_t* str, size_t len) { return CompileTime::StrHash(str); }