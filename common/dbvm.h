#pragma once
#include <Windows.h>
#include <intrin.h>
#include "physicalmemory.h"
#include "ida_defs.h"
#include "exception.h"

extern "C" uintptr_t vmcall_intel(uintptr_t password3, uintptr_t passwordr1, void* vmcallinfo);
extern "C" uintptr_t vmcall_amd(uintptr_t password3, uintptr_t passwordr1, void* vmcallinfo);

#define VMCALL_GETVERSION 0
#define VMCALL_CHANGEPASSWORD 1
#define VMCALL_READ_PHYSICAL_MEMORY 3
#define VMCALL_WRITE_PHYSICAL_MEMORY 4
#define VMCALL_REDIRECTINT1 9
#define VMCALL_INT1REDIRECTED 10
#define VMCALL_CHANGESELECTORS 12
#define VMCALL_BLOCK_INTERRUPTS 13
#define VMCALL_RESTORE_INTERRUPTS 14

#define VMCALL_REGISTER_CR3_EDIT_CALLBACK 16
#define VMCALL_RETURN_FROM_CR3_EDIT_CALLBACK 17
#define VMCALL_GETCR0 18
#define VMCALL_GETCR3 19
#define VMCALL_GETCR4 20
#define VMCALL_RAISEPRIVILEGE 21
#define VMCALL_REDIRECTINT14 22
#define VMCALL_INT14REDIRECTED 23
#define VMCALL_REDIRECTINT3 24
#define VMCALL_INT3REDIRECTED 25

//dbvm v6+
#define VMCALL_READMSR 26
#define VMCALL_WRITEMSR 27

#define VMCALL_ULTIMAP 28
#define VMCALL_ULTIMAP_DISABLE 29


//dbvm v7
#define VMCALL_SWITCH_TO_KERNELMODE 30
#define VMCALL_DISABLE_DATAPAGEFAULTS 31
#define VMCALL_ENABLE_DATAPAGEFAULTS 32
#define VMCALL_GETLASTSKIPPEDPAGEFAULT 33

#define VMCALL_ULTIMAP_PAUSE 34
#define VMCALL_ULTIMAP_RESUME 35

#define VMCALL_ULTIMAP_DEBUGINFO 36

#define VMCALL_PSODTEST 37

//dbvm11
#define VMCALL_GETMEM 38
#define VMCALL_JTAGBREAK 39
#define VMCALL_GETNMICOUNT 40

#define VMCALL_WATCH_WRITES 41
#define VMCALL_WATCH_READS 42
#define VMCALL_WATCH_RETRIEVELOG 43
#define VMCALL_WATCH_DELETE 44

#define VMCALL_CLOAK_ACTIVATE 45
#define VMCALL_CLOAK_DEACTIVATE 46
#define VMCALL_CLOAK_READORIGINAL 47
#define VMCALL_CLOAK_WRITEORIGINAL 48

#define VMCALL_CLOAK_CHANGEREGONBP 49
#define VMCALL_CLOAK_REMOVECHANGEREGONBP  50

#define VMCALL_EPT_RESET 51

#define VMCALL_LOG_CR3VALUES_START 52
#define VMCALL_LOG_CR3VALUES_STOP 53

#define VMCALL_REGISTERPLUGIN 54
#define VMCALL_RAISEPMI 55
#define VMCALL_ULTIMAP2_HIDERANGEUSAGE 56

#define VMCALL_ADD_MEMORY 57
//#define VMCALL_DISABLE_EPT 58 dus nut wurk


#ifdef STATISTICS
#define VMCALL_GET_STATISTICS 59
#endif

#define VMCALL_WATCH_EXECUTES 60
#define VMCALL_SETTSCADJUST   61
#define VMCALL_SETSPEEDHACK   62
#define VMCALL_CAUSEDDEBUGBREAK 63

#define VMCALL_DISABLE_TSCADJUST 64

#define VMCALL_CLOAKEX_ACTIVATE 65

#define VMCALL_DISABLETSCHOOK 66
#define VMCALL_ENABLETSCHOOK 67

#define VMCALL_WATCH_GETSTATUS 68

#define VMCALL_CLOAK_TRACEONBP 69
#define VMCALL_CLOAK_TRACEONBP_REMOVE 70
#define VMCALL_CLOAK_TRACEONBP_READLOG 71
#define VMCALL_CLOAK_TRACEONBP_GETSTATUS 72
#define VMCALL_CLOAK_TRACEONBP_STOPTRACE 73

#define VMCALL_GETBROKENTHREADLISTSIZE 74
#define VMCALL_GETBROKENTHREADENTRYSHORT 75
#define VMCALL_GETBROKENTHREADENTRYFULL 76
#define VMCALL_SETBROKENTHREADENTRYFULL 77
#define VMCALL_RESUMEBROKENTHREAD 78

#define VMCALL_HIDEDBVMPHYSICALADDRESSES 79
#define VMCALL_HIDEDBVMPHYSICALADDRESSESALL 80

#define VMCALL_KERNELMODE 100
#define VMCALL_USERMODE 101

#define VMCALL_DEBUG_SETSPINLOCKTIMEOUT 254

struct TChangeRegOnBPInfo {
	union {
		bool changeRAX : 1;			//0
		bool changeRBX : 1;			//1
		bool changeRCX : 1;			//2
		bool changeRDX : 1;        //3
		bool changeRSI : 1;        //4
		bool changeRDI : 1;        //5
		bool changeRBP : 1;        //6
		bool changeRSP : 1;        //7
		bool changeRIP : 1;        //8
		bool changeR8 : 1;        //9
		bool changeR9 : 1;        //10
		bool changeR10 : 1;        //11
		bool changeR11 : 1;        //12
		bool changeR12 : 1;        //13
		bool changeR13 : 1;        //14
		bool changeR14 : 1;        //15
		bool changeR15 : 1;        //16
		bool changeCF : 1;         //17
		bool changePF : 1;         //18
		bool changeAF : 1;         //19
		bool changeZF : 1;         //20
		bool changeSF : 1;         //21
		bool changeOF : 1;         //22
		bool newCF : 1;            //23
		bool newPF : 1;            //24
		bool newAF : 1;            //25
		bool newZF : 1;            //26
		bool newSF : 1;            //27
		bool newOF : 1;            //28
		bool reserved : 7;         //29,30,31
		};
	uint64_t changeXMM; //16 nibbles, each bit is one unsigned
	uint64_t changeFP; //just one bit, each bit is a fpu field
	uint64_t newRAX;
	uint64_t newRBX;
	uint64_t newRCX;
	uint64_t newRDX;
	uint64_t newRSI;
	uint64_t newRDI;
	uint64_t newRBP;
	uint64_t newRSP;
	uint64_t newRIP;
	uint64_t newR8;
	uint64_t newR9;
	uint64_t newR10;
	uint64_t newR11;
	uint64_t newR12;
	uint64_t newR13;
	uint64_t newR14;
	uint64_t newR15;
	uint64_t newFP0;
	uint64_t newFP0_H;
	uint64_t newFP1;
	uint64_t newFP1_H;
	uint64_t newFP2;
	uint64_t newFP2_H;
	uint64_t newFP3;
	uint64_t newFP3_H;
	uint64_t newFP4;
	uint64_t newFP4_H;
	uint64_t newFP5;
	uint64_t newFP5_H;
	uint64_t newFP6;
	uint64_t newFP6_H;
	uint64_t newFP7;
	uint64_t newFP7_H;
	uint64_t XMM0;
	uint64_t XMM0_H;
	uint64_t XMM1;
	uint64_t XMM1_H;
	uint64_t XMM2;
	uint64_t XMM2_H;
	uint64_t XMM3;
	uint64_t XMM3_H;
	uint64_t XMM4;
	uint64_t XMM4_H;
	uint64_t XMM5;
	uint64_t XMM5_H;
	uint64_t XMM6;
	uint64_t XMM6_H;
	uint64_t XMM7;
	uint64_t XMM7_H;
	uint64_t XMM8;
	uint64_t XMM8_H;
	uint64_t XMM9;
	uint64_t XMM9_H;
	uint64_t XMM10;
	uint64_t XMM10_H;
	uint64_t XMM11;
	uint64_t XMM11_H;
	uint64_t XMM12;
	uint64_t XMM12_H;
	uint64_t XMM13;
	uint64_t XMM13_H;
	uint64_t XMM14;
	uint64_t XMM14_H;
	uint64_t XMM15;
	uint64_t XMM15_H;
};

#pragma pack(push, 1)
class DBVM {
private:
	bool bIntel;
	uintptr_t current_password1, current_password3;
	unsigned current_password2;
public:
	static bool IsIntel() {
		int info[4];
		__cpuid(info, 0);
		if (info[1] == 0x756e6547 && info[3] == 0x49656e69 && info[2] == 0x6c65746e)
			return true;
		return false;
	}

	static bool IsAMD() {
		int info[4];
		__cpuid(info, 0);
		if (info[1] == 0x68747541 && info[3] == 0x69746E65 && info[2] == 0x444D4163)
			return true;
		return false;
	}

	uintptr_t dovmcall(void* vmcallinfo) const {
		if (bIntel)
			return vmcall_intel(current_password3, current_password1, vmcallinfo);
		return vmcall_amd(current_password3, current_password1, vmcallinfo);
	}

	uintptr_t GetMemory() const {
		struct
		{
			unsigned structsize;
			unsigned level2pass;
			unsigned command;
		} vmcallinfo;

		vmcallinfo.structsize = sizeof(vmcallinfo);
		vmcallinfo.level2pass = current_password2;
		vmcallinfo.command = VMCALL_GETMEM;

		return (unsigned)dovmcall(&vmcallinfo);
	}

	_CR3 GetCR3() const {
		struct
		{
			unsigned structsize;
			unsigned level2pass;
			unsigned command;
		} vmcallinfo;

		vmcallinfo.structsize = sizeof(vmcallinfo);
		vmcallinfo.level2pass = current_password2;
		vmcallinfo.command = VMCALL_GETCR3;

		_CR3 CR3;
		CR3.Value = dovmcall(&vmcallinfo);
		return CR3;
	}

	bool ReadPhysicalMemory(PhysicalAddress srcPA, void* dstVA, SIZE_T size) const {
		struct
		{
			unsigned structsize;
			unsigned level2pass;
			unsigned command;
			PhysicalAddress srcPA;
			unsigned size;
			uintptr_t dstVA;
			unsigned nopagefault;
		} vmcallinfo;

		vmcallinfo.structsize = sizeof(vmcallinfo);
		vmcallinfo.level2pass = current_password2;
		vmcallinfo.command = VMCALL_READ_PHYSICAL_MEMORY;
		vmcallinfo.srcPA = srcPA;
		vmcallinfo.size = (unsigned)size;
		vmcallinfo.dstVA = (uintptr_t)dstVA;
		vmcallinfo.nopagefault = true;

		return dovmcall(&vmcallinfo) == 0;
	}

	bool WritePhysicalMemory(PhysicalAddress dstPA, LPCVOID srcVA, SIZE_T size) const {
		struct
		{
			unsigned structsize;
			unsigned level2pass;
			unsigned command;
			PhysicalAddress dstPA;
			unsigned size;
			uintptr_t srcVA;
			unsigned nopagefault;
		} vmcallinfo;

		vmcallinfo.structsize = sizeof(vmcallinfo);
		vmcallinfo.level2pass = current_password2;
		vmcallinfo.command = VMCALL_WRITE_PHYSICAL_MEMORY;
		vmcallinfo.dstPA = dstPA;
		vmcallinfo.size = (unsigned)size;
		vmcallinfo.srcVA = (uintptr_t)srcVA;
		vmcallinfo.nopagefault = true;

		return dovmcall(&vmcallinfo) == 0;
	}

	uintptr_t SwitchToKernelMode(uint16 newCS) const {
		struct
		{
			unsigned structsize;
			unsigned level2pass;
			unsigned command;
			uint16 newCS;
		} vmcallinfo;

		vmcallinfo.structsize = sizeof(vmcallinfo);
		vmcallinfo.level2pass = current_password2;
		vmcallinfo.command = VMCALL_KERNELMODE;
		vmcallinfo.newCS = newCS;

		return dovmcall(&vmcallinfo);
	}

	void ReturnToUserMode() const {
		struct
		{
			unsigned structsize;
			unsigned level2pass;
			unsigned command;
		} vmcallinfo;

		vmcallinfo.structsize = sizeof(vmcallinfo);
		vmcallinfo.level2pass = current_password2;
		vmcallinfo.command = VMCALL_USERMODE;

		dovmcall(&vmcallinfo);
	}

	void SetCR3(CR3 cr3) const {
		SwitchToKernelMode(0x10);
		__writecr3(cr3.Value);
		ReturnToUserMode();
	}

	bool ChangeRegisterOnBP(PhysicalAddress PABase, TChangeRegOnBPInfo changeregonbpinfo) const {
		struct
		{
			unsigned structsize;
			unsigned level2pass;
			unsigned command;
			PhysicalAddress PABase;
			TChangeRegOnBPInfo changeregonbpinfo;
		} vmcallinfo;

		vmcallinfo.structsize = sizeof(vmcallinfo);
		vmcallinfo.level2pass = current_password2;
		vmcallinfo.command = VMCALL_CLOAK_CHANGEREGONBP;
		vmcallinfo.PABase = PABase;
		vmcallinfo.changeregonbpinfo = changeregonbpinfo;

		return dovmcall(&vmcallinfo) == 0;
	}

	bool RemoveChangeRegisterOnBP(PhysicalAddress PABase) const {
		struct
		{
			unsigned structsize;
			unsigned level2pass;
			unsigned command;
			PhysicalAddress PABase;
			TChangeRegOnBPInfo changeregonbpinfo;
		} vmcallinfo;

		vmcallinfo.structsize = sizeof(vmcallinfo);
		vmcallinfo.level2pass = current_password2;
		vmcallinfo.command = VMCALL_CLOAK_REMOVECHANGEREGONBP;
		vmcallinfo.PABase = PABase;

		return dovmcall(&vmcallinfo) == 0;
	}

	bool CloakWriteOriginal(PhysicalAddress PABase, void* Src) const {
		struct
		{
			unsigned structsize;
			unsigned level2pass;
			unsigned command;
			PhysicalAddress PABase;
			void* Src;
		} vmcallinfo;

		vmcallinfo.structsize = sizeof(vmcallinfo);
		vmcallinfo.level2pass = current_password2;
		vmcallinfo.command = VMCALL_CLOAK_WRITEORIGINAL;
		vmcallinfo.PABase = PABase;
		vmcallinfo.Src = Src;

		return dovmcall(&vmcallinfo) == 0;
	}

	bool CloakReadOriginal(PhysicalAddress PABase, void* Dst) const {
		struct
		{
			unsigned structsize;
			unsigned level2pass;
			unsigned command;
			PhysicalAddress PABase;
			void* Dst;
		} vmcallinfo;

		vmcallinfo.structsize = sizeof(vmcallinfo);
		vmcallinfo.level2pass = current_password2;
		vmcallinfo.command = VMCALL_CLOAK_READORIGINAL;
		vmcallinfo.PABase = PABase;
		vmcallinfo.Dst = Dst;

		return dovmcall(&vmcallinfo) == 0;
	}

	uintptr_t CloakActivate(PhysicalAddress PABase, int Mode = 1) const {
		struct
		{
			unsigned structsize;
			unsigned level2pass;
			unsigned command;
			PhysicalAddress PABase;
			uintptr_t Mode;
		} vmcallinfo;

		vmcallinfo.structsize = sizeof(vmcallinfo);
		vmcallinfo.level2pass = current_password2;
		vmcallinfo.command = VMCALL_CLOAK_ACTIVATE;
		vmcallinfo.PABase = PABase;
		vmcallinfo.Mode = Mode;

		//1 already clocked
		//0 success
		return dovmcall(&vmcallinfo);
	}

	uintptr_t CloakDeactivate(PhysicalAddress PABase) const {
		struct
		{
			unsigned structsize;
			unsigned level2pass;
			unsigned command;
			PhysicalAddress PABase;
		} vmcallinfo;

		vmcallinfo.structsize = sizeof(vmcallinfo);
		vmcallinfo.level2pass = current_password2;
		vmcallinfo.command = VMCALL_CLOAK_DEACTIVATE;
		vmcallinfo.PABase = PABase;

		//0 success
		return dovmcall(&vmcallinfo);
	}

	void CloakReset() const {
		struct
		{
			unsigned structsize;
			unsigned level2pass;
			unsigned command;
		} vmcallinfo;

		vmcallinfo.structsize = sizeof(vmcallinfo);
		vmcallinfo.level2pass = current_password2;
		vmcallinfo.command = VMCALL_EPT_RESET;

		dovmcall(&vmcallinfo);
	}

	void HideDBVM() const {
		struct
		{
			unsigned structsize;
			unsigned level2pass;
			unsigned command;
		} vmcallinfo;

		vmcallinfo.structsize = sizeof(vmcallinfo);
		vmcallinfo.level2pass = current_password2;
		vmcallinfo.command = VMCALL_HIDEDBVMPHYSICALADDRESSESALL;

		dovmcall(&vmcallinfo);
	}

	void ChangePassword(uintptr_t password1, unsigned password2, uintptr_t password3) {
		struct
		{
			unsigned structsize;
			unsigned level2pass;
			unsigned command;
			uintptr_t password1;
			unsigned password2;
			uintptr_t password3;
		} vmcallinfo;

		vmcallinfo.structsize = sizeof(vmcallinfo);
		vmcallinfo.level2pass = current_password2;
		vmcallinfo.command = VMCALL_CHANGEPASSWORD;
		vmcallinfo.password1 = password1;
		vmcallinfo.password2 = password2;
		vmcallinfo.password3 = password3;

		dovmcall(&vmcallinfo);
		SetPassword(password1, password2, password3);
	}

	void SetPassword(uintptr_t password1, unsigned password2, uintptr_t password3) {
		current_password1 = password1;
		current_password2 = password2;
		current_password3 = password3;
	}

	void SetDefaultPassword() {
		current_password1 = 0x76543210;
		current_password2 = 0xfedcba98;
		current_password3 = 0x90909090;
	}

	unsigned GetVersion() const {
		struct
		{
			unsigned structsize;
			unsigned level2pass;
			unsigned command;
		} vmcallinfo;

		vmcallinfo.structsize = sizeof(vmcallinfo);
		vmcallinfo.level2pass = current_password2;
		vmcallinfo.command = VMCALL_GETVERSION;

		unsigned ret;
		if (!ExceptionHandler::TryExcept([&]() { ret = (unsigned)dovmcall(&vmcallinfo); }))
			return 0;

		if ((ret >> 24) != 0xCE)
			return 0;

		return ret & 0xffffff;
	}

	PhysicalAddress GetPTEAddress(uintptr_t VirtualAddress, CR3 cr3) const {
		return ::GetPTEAddressByPhysicalMemoryAccess(VirtualAddress, cr3, [&](PhysicalAddress PA, void* Buffer, size_t Size) {
			return ReadPhysicalMemory(PA, Buffer, Size);
			});
	}

	PhysicalAddress GetPhysicalAddress(uintptr_t VirtualAddress, CR3 cr3) const {
		return ::GetPhysicalAddressByPhysicalMemoryAccess(VirtualAddress, cr3, [&](PhysicalAddress PA, void* Buffer, size_t Size) {
			return ReadPhysicalMemory(PA, Buffer, Size);
			});
	}

	bool WPM(uintptr_t Address, const void* Buffer, size_t Size, CR3 cr3) const {
		return WriteProcessMemoryByPhysicalMemoryAccess(Address, Buffer, Size, cr3,
			[&](PhysicalAddress PA, void* Buffer, size_t Size) { return ReadPhysicalMemory(PA, Buffer, Size); },
			[&](PhysicalAddress PA, const void* Buffer, size_t Size) { return WritePhysicalMemory(PA, Buffer, Size); });
	}

	bool RPM(uintptr_t Address, void* Buffer, size_t Size, CR3 cr3) const {
		return ReadProcessMemoryByPhysicalMemoryAccess(Address, Buffer, Size, cr3,
			[&](PhysicalAddress PA, void* Buffer, size_t Size) { return ReadPhysicalMemory(PA, Buffer, Size); });
	}

	bool WPMCloak(uintptr_t Address, const void* Buffer, size_t Size, CR3 cr3) const {
		return WriteProcessMemoryByPhysicalMemoryAccess(Address, Buffer, Size, cr3,
			[&](PhysicalAddress PA, void* Buffer, size_t Size) { return ReadPhysicalMemory(PA, Buffer, Size); },
			[&](PhysicalAddress PA, const void* Buffer, size_t Size) {
				PhysicalAddress PABase = PA & ~0xFFF;
				CloakActivate(PABase, 0);

				uint8_t buf[0x1000];
				if (!CloakReadOriginal(PABase, buf))
					return false;

				memcpy(buf + PA - PABase, Buffer, Size);

				if (!CloakWriteOriginal(PABase, buf))
					return false;

				return true;
			});
	}

	bool RemoveCloak(uintptr_t Address, size_t Size, CR3 cr3) const {
		return ReadProcessMemoryByPhysicalMemoryAccess(Address, 0, Size, cr3,
			[&](PhysicalAddress PA, void* Buffer, size_t Size) { 
				uintptr_t PABase = PA & ~0xFFF;
				CloakDeactivate(PABase);
				return true;
			});
	}

	DBVM(uintptr_t password1 = 0x76543210, unsigned password2 = 0xfedcba98, uintptr_t password3 = 0x90909090) : current_password1(password1), current_password2(password2), current_password3(password3), bIntel(IsIntel()) {}
};
#pragma pack(pop)