/***************************************************************************************
* AUTHOR : LycorisGuard
* DATE   : 2018-1-20
* MODULE : ProcessManagerDrv.H
****************************************************************************************/

#ifndef CXX_PROCESSMANAGERRING0_H
#define CXX_PROCESSMANAGERRING0_H

#include <ntifs.h>
#include <devioctl.h>
#include "Common.h"


#define DEVICE_NAME   L"\\Device\\ProcessManagerDeviceName"
#define LINK_NAME       L"\\DosDevices\\ProcessManagerLinkName"

#define CTL_ENUMPROCESSBYOPENPROCESS \
	CTL_CODE(FILE_DEVICE_UNKNOWN,0x830,METHOD_NEITHER,FILE_ANY_ACCESS)

#define CTL_ENUMPROCESSACTIVELIST \
	CTL_CODE(FILE_DEVICE_UNKNOWN,0x831,METHOD_NEITHER,FILE_ANY_ACCESS)
#define CTL_ENUMPROCESSPSPCIDTABLE \
	CTL_CODE(FILE_DEVICE_UNKNOWN,0x832,METHOD_NEITHER,FILE_ANY_ACCESS)

//Òþ²Ø½ø³Ì
#define CTL_HIDEPROCESS \
	CTL_CODE(FILE_DEVICE_UNKNOWN,0x833,METHOD_NEITHER,FILE_ANY_ACCESS)


//½ø³Ì¼à¿Ø
#define CTL_MONITORPROCESS \
	CTL_CODE(FILE_DEVICE_UNKNOWN,0x834,METHOD_NEITHER,FILE_ANY_ACCESS)
#define CTL_GETRPROCESSINFOR \
	CTL_CODE(FILE_DEVICE_UNKNOWN,0x835,METHOD_NEITHER,FILE_ANY_ACCESS)
#define CTL_SUSMONITOR \
	CTL_CODE(FILE_DEVICE_UNKNOWN,0x836,METHOD_NEITHER,FILE_ANY_ACCESS)
typedef struct _MSG_
{
	ULONG  ulCreate;
	WCHAR wzProcessPath[512];
}MSG,*PMSG;

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING pRegistryString);
VOID DriverUnload(IN PDRIVER_OBJECT DriverObject);
NTSTATUS DispatchDeviceControl(IN PDEVICE_OBJECT DeviceObject, IN PIRP pIrp);
NTSTATUS
	DefaultPassThrough(PDEVICE_OBJECT  DeviceObject,PIRP Irp);


#endif
