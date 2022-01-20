#pragma once
#include <string>
#include "fixed_string.hpp"
#include "compiletime.h"

template <class Type>
__declspec(noinline) void _Decrypt(Type* Dst, unsigned Seed, size_t Len) noexcept {
	for (size_t i = 0; i < Len; i++)
		Dst[i] ^= (Type)CompileTime::Rand(Seed);
};

template<class Type, size_t Len, unsigned Seed>
struct EncryptedData {
	using StorageType = std::array<Type, Len>;
	StorageType _data;

	constexpr EncryptedData(const StorageType& Src) noexcept {
		unsigned _Seed = Seed;
		for (size_t i = 0; i < Len; i++)
			_data[i] = Src[i] ^ (Type)CompileTime::Rand(_Seed);
	}

	void Decrypt() noexcept {
		_Decrypt(_data.data(), Seed, Len);
	};
};

template <fixstr::basic_fixed_string Src>
class EncryptedString {
private:
	using Type = std::remove_cvref_t<decltype(Src[0])>;
	constexpr static auto Len = Src.size() + 1;
	constexpr static unsigned Seed = CompileTime::Hash(Src.data());
	using EncryptedType = EncryptedData<Type, Len, Seed>;

	void MoveString(Type* Dst) const noexcept {
		constexpr EncryptedType Data = Src._data;
		EncryptedType& DstData = *(EncryptedType*)Dst;
		DstData = Data;
		DstData.Decrypt();
	}

	template <size_t N> requires (N >= Len)
	void MoveArray(Type(&Dst)[N]) const noexcept {
		MoveString(Dst);
	}

	template <size_t N> requires (N >= Len)
	void MoveStdArray(std::array<Type, N>& Dst) const noexcept {
		MoveString(Dst.data());
	}

	void MoveStdString(std::basic_string<Type>& Dst) const noexcept {
		Dst.resize(Src.size());
		MoveString(Dst.data());
	}

public:
	std::basic_string<Type> GetStdString() const noexcept {
		std::basic_string<Type> Dst;
		MoveStdString(Dst);
		return Dst;
	}

	template <size_t N> requires (N >= Len)
	std::array<Type, N> GetStdArray() const noexcept {
		std::array<Type, N> Result;
		MoveStdArray<N>(Result);
		return Result;
	}

	template <size_t N> requires (N >= Len)
	operator const std::array<Type, N>() const noexcept {
		return GetStdArray<N>();
	}

	operator const std::basic_string<Type>() const noexcept {
		return GetStdString();
	}

private:
	mutable Type _Buf[Src.size() + 1];

public:
	EncryptedString() noexcept {}

	operator const Type* () const noexcept {
		MoveArray(_Buf);
		return _Buf;
	}

	//Decrypt string and put into buffer.
	friend Type* operator<<(Type* Dst, const EncryptedString<Src> Str) noexcept {
		Str.MoveString(Dst);
		return Dst;
	}

	//Decrypt string and put into std::array.
	template <size_t N> requires (N >= Len)
	friend std::array<Type, N>& operator<<(std::array<Type, N>& Dst, const EncryptedString<Src> Str) noexcept {
		Str.MoveStdArray(Dst);
		return Dst;
	}

	//Decrypt string and put into std::basic_string.
	friend std::basic_string<Type>& operator<<(std::basic_string<Type>& Dst, const EncryptedString<Src> Str) noexcept {
		Str.MoveStdString(Dst);
		return Dst;
	}
};

#pragma warning(disable : 4455)
#ifndef __INTELLISENSE__
template <fixstr::basic_fixed_string Src>
const EncryptedString<Src> operator""e() noexcept { return EncryptedString<Src>(); }
#else
const EncryptedString<fixstr::fixed_string<0>()> operator""e(const char*, size_t);
const EncryptedString<fixstr::fixed_wstring<0>()> operator""e(const wchar_t*, size_t);
const EncryptedString<fixstr::fixed_u8string<0>()> operator""e(const char8_t*, size_t);
const EncryptedString<fixstr::fixed_u16string<0>()> operator""e(const char16_t*, size_t);
const EncryptedString<fixstr::fixed_u32string<0>()> operator""e(const char32_t*, size_t);
#endif