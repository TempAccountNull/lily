#pragma warning(disable : 4455)
#pragma once

#include <string>
#include <atomic>
#include "fixed_string.hpp"
#include "compiletime.hpp"

static_assert(sizeof(char) == 1, "sizeof(char) must be 1");
static_assert(sizeof(wchar_t) == 2, "sizeof(wchar_t) must be 2");

template <class Type>
__declspec(noinline) void DecryptString(Type* Dst, unsigned Seed) noexcept {
	static_assert(sizeof(Type) <= 2, "Type must be character.");

	unsigned i = 0;
	do {
		Dst[i] ^= (Type)CompileTime::Rand(Seed);
	} while (Dst[i++]);
}

template <class Type>
__declspec(noinline) void DecryptStringWithCrtRand(Type* Dst, unsigned Seed) noexcept {
	static_assert(sizeof(Type) <= 2, "Type must be character.");

	unsigned i = 0;
	srand(Seed);
	do {
		Dst[i] ^= (Type)rand();
	} while (Dst[i++]);
}

template <fixstr::basic_fixed_string Src>
char* MovString(char* Dst) noexcept {
	constexpr auto Len = Src.size() + 1;
	static_assert(Len <= 0x200, "String size must be less than 512");
	using type_unsigned = typename std::make_unsigned<char>::type;
	constexpr unsigned Seed = CompileTime::TimeSeed * CompileTime::Hash(Src.data());
	constexpr auto Num = sizeof(uint32_t) / sizeof(char);

	CompileTimeRepeat((Len - 1) / Num, Index, {
		constexpr auto Base = Index * Num;
		*(uint32_t*)(Dst + Base) = CompileTime::ConstEval(
			((type_unsigned)(Src[Base + 0] ^ (char)CompileTime::Rand(Base + 0, Seed)) << (sizeof(char) * 8 * 0)) |
			((type_unsigned)(Src[Base + 1] ^ (char)CompileTime::Rand(Base + 1, Seed)) << (sizeof(char) * 8 * 1)) |
			((type_unsigned)(Src[Base + 2] ^ (char)CompileTime::Rand(Base + 2, Seed)) << (sizeof(char) * 8 * 2)) |
			((type_unsigned)(Src[Base + 3] ^ (char)CompileTime::Rand(Base + 3, Seed)) << (sizeof(char) * 8 * 3))
		);
		//std::atomic_thread_fence(std::memory_order_acq_rel);
		});

	constexpr auto Base = (Len - 1) / Num * Num;
	if constexpr (Base + 0 < Len) Dst[Base + 0] = CompileTime::ConstEval(Src[Base + 0] ^ (char)CompileTime::Rand(Base + 0, Seed));
	if constexpr (Base + 1 < Len) Dst[Base + 1] = CompileTime::ConstEval(Src[Base + 1] ^ (char)CompileTime::Rand(Base + 1, Seed));
	if constexpr (Base + 2 < Len) Dst[Base + 2] = CompileTime::ConstEval(Src[Base + 2] ^ (char)CompileTime::Rand(Base + 2, Seed));
	if constexpr (Base + 3 < Len) Dst[Base + 3] = CompileTime::ConstEval(Src[Base + 3] ^ (char)CompileTime::Rand(Base + 3, Seed));

	DecryptStringWithCrtRand(Dst, Seed);
	//DecryptString(Dst, Seed);
	return Dst;
}

template <fixstr::basic_fixed_string Src>
wchar_t* MovString(wchar_t* Dst) noexcept {
	constexpr auto Len = Src.size() + 1;
	static_assert(Len <= 0x200, "String size must be less than 512");
	using type_unsigned = typename std::make_unsigned<wchar_t>::type;
	constexpr unsigned Seed = CompileTime::TimeSeed * CompileTime::Hash(Src.data());
	constexpr auto Num = sizeof(uint32_t) / sizeof(wchar_t);

	CompileTimeRepeat((Len - 1) / Num, Index, {
		constexpr auto Base = Index * Num;
		*(uint32_t*)(Dst + Base) = CompileTime::ConstEval(
			((type_unsigned)(Src[Base + 0] ^ (wchar_t)CompileTime::Rand(Base + 0, Seed)) << (sizeof(wchar_t) * 8 * 0)) |
			((type_unsigned)(Src[Base + 1] ^ (wchar_t)CompileTime::Rand(Base + 1, Seed)) << (sizeof(wchar_t) * 8 * 1))
		);
		//std::atomic_thread_fence(std::memory_order_acq_rel);
		});

	constexpr auto Base = (Len - 1) / Num * Num;
	if constexpr (Base + 0 < Len) Dst[Base + 0] = CompileTime::ConstEval(Src[Base + 0] ^ (wchar_t)CompileTime::Rand(Base + 0, Seed));
	if constexpr (Base + 1 < Len) Dst[Base + 1] = CompileTime::ConstEval(Src[Base + 1] ^ (wchar_t)CompileTime::Rand(Base + 1, Seed));

	DecryptStringWithCrtRand(Dst, Seed);
	//DecryptString(Dst, Seed);
	return Dst;
}

template <fixstr::basic_fixed_string Src, size_t N, class Type = decltype(Src)::value_type>
Type* MovArray(Type(&Dst)[N]) noexcept {
	static_assert(N >= Src.size() + 1, "Array size is less than string.");
	return MovString<Src>(Dst);
}

template <fixstr::basic_fixed_string Src, size_t N, class Type = decltype(Src)::value_type>
std::array<Type, N>& MovStdArray(std::array<Type, N>& Dst) noexcept {
	static_assert(N >= Src.size() + 1, "Array size is less than string.");
	MovString<Src>(Dst.data());
	return Dst;
}

template <fixstr::basic_fixed_string Src, class Type = decltype(Src)::value_type>
std::basic_string<Type>& MovStdString(std::basic_string<Type>& Dst) noexcept {
	Dst.resize(Src.size());
	MovString<Src>(Dst.data());
	return Dst;
}

template <fixstr::basic_fixed_string Src, class Type = decltype(Src)::value_type>
const std::basic_string<Type> MakeStdString() noexcept {
	std::basic_string<Type> Dst;
	Dst.resize(Src.size());
	MovString<Src>(Dst.data());
	return Dst;
}

#define ElementType(Element) std::remove_const_t<std::remove_reference_t<decltype(*Element.begin())>>

template <fixstr::basic_fixed_string Src>
class EncryptedString {
private:
	mutable ElementType(Src) Temp[Src.size() + 1];
public:
	//This constructor make compiler not initialize temp array to 0
	EncryptedString() noexcept {}
	operator const ElementType(Src)* () const noexcept {
		return MovArray<Src>(Temp);
	}
};

#ifndef __INTELLISENSE__
template <fixstr::basic_fixed_string Src>
ElementType(Src)* operator<<(ElementType(Src)* Dst, const EncryptedString<Src> Str) noexcept {
	return MovString<Src>(Dst);
}

template <fixstr::basic_fixed_string Src, size_t M>
std::array<ElementType(Src), M>& operator<<(std::array<ElementType(Src), M>& Dst, const EncryptedString<Src> Str) noexcept {
	return MovStdArray<Src, M>(Dst);
}

template <fixstr::basic_fixed_string Src>
std::basic_string<ElementType(Src)>& operator<<(std::basic_string<ElementType(Src)>& Dst, const EncryptedString<Src> Str) noexcept {
	return MovStdString<Src>(Dst);
}

template <fixstr::basic_fixed_string Src>
const EncryptedString<Src> operator""e() noexcept {
	return EncryptedString<Src>();
}

template <fixstr::basic_fixed_string Src>
const std::basic_string<ElementType(Src)> operator""es() noexcept {
	return MakeStdString<Src>();
}
#else
//Decrypt string and put into buffer.
char* operator<<(char* Dst, const EncryptedString<"String"> Str);

//Decrypt unicode string and put into buffer.
wchar_t* operator<<(wchar_t* Dst, const EncryptedString<L"String"> Str);

//Decrypt string and put into char std::array.
template <size_t M> std::array<char, M>& operator<<(std::array<char, M>& Dst, const EncryptedString<"String"> Str);

//Decrypt unicode string and put into wchar_t std::array.
template <size_t M> std::array<wchar_t, M>& operator<<(std::array<wchar_t, M>& Dst, const EncryptedString<L"String"> Str);

//Decrypt string and put into std::string.
std::basic_string<char>& operator<<(std::basic_string<char>& Dst, const EncryptedString<"String"> Str);

//Decrypt unicode string and put into std::wstring.
std::basic_string<wchar_t>& operator<<(std::basic_string<wchar_t>& Dst, const EncryptedString<L"String"> Str);

//Decrypt string and get const char* pointer.
const EncryptedString<"String"> operator""e(const char*, size_t);

//Decrypt unicode string and get const wchar_t* pointer.
const EncryptedString<L"String"> operator""e(const wchar_t*, size_t);

//Decrypt string and get std::string.
std::string operator""es(const char*, size_t);

//Decrypt unicode string and get std::wstring.
std::wstring operator""es(const wchar_t*, size_t);
#endif