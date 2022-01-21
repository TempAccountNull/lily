#pragma once
#include <string>
#include "fixed_string.hpp"
#include "compiletime.h"

template<std::array Data, unsigned Seed>
struct EncryptedData {
public:
	using TStdArray = std::remove_cvref_t<decltype(Data)>;
	using TElem = std::remove_cvref_t<decltype(Data[0])>;
	constexpr static size_t Size = Data.size();
	constexpr static TStdArray _Data = [] {
		TStdArray temp{};
		unsigned _Seed = Seed;
		for (size_t i = 0; i < Size; i++)
			temp[i] = Data[i] ^ (TElem)CompileTime::Rand(_Seed);
		return temp;
	}();

	consteval EncryptedData() {}

	__forceinline void Decrypt(TElem* Dst) const {
		constexpr TStdArray HardCodedData = _Data;
		*(TStdArray*)Dst = HardCodedData;

		//+[] makes lambda not inlined.
		auto _Decrypt = +[](TElem* Dst, size_t Size, unsigned _Seed) {
			for (size_t i = 0; i < Size; i++)
				Dst[i] ^= (TElem)CompileTime::Rand(_Seed);
		};

		_Decrypt(Dst, Size, Seed);
	}
};

template <fixstr::basic_fixed_string Src>
class EncryptedString {
private:
	using TElem = std::remove_cvref_t<decltype(Src[0])>;
	constexpr static auto Size = Src.size() + 1;
	constexpr static auto Data = EncryptedData<Src._data, CompileTime::Hash(Src.data())>();

	template <size_t N>
	using TArray = TElem(&)[N];
	template <size_t N>
	using TStdArray = std::array<TElem, N>;
	template <size_t N>
	using TFixedString = fixstr::basic_fixed_string<TElem, N>;
	using TStdString = std::basic_string<TElem>;

public:
	EncryptedString() noexcept {}
	~EncryptedString() noexcept {}

	void MoveString(TElem* Dst) const noexcept {
		Data.Decrypt(Dst);
	}

	template <size_t N>
	void MoveArray(TArray<N> Dst) const noexcept {
		static_assert(N >= Size, "Too less array size");
		MoveString(Dst);
	}

	template <size_t N>
	void MoveStdArray(TStdArray<N>& Dst) const noexcept {
		MoveArray<N>(Dst._Elems);
	}

	template <size_t N>
	void MoveFixedString(TFixedString<N>& Dst) const noexcept {
		MoveArray<N + 1>(Dst._data._Elems);
	}

	void MoveStdString(TStdString& Dst) const noexcept {
		Dst.resize(Size - 1);
		MoveString(Dst.data());
	}

	template <size_t N>
	TStdArray<N> GetStdArray() const noexcept {
		TStdArray<N> Dst;
		MoveStdArray<N>(Dst);
		return Dst;
	}

	template <size_t N>
	TFixedString<N> GetFixedString() const noexcept {
		TFixedString<N> Dst;
		MoveFixedString<N>(Dst);
		return Dst;
	}

	TStdString GetStdString() const noexcept {
		TStdString Dst;
		MoveStdString(Dst);
		return Dst;
	}

	template <size_t N>
	operator const TStdArray<N>() const noexcept {
		return GetStdArray<N>();
	}

	template <size_t N>
	operator const TFixedString<N>() const noexcept {
		return GetFixedString<N>();
	}

	operator const TStdString() const noexcept {
		return GetStdString();
	}

	friend TElem* operator<<(TElem* Dst, const EncryptedString<Src> Str) noexcept {
		Str.MoveString(Dst);
		return Dst;
	}

private:
	mutable TElem _Buf[Size];

public:
	operator const TElem* () const noexcept {
		MoveArray(_Buf);
		return _Buf;
	}
};

#pragma warning(disable : 4455)
#ifndef __INTELLISENSE__
template <fixstr::basic_fixed_string Src>
auto operator""e() noexcept {
	return EncryptedString<Src>();
}
#else
EncryptedString<"str"> operator""e(const char*, size_t) {}
EncryptedString<L"str"> operator""e(const wchar_t*, size_t) {}
EncryptedString<u8"str"> operator""e(const char8_t*, size_t) {}
EncryptedString<u"str"> operator""e(const char16_t*, size_t) {}
EncryptedString<U"str"> operator""e(const char32_t*, size_t) {}
#endif