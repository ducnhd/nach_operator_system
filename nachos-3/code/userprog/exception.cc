// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#define MaxFileLength 32
#define MAX_INT_LENGTH 100
#define MAX_STRING_LENGTH 1000

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

// Ham tang program counter 
void
IncreaseProgramCounterRegs()
{
	int progCounter;

	// Dich chuyen sang dia chi cau lenh tiep theo ma chuong trinh thuc hien
	progCounter = machine->ReadRegister(PCReg);
	machine->WriteRegister(PrevPCReg, progCounter);
	progCounter = machine->ReadRegister(NextPCReg);
	machine->WriteRegister(PCReg, progCounter);
	progCounter += 4; // Day la dia chi cua cau lenh tiep theo
	machine->WriteRegister(NextPCReg, progCounter);
}

// Input: - User space address (int)
// - Limit of buffer (int)
// Output:- Buffer (char*)
// Purpose: Copy buffer from User memory space to System memory space
char* User2System(int virtAddr, int limit)
{
	int i;// index
	int oneChar;
	char* kernelBuf = NULL;
	kernelBuf = new char[limit + 1];//need for terminal string
	if (kernelBuf == NULL)
		return kernelBuf;
	memset(kernelBuf, 0, limit + 1);
	//printf("\n Filename u2s:");
	for (i = 0; i < limit;i++)
	{
		machine->ReadMem(virtAddr + i, 1, &oneChar);
		kernelBuf[i] = (char)oneChar;
		//printf("%c",kernelBuf[i]);
		if (oneChar == 0)
			break;
	}
	return kernelBuf;
}

// Input: - User space address (int)
// - Limit of buffer (int)
// - Buffer (char[])
// Output:- Number of bytes copied (int)
// Purpose: Copy buffer from System memory space to User memory space
int System2User(int virtAddr, int len, char* buffer)
{
	if (len < 0) return -1;
	if (len == 0)return len;
	int i = 0;
	int oneChar = 0;
	do {
		oneChar = (int)buffer[i];
		machine->WriteMem(virtAddr + i, 1, oneChar);
		i++;
	} while (i < len && oneChar != 0);
	return i;
}

// Input: reg4 -filename (string)
// Output: reg2 -1: error and 0: success
// Purpose: process the event SC_Create of System call
void ExceptionHandler(ExceptionType which)
{
	int type = machine->ReadRegister(2);

	switch (which)
	{
	case NoException:
		break;
	case SyscallException:
		switch (type)
		{
		case SC_Halt:
		{
			DEBUG('a', "\n Shutdown, initiated by user program.");
			printf("\n\n Shutdown, initiated by user program.");
			interrupt->Halt();
			break;
		}

		case SC_CreateF:
		{
			int virtAddr;
			char* filename;
			DEBUG('a', "\n SC_Create call ...");
			DEBUG('a', "\n Reading virtual address of filename");
			// Lay tham so ten tap tin tu thanh ghi r4, ten file
			virtAddr = machine->ReadRegister(4);
			DEBUG('a', "\n Reading filename.");
			// MaxFileLength = 32
			filename = User2System(virtAddr, MaxFileLength + 1); //chuyen tu ten file nhap tu nguoi dung vao he thong de xu ly
			if (filename == NULL) //khong du cho trong de doc ten file vao he thong
			{
				printf("\n Not enough memory in system");
				DEBUG('a', "\n Not enough memory in system");
				machine->WriteRegister(2, -1); // tra ve loi cho chuong
							       // trinh nguoi dung
				delete filename;
				break;
			}
			if (filename[MaxFileLength] != '\0') //qua chieu dai toi da cua ten file 
			{
				printf("\n Too many characters in filename: %s", filename);
				DEBUG('a', "\n Too many characters in filename");
				machine->WriteRegister(2, -1); //bao loi, return -1
				delete filename;
			}
			if (strlen(filename) == 0) //ten file chua co
			{
				printf("\n Don't have any character in filename: %s", filename);
				DEBUG('a', "\n Don't have any character in filename");
				machine->WriteRegister(2, -1);
				delete filename;
			}
			DEBUG('a', "\n Finish reading filename.");
			// Create file with size = 0
			// Dung doi tuong fileSystem cua lop OpenFile de tao file,
			// viec tao file nay la su dung cac thu tuc tao file cua he dieu
			// hanh Linux, chung ta khong quan ly truc tiep cac block tren
			// dia cung cap phat cho file.
			if (!fileSystem->Create(filename, 0)) //tao file moi voi size 0
			{
				printf("\n Error create file '%s'", filename); //thong bao khong the tao duoc file 
				machine->WriteRegister(2, -1); //Tra ve loi
				delete filename;
				break;
			}
			machine->WriteRegister(2, 0); // tra ve cho chuong trinh
						      // nguoi dung thanh cong
			delete filename;
			break;
		}

		case SC_OpenF:
		{
			int virtAddr;
			char* filename;
			int itype;
			OpenFileId idFile;

			DEBUG('a', "\n SC_Open call ...");
			DEBUG('a', "\n Reading virtual address of filename");
			// Lay tham so ten tap tin tu thanh ghi r4, ten file
			virtAddr = machine->ReadRegister(4);
			// Lay tham so kieu tap tin tu thanh ghi r5, kieu cho phep truy cap vao tap tin nhu the nao
			itype = machine->ReadRegister(5);
			DEBUG('a', "\n Reading filename.");
			filename = User2System(virtAddr, MaxFileLength + 1); //chuyen tu ten file nhap tu nguoi dung vao he thong de xu ly
			if (filename == NULL)//khong du bo nho trong he thong de luu ten file va xu ly
			{
				printf("\n Not enough memory in system");
				DEBUG('a', "\n Not enough memory in system");
				machine->WriteRegister(2, -1); // tra ve loi cho chuong
							       // trinh nguoi dung
				delete filename;
				break;
			}
			DEBUG('a', "\n Finish opening filename.");

			idFile = currentThread->fdTable->FindIdFile(filename, itype); //tim idFile theo thread hien tai va su dung class fileSystem
										      //de mo file do, dong thoi danh dau id cua file trong bitmap
										      //la dang duoc su dung
			printf("\n idFile %d", idFile);

			machine->WriteRegister(2, idFile); // tra ve cho chuong trinh
				                           // nguoi dung thanh cong
							   // tham so tra ve idFile
			delete filename;
			break;
		}

		case SC_ReadF:
		{
			int virtAddr;
			char* buffer; //tao buffer luu tru ket qua se doc ra
			int len;
			int sizeBuffer;
			int i;
			OpenFileId idFile;
			OpenFile *openFile = NULL;

			DEBUG('a', "\n SC_Read call ...");
			DEBUG('a', "\n Reading virtual address of buffer");
			// Lay tham so chuoi duoc dung de luu tru du lieu tu file tu thanh ghi r4, chuoi luu tru cac ki tu doc ra tu file
			virtAddr = machine->ReadRegister(4);
			// Lay tham so do dai cac byte se doc tu tap tin tu thanh ghi r5, do dai so byte se doc
			sizeBuffer = machine->ReadRegister(5);
			// Lay tham so id tap tin tu thanh ghi r6, id File
			idFile = machine->ReadRegister(6);

			if (sizeBuffer <= 0) // Kiem tra xem kich thuoc can doc co phai la so duong.
			{
				printf("\n length <= 0");
				DEBUG('a', "\n length <= 0");
				machine->WriteRegister(2, -1); // tra ve loi cho chuong
							       // trinh nguoi dung
				break;
			}

			if (currentThread->fdTable->isRead(idFile) == false) //kiem tra xem idFile do hien tai co the doc file ra duoc khong
			{
				printf("\n Error isRead %d", idFile);//thong bao khong doc duoc id File nay
				DEBUG('a', "\n Error isRead");
				machine->WriteRegister(2, -1); // tra ve loi cho chuong
				                               // trinh nguoi dung
				break;
			}
			//Hoan tat kiem tra 

			buffer = new char[sizeBuffer + 1]; 

			if (buffer == NULL)//khong cap phat vung nho duoc cho buffer
			{
				printf("\n Not enough memory in system"); 
				DEBUG('a', "\n Not enough memory in system");
				machine->WriteRegister(2, -1); // tra ve loi cho chuong
							       // trinh nguoi dung
				delete buffer;
				break;
			}
			for (i = 0; i <= sizeBuffer; ++i)
			{
				buffer[i] = NULL;
			}

			if (idFile == 0) //idFile = 0, console input
			{
				len = gSynchConsole->Read(buffer, sizeBuffer);	// Doc du lieu tu console
										// sang mang buffer
				if (len == -1) // Kiem tra xem co nhap CTRL-A khong?
				{
					machine->WriteRegister(2, -1);
					delete buffer;
					break;
				}
				System2User(virtAddr, len + 1, buffer); // Chuyen du lieu tu bo nho
									// Kernelspace sang Userspace

				machine->WriteRegister(2, len); // Tra ve chieu dai cua chuoi co the doc duoc
				delete buffer;
				break;
			}
			openFile = currentThread->fdTable->Open(idFile);
			if (openFile == NULL)//khong mo duoc idFile nay vi no khong nam trong khoang 2->10
			{
				printf("\n Error ");
				DEBUG('a', "\n Error ");
				machine->WriteRegister(2, -1); // tra ve loi cho chuong
							       // trinh nguoi dung
				delete[] buffer;
				break;
			}
			DEBUG('a', "\n Finish opening file.");
			len = openFile->Read(buffer, sizeBuffer); //doc tu file mot khoang sizeBuffer
			if (len == 0)  // Cuoi file
			{
				printf("\n Error eof read \n");
				DEBUG('a', "\n Error eof");
				machine->WriteRegister(2, -2);
				delete[] buffer;
				break;
			}
			System2User(virtAddr, len + 1, buffer); //tra du lieu ve cho nguoi dung 
			machine->WriteRegister(2, len);
			delete buffer;
			break;
		}

		case SC_WriteF:
		{
			int virtAddr;
			char* buffer;
			int len;
			int sizeBuffer;
			OpenFileId idFile;
			OpenFile *openFile = NULL;

			DEBUG('a', "\n SC_Write call ...");
			DEBUG('a', "\n Reading virtual address of buffer");

			// Lay tham so chuoi ghi vao file tu thanh ghi r4, chuoi ghi vao file
			virtAddr = machine->ReadRegister(4);
			// Lay tham so do dai cac byte se ghi vao tap tin tu thanh ghi r5, do dai so byte se doc
			sizeBuffer = machine->ReadRegister(5);
			// Lay tham so id tap tin tu thanh ghi r6, id File
			idFile = machine->ReadRegister(6);

			if (sizeBuffer <= 0) // Kiem tra xem kich thuoc can viet co phai la so duong.
			{
				printf("\n length <= 0");
				DEBUG('a', "\n length <= 0");
				machine->WriteRegister(2, -1); // tra ve loi cho chuong
							       // trinh nguoi dung
				break;
			}

			if (currentThread->fdTable->isWrite(idFile) == false)//Kiem tra xem idFile hien tai co the ghi vao duoc khong
			{
				printf("\n Error");
				DEBUG('a', "\n Error");
				machine->WriteRegister(2, -1); // tra ve loi cho chuong
							       // trinh nguoi dung
				break;
			}

			DEBUG('a', "\n Reading buffer.");
			buffer = User2System(virtAddr, sizeBuffer); //doc du lieu tu chuoi ghi xuong file vao system de xu ly
			if (buffer == NULL)//khong du cho trong trong he thong de luu tru chuoi can ghi xuong file
			{
				printf("\n Not enough memory in system");
				DEBUG('a', "\n Not enough memory in system");
				machine->WriteRegister(2, -1); // tra ve loi cho chuong
							       // trinh nguoi dung
				delete buffer;
				break;
			}

			if (idFile == 1)//console output
			{
				int i = 0;
				// Ghi tung ki tu ra man hinh console
				while (buffer[i] != 0 && i < sizeBuffer)
				{
					gSynchConsole->Write(&buffer[i], 1);
					++i;
				}

				DEBUG('a', "\n Finish printing string.");
				machine->WriteRegister(2, i); // Tra ve chieu dai cua chuoi can xuat
				delete buffer;
				break;
			}
			openFile = currentThread->fdTable->Open(idFile);
			if (openFile == NULL)//khong the mo duoc file do id khong nam trong vung 2->10
			{
				printf("\n Error ");
				DEBUG('a', "\n Error ");
				machine->WriteRegister(2, -1); // tra ve loi cho chuong
							       // trinh nguoi dung
				delete buffer;
				break;
			}

			DEBUG('a', "\n Finish opening file.");
			len = openFile->Write(buffer, sizeBuffer);//ghi vao file chuoi buffer
			//int i = 0;
			//while (buffer[i] != 0 && i < sizeBuffer && len == 1)
			//{
			//	len = openFile->Write(&buffer[i], sizeBuffer);
			//	++i;
			//}
			if (len == 0) // Cuoi file
			{
				printf("\n Error eof");
				DEBUG('a', "\n Error eof");
				machine->WriteRegister(2, -2);
				delete[] buffer;
				break;
			}
			machine->WriteRegister(2, len);
			delete buffer;
			break;
		}

		case SC_SeekF:
		{
			int position;
			OpenFileId idFile;
			OpenFile *openFile = NULL;

			DEBUG('a', "\n SC_Seek call ...");

			// Lay tham so khoang cach de seek con tro trong tap tin tu thanh ghi r4, khoang cach di chuyen con tro trong file
			position = machine->ReadRegister(4);
			// Lay tham so id tap tin tu thanh ghi r5, id File
			idFile = machine->ReadRegister(5);

			openFile = currentThread->fdTable->Open(idFile);
			if (openFile == NULL)//khong the mo duoc file do id khong nam trong vung 2->10
			{
				printf("\n Error ");
				DEBUG('a', "\n Error");
				machine->WriteRegister(2, -1); // tra ve loi cho chuong
							       // trinh nguoi dung
				break;
			}

			DEBUG('a', "\n Finish seeking file.");
			if (position < 0)
			{
				printf("\n position < 0");
				DEBUG('a', "\n position < 0");
				machine->WriteRegister(2, -1); // tra ve loi cho chuong
							       // trinh nguoi dung
				break;
			}
			openFile->Seek(position);
			machine->WriteRegister(2, 0);
		}

		case SC_CloseF:
		{
			OpenFileId idFile;

			DEBUG('a', "\n SC_Close call ...");

			// Lay tham so id tap tin tu thanh ghi r4, id File
			idFile = machine->ReadRegister(4);

			if (currentThread->fdTable->Close(idFile) == -1)
			{
				printf("\n Error ");
				DEBUG('a', "\n Error ");
				machine->WriteRegister(2, -1); // tra ve loi cho chuong
							       // trinh nguoi dung
				break;
			}
			machine->WriteRegister(2, 0);
			break;
		}

		case SC_ReadInt:
		{
			int num = 0, ele = 0;
			int count = 0, tmp = 0;
			char* buf = new char[MAX_INT_LENGTH + 1];
			if (buf == NULL)//khong du bo nho khoi tao buffer
			{
				printf("\n Not enough memory in system");
				DEBUG('a', "\n Not enough memory in system");
				machine->WriteRegister(2, -1);
				delete buf;
				return;
			}
			count = gSynchConsole->Read(buf, MAX_INT_LENGTH + 1); //doc input tu console
			if (count == MAX_INT_LENGTH + 1) //so qua lon
			{
				machine->WriteRegister(2, -2);
				delete buf;
				break;
			}
			if (count == -1) //Ctrl+A
			{
				machine->WriteRegister(2, -1);
				delete buf;
				break;
			}
			if (count == 0)
			{
				machine->WriteRegister(2, 0);
				delete buf;
				break;
			}

			int i = 0;
			if (buf[0] == '-')
				i = 1;
			for (; i < count; ++i)
			{
				if (ele == 10)//Out of length
				{
					num = 0;
					break;
				}
				tmp = (int)buf[i];
				if (tmp<48 || tmp>57)//Not a number
				{
					num = 0;
					break;
				}
				if ((num > 214748364) || (num == 214748364 && ((buf[0] == '-'&&tmp - 48 > 8) || (buf[0] != '-'&&tmp - 48 > 7))))//Bigger than INT_MAX or smaller than INT_MIN
				{
					num = 0;
					break;
				}
				num = num * 10 + tmp - 48;
				if (num != 0)
					++ele;
			}
			if (buf[0] == '-')
				num = -1 * num;
			machine->WriteRegister(2, num);
			delete buf;
			break;
		}
		case SC_PrintInt:
		{
			char* s, neg = '-', tmp;
			int i = 0, n = 0, mid, size;
			s = new char[MAX_INT_LENGTH];
			if (s == NULL)
			{
				printf("\n Not enough memory in system");
				DEBUG('a', "\n Not enough memory in system");
				machine->WriteRegister(2, -1);
				delete s;
				return;
			}
			// Doc so nguyen can in ra man hinh tu thanh ghi r4
			n = machine->ReadRegister(4);
			if (n < 0)
			{
				gSynchConsole->Write(&neg, 1);
				n = -1 * n;
			}
			do
			{
				s[i] = n % 10 + '0';
				++i;
				n /= 10;
			} while (n > 0);
			size = i;
			mid = size / 2;
			while (mid < i)
			{
				tmp = s[size - i];
				s[size - i] = s[i - 1];
				s[i - 1] = tmp;
				--i;
			}
			gSynchConsole->Write(s, size);
			delete s;
			break;
		}

		case SC_PrintChar:
		{
			char ch;
			//Doc ki tu can duoc in tu thanh ghi r4
			ch = (char)machine->ReadRegister(4);
			gSynchConsole->Write(&ch, 1);
			break;
		}

		case SC_ReadChar:
		{
			char ch;
			int size = gSynchConsole->Read(&ch, 1);
			if (size == -1) //Ctrl+A
				machine->WriteRegister(2, -1);
			else if (size == 0)
				machine->WriteRegister(2, 0);
			else
				machine->WriteRegister(2, int(ch));
			break;
		}

		case SC_ReadString:
		{
			int virtAddr;
			char* buffer;
			int len;
			int sizeBuffer;
			int i;

			DEBUG('a', "\n SC_ReadString call ...");
			DEBUG('a', "\n Reading virtual address of buffer");
			
			//Doc chuoi se luu string doc vao tu thanh ghi r4
			virtAddr = machine->ReadRegister(4);
			//Doc kich thuoc buffer can doc tu thanh ghi r5
			sizeBuffer = machine->ReadRegister(5);

			if (sizeBuffer <= 0) // Kiem tra xem kich thuoc can doc co phai la so duong.
			{
				printf("\n length <= 0");
				DEBUG('a', "\n length <= 0");
				machine->WriteRegister(2, -1); // tra ve loi cho chuong
							       // trinh nguoi dung
				break;
			}
			buffer = new char[sizeBuffer];
			if (buffer == NULL) // Kiem tra xem co cap phat bo nho thanh cong khong?
			{
				printf("\n Not enough memory in system");
				DEBUG('a', "\n Not enough memory in system");
				machine->WriteRegister(2, -1); // tra ve loi cho chuong
						               // trinh nguoi dung
				delete buffer;
				break;
			}
			for (i = 0; i < sizeBuffer; ++i)
			{
				buffer[i] = NULL;
			}

			len = gSynchConsole->Read(buffer, sizeBuffer - 1);	// Doc du lieu tu console
						                                // sang mang buffer
			if (len == -1) // Kiem tra xem co nhap CTRL-A khong?
			{
				machine->WriteRegister(2, -1);
				delete buffer;
				break;
			}
			System2User(virtAddr, len + 1, buffer); // Chuyen du lieu tu bo nho
							        // Kernelspace sang Userspace
										
			machine->WriteRegister(2, len); // Tra ve chieu dai cua chuoi co the doc duoc
			delete buffer;
			break;
		}

		case SC_PrintString:
		{
			int virtAddr;
			char* buffer;
			int i = 0;

			DEBUG('a', "\n SC_PrintString call ...");
			DEBUG('a', "\n Reading virtual address of buffer");

			virtAddr = machine->ReadRegister(4); // Doc dia chi virtual address cua buffer

			DEBUG('a', "\n Reading buffer.");
			buffer = User2System(virtAddr, MAX_STRING_LENGTH);	// Chuyen du lieu tu bo nho
									        // Userspace sang Kernelspace
										
			if (buffer == NULL) // Kiem tra xem cap phat bo nho co thanh cong khong?
			{
				printf("\n Not enough memory in system");
				DEBUG('a', "\n Not enough memory in system");
				machine->WriteRegister(2, -1); // tra ve loi cho chuong
							       // trinh nguoi dung
				delete buffer;
				break;
			}

			// Ghi tung ki tu ra man hinh console
			while (buffer[i] != 0)
			{
				gSynchConsole->Write(&buffer[i], 1);
				++i;
			}

			DEBUG('a', "\n Finish printing string.");
			machine->WriteRegister(2, i); // Tra ve chieu dai cua chuoi can xuat
			delete buffer;
			break;
		}

		case SC_Exec:
		{
			int virtAddr;
			char* filename;

			DEBUG('a', "\n SC_Exec call ...");
			DEBUG('a', "\n Reading virtual address of filename");
			//Doc ten file thuc thi can exec
			virtAddr = machine->ReadRegister(4);

			DEBUG('a', "\n Reading filename.");
			filename = User2System(virtAddr, MaxFileLength + 1);//chuyen file name tu nguoi dung nhap sang he thong xu ly
			if (filename == NULL)//khong du bo nho luu ten file cho he thong xu ly
			{
				printf("\n Not enough memory in system");
				DEBUG('a', "\n Not enough memory in system");
				machine->WriteRegister(2, -1); // tra ve loi cho chuong
							       // trinh nguoi dung
				delete []filename;
				break;
			}
			int error = pTab->ExecUpdate(filename);//Goi execcute tu class PTable
			machine->WriteRegister(2, error);
			break;
		}

		case SC_Join:
		{
			int id;

			DEBUG('a', "\n SC_Join call ...");
			DEBUG('a', "\n Reading ID");
			
			//ghi nhan idFile thuc thi can join tu thanh ghi r4
			id = machine->ReadRegister(4);

			int error = pTab->JoinUpdate(id);
			machine->WriteRegister(2, error);
			break;
		}

		case SC_Exit:
		{
			int status;

			DEBUG('a', "\n SC_Exit call ...");
			DEBUG('a', "\n Reading status");
			
			status = machine->ReadRegister(4);

			int error = pTab->ExitUpdate(status);
			machine->WriteRegister(2, error);
			break;
		}

		case SC_CreateSemaphore:
		{
			int virtAddr;
			int semval;
			char* filename;

			DEBUG('a', "\n SC_CreateSemaphore call ...");
			DEBUG('a', "\n Reading virtual address of filename");
			virtAddr = machine->ReadRegister(4);
			semval = machine->ReadRegister(5);

			DEBUG('a', "\n Reading filename.");
			filename = User2System(virtAddr, MaxFileLength + 1);
			if (filename == NULL)
			{
				printf("\n Not enough memory in system");
				DEBUG('a', "\n Not enough memory in system");
				machine->WriteRegister(2, -1); // tra ve loi cho chuong
							       // trinh nguoi dung
				delete[]filename;
				break;
			}
			int error = semTab->Create(filename, semval);
			machine->WriteRegister(2, error);
			break;
		}

		case SC_Wait:
		{
			int virtAddr;
			char* filename;

			DEBUG('a', "\n SC_Wait call ...");
			DEBUG('a', "\n Reading virtual address of filename");
			virtAddr = machine->ReadRegister(4);

			DEBUG('a', "\n Reading filename.");
			filename = User2System(virtAddr, MaxFileLength + 1);
			if (filename == NULL)
			{
				printf("\n Not enough memory in system");
				DEBUG('a', "\n Not enough memory in system");
				machine->WriteRegister(2, -1); // tra ve loi cho chuong
							       // trinh nguoi dung
				delete[]filename;
				break;
			}
			int error = semTab->Wait(filename);
			machine->WriteRegister(2, error);
			break;
		}

		case SC_Signal:
		{
			int virtAddr;
			char* filename;

			DEBUG('a', "\n SC_Signal call ...");
			DEBUG('a', "\n Reading virtual address of filename");
			virtAddr = machine->ReadRegister(4);

			DEBUG('a', "\n Reading filename.");
			filename = User2System(virtAddr, MaxFileLength + 1);
			if (filename == NULL)
			{
				printf("\n Not enough memory in system");
				DEBUG('a', "\n Not enough memory in system");
				machine->WriteRegister(2, -1); // tra ve loi cho chuong
							       // trinh nguoi dung
				delete[]filename;
				break;
			}
			int error = semTab->Signal(filename);
			machine->WriteRegister(2, error);
			break;		
		}

		default:
		{
			printf("\n Unexpected user mode exception (%d %d)", which, type);
			interrupt->Halt();
		}
		}
		IncreaseProgramCounterRegs();
		break;
	case  PageFaultException:    // No valid translation found.
		DEBUG('a', "No valid translation found.\n");
		printf("No valid translation found.\n");
		interrupt->Halt();
		break;
	case  ReadOnlyException:     // Write attempted to page marked "read-only".
		DEBUG('a', "Write attempted tp page marked \"read - only\".\n");
		printf("Write attempted tp page marked \"read - only\".\n");
		interrupt->Halt();
		break;
	case  BusErrorException:     // Translation resulted in an invalid physical address.
		DEBUG('a', "Translation resulted in an invalid physical address.\n");
		printf("Translation resulted in an invalid physical address.\n");
		interrupt->Halt();
		break;
	case  AddressErrorException: // Unaligned reference or one that was beyond the end of the address space.
		DEBUG('a', "Unaligned reference or one that was beyond the end of the address space.\n");
		printf("Unaligned reference or one that was beyond the end of the address space.\n");
		interrupt->Halt();
		break;
	case  OverflowException:     // Integer overflow in add or sub.
		DEBUG('a', "Integer overflow in add or sub.\n");
		printf("Integer overflow in add or sub.\n");
		interrupt->Halt();
		break;
	case  IllegalInstrException: // Unimplemented or reserved instr.
		DEBUG('a', "Unimplemented or reserved instr.\n");
		printf("Unimplemented or reserved instr.\n");
		interrupt->Halt();
		break;
	}
}
