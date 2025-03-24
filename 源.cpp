
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
	printf("��һ����(��������)��\n");
	// int flag = 0;
	for (size_t i = 0; i < len; ++i)
	{
		unsigned char c = buf[i]; // must use unsigned char to print >128 value 

		if (c < 16)
			printf("0%x ", c);
		else
			printf("%x ", c);

		if (i == 445)
			printf("\n\n�ڶ�����(������)��\n");

		if (i == 509)
			printf("\n\n��������(������־)��\n");
		if ((i + 1) % 25 == 0)
			printf("\n");
	}
	printf("\n\n");
}

BOOL GetDriveGeometry(DISK_GEOMETRY* pdg)
{
	HANDLE hDevice;// �豸���
	BOOL bResult;// results flag
	DWORD junk;// discard results
	char lpBuffer[BufferLength] = { 0 };


	//ͨ��CreateFile������豸�ľ��
	hDevice = CreateFile(TEXT("\\\\.\\E:"), // �豸����,����ָ��һ��Ӳ��
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


	//ͨ��DeviceIoControl�������豸����IO
	bResult = DeviceIoControl(hDevice, // �豸�ľ��
		IOCTL_DISK_GET_DRIVE_GEOMETRY, // �����룬ָ���豸������
		NULL, 0, // no input buffer
		pdg, sizeof(*pdg),// output buffer �����������̲�����Ϣ
		&junk,// # bytes returned
		(LPOVERLAPPED)NULL); // synchronous I/O
	 //������������λ��Ϊ0����0��ͷ1����
	int BlockAddr = (0 * 256 + 0) * 63 + 1 - 1; //������Ե�ַ
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
	DISK_GEOMETRY pdg;// ������̲����Ľṹ��
	BOOL bResult;// generic results flag
	ULONGLONG DiskSize;// size of the drive, in bytes

	bResult = GetDriveGeometry(&pdg);

	if (bResult)
	{
		printf("������ = %ld\n", pdg.Cylinders);                        //������
		printf("ÿ����Ĵŵ��� = %ld\n", (ULONG)pdg.TracksPerCylinder); //ÿ����Ĵŵ���
		printf("ÿ�ŵ������� = %ld\n", (ULONG)pdg.SectorsPerTrack);    //ÿ�ŵ�������
		printf("ÿ�������ֽ��� = %ld\n", (ULONG)pdg.BytesPerSector);   //ÿ�������ֽ���


		DiskSize = pdg.Cylinders.QuadPart * (ULONG)pdg.TracksPerCylinder *
			(ULONG)pdg.SectorsPerTrack * (ULONG)pdg.BytesPerSector;

		printf("���̴�С = %I64d (Bytes) = %I64d (Gb)\n", DiskSize,
			DiskSize / (1024 * 1024 * 1024));
	}
	else
	{
		printf("GetDriveGeometry failed. Error %ld.\n", GetLastError());
	}
	return ((int)bResult);
}