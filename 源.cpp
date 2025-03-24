
/* The code of interest is in the subroutine GetDriveGeometry. The
code in main shows how to interpret the results of the call. */
#include <windows.h>
#include <winioctl.h>
#include <stdio.h>
#define BufferLength 1024

void HexOutput(char* buf, size_t len)
{
	// unsigned char a = buf[0];
	// printf("%x",a);
	// printf("The Hex output of data :\n\t0x"); 
	printf("第一部分(引导代码)：\n");
	// int flag = 0;
	for (size_t i = 0; i < len; ++i)
	{
		unsigned char c = buf[i]; // must use unsigned char to print >128 value 

		if (c < 16)
			printf("0%x ", c);
		else
			printf("%x ", c);

		if (i == 445)
			printf("\n\n第二部分(分区表)：\n");

		if (i == 509)
			printf("\n\n第三部分(结束标志)：\n");
		if ((i + 1) % 25 == 0)
			printf("\n");
	}
	printf("\n\n");
}

BOOL GetDriveGeometry(DISK_GEOMETRY* pdg)
{
	HANDLE hDevice;// 设备句柄
	BOOL bResult;// results flag
	DWORD junk;// discard results
	char lpBuffer[BufferLength] = { 0 };


	//通过CreateFile来获得设备的句柄
	hDevice = CreateFile(TEXT("\\\\.\\E:"), // 设备名称,这里指第一块硬盘
		GENERIC_READ,// no access to the drive
		FILE_SHARE_READ | // share mode
		FILE_SHARE_WRITE,
		NULL,// default security attributes
		OPEN_EXISTING,// disposition
		0,// file attributes
		NULL);// do not copy file attributes

	if (hDevice == INVALID_HANDLE_VALUE) // cannot open the drive
	{
		return (FALSE);
	}


	//通过DeviceIoControl函数与设备进行IO
	bResult = DeviceIoControl(hDevice, // 设备的句柄
		IOCTL_DISK_GET_DRIVE_GEOMETRY, // 控制码，指明设备的类型
		NULL, 0, // no input buffer
		pdg, sizeof(*pdg),// output buffer 输出，保存磁盘参数信息
		&junk,// # bytes returned
		(LPOVERLAPPED)NULL); // synchronous I/O
	 //主引导扇区的位置为0柱面0磁头1扇区
	int BlockAddr = (0 * 256 + 0) * 63 + 1 - 1; //计算绝对地址
	SetFilePointer(hDevice, (BlockAddr * 512), NULL, FILE_BEGIN);

	// printf("%ld\n",GetLastError());
	DWORD dwCB;
	BOOL bRet = ReadFile(hDevice, lpBuffer, 512, &dwCB, NULL);
	// printf("%x\n\n",lpBuffer);
	HexOutput(lpBuffer, 512);

	CloseHandle(hDevice);

	return (bResult);
}

int main()
{
	DISK_GEOMETRY pdg;// 保存磁盘参数的结构体
	BOOL bResult;// generic results flag
	ULONGLONG DiskSize;// size of the drive, in bytes

	bResult = GetDriveGeometry(&pdg);

	if (bResult)
	{
		printf("柱面数 = %ld\n", pdg.Cylinders);                        //柱面数
		printf("每柱面的磁道数 = %ld\n", (ULONG)pdg.TracksPerCylinder); //每柱面的磁道数
		printf("每磁道扇区数 = %ld\n", (ULONG)pdg.SectorsPerTrack);    //每磁道扇区数
		printf("每扇区的字节数 = %ld\n", (ULONG)pdg.BytesPerSector);   //每扇区的字节数


		DiskSize = pdg.Cylinders.QuadPart * (ULONG)pdg.TracksPerCylinder *
			(ULONG)pdg.SectorsPerTrack * (ULONG)pdg.BytesPerSector;

		printf("磁盘大小 = %I64d (Bytes) = %I64d (Gb)\n", DiskSize,
			DiskSize / (1024 * 1024 * 1024));
	}
	else
	{
		printf("GetDriveGeometry failed. Error %ld.\n", GetLastError());
	}
	return ((int)bResult);
}