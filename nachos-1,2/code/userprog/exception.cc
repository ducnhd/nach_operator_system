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

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);
    
    switch (which)
    {
	case NoException:
	 return;
	case PageFaultException:
	 printf("No valid translation found %d %d\n", which, type);
         ASSERT(FALSE);
         break;
	case  ReadOnlyException:     // Write attempted to page marked 
		printf("Write attempted tp page marked %d %d\n", which, type);
		ASSERT(FALSE);
		break;
	case  BusErrorException:     // Translation resulted in an 
		printf("Translaion resulted in an %d %d\n", which, type);
		ASSERT(FALSE);
		break;
	case  AddressErrorException: // Unaligned reference or one that
		printf("Unaligned reference or one that %d %d\n", which, type);
		ASSERT(FALSE);
		break;
	case  OverflowException:     // Integer overflow in add or sub.
		printf("Integer overflow in add or sub %d %d\n", which, type);
		ASSERT(FALSE);
		break;
	case  IllegalInstrException: // Unimplemented or reserved instr.
		printf("Unimplemented or reserved instr %d %d\n", which, type);
		ASSERT(FALSE);
		break;
	case SyscallException:
	 switch (type)
	 {
		case SC_Halt: //Truong hop halt, goi interrupt->Halt
		{
			DEBUG('a', "Shutdown, initiated by user program.\n");
                        interrupt->Halt();
                        break;
		}		
	 	case SC_Sub: //Truong hop tru hai so nguyen
		{
			int op1 = machine->ReadRegister(4); //doc bien 1, thanh ghi r4
			int op2 = machine->ReadRegister(5); //doc bien 2, thanh ghi r5
			int result = op1 - op2; 
			machine->WriteRegister (2, result); //tra ket qua ve thanh ghi r2
			break; 
		}
		case SC_Add:
		{
			int op1 = machine->ReadRegister(4); //doc bien 1, thanh ghi r4
			int op2 = machine->ReadRegister(5); //doc bien 2, thanh ghi r5
			int result = op1 + op2;
			machine->WriteRegister (2, result); //tra ket qua ve thanh ghi r2
			break;
		}
		//ca'c ha`m goi gSynchConsole de^`u da~ co' ha`m WLineBlock->P(), RLineBlock->P() de^* du*`ng ma`n hi`nh console
		//thu*c hie^n ca'c ta'c vu
		case SC_ReadInt:
		{
				
		        DEBUG('a', "Read integer number from console.\n");
			int number = 0;
			int nDigit = 0;
			int i;
			char* buffer = new char[MAX_INT_LENGTH];
			nDigit = gSynchConsole->Read(buffer, MAX_INT_LENGTH); //so^' chu*~ so^' cu*a kie^*u doc na`y la` 9 so^'
									      // MAX doc duoc la 999,999,999
									      // MIN doc duoc la -99,999,999
			i = buffer[0] == '-' ? 1:0 ; //buffer[0] de^* quy dinh da^'u
			for (; i < nDigit; ++i)
			{
				number = number*10 + (int) (buffer[i] & MASK_GET_NUM);//Mask_get_num: 1111 (0xF), do 1 char la 1 											      //byte
			}
			number = buffer[0] == '-' ? -1*number : number; 
			machine->WriteRegister(2, number);
			delete buffer;
			break;
		}
		case SC_PrintInt:
		{
			char s[MAX_INT_LENGTH], neg, tmp;
			neg = '-'; //dau am
			int i, n, mid, sz;
			i = n = 0;
			DEBUG('a', "Read argument value at r4");
			n = machine->ReadRegister(4); //doc gia tri thanh ghi r4
			if (n < 0)
			{
				gSynchConsole->Write(&neg,1);
				n = -n;
			}
			do 
                        {
			        s[i++] = n%10 + '0';
			}while (( n /= 10) > 0); //chuyen kieu int thanh chuo^~i
			sz = i;
			s[sz] = '\n'; //the^m ki' tu* xuo^'ng do`ng
			mid = i / 2; //ga'n gia' tri mid 
			while ((i--) > mid) //du`ng de^* da*o lai 2 pha^`n thanh ghi quy dinh pha^`n tha^'p, pha^`n cao
			{
				tmp = s[sz-i-1]; 
				s[sz-i-1] = s[i];
				s[i] = tmp;
			}
			gSynchConsole->Write(s, sz);
			break;
		}
		case SC_ReadChar:
		{
			int sz;
			char buf[MAX_INT_LENGTH]; //goi ra ma*ng de^* doc chuo^~i, cho phe'p nguo*`i du`ng nha^p chuo^~i 
						  //to^'i da 9 ki' tu* 
			sz = gSynchConsole->Read(buf, MAX_INT_LENGTH);
			machine->WriteRegister(2, buf[sz-1]);//chi* ghi nha^n ki' tu* thu*' 9 ma` nguo*`i du`ng nha^p
			break;
		}
		case SC_PrintChar:
		{
			char ch;
			ch = (char) machine->ReadRegister(4); //doc tu*` thanh ghi r4
			gSynchConsole->Write(&ch, 1); 
			break;
		}
		case SC_ReadString:
		{
			char *buf = new char[LIMIT]; //LIMIT = 255 ki' tu*
			if (buf == 0) break; //kie^*m tra ca^'p pha't
			int bufAddrUser = machine->ReadRegister(4); //doc thanh ghi r4
			int length = machine->ReadRegister(5); //doc thanh ghi r5
			int sz = gSynchConsole->Read(buf, length); //goi ha`m doc chuo^~i de^* bie^'t so^' ky' tu* doc va`o va` 
								   //so^' bytes luu* tru*~ chuo^~i na`y
			machine->System2User(bufAddrUser, sz, buf);//goi ha`m chuye^* ke^'t qua* le^n cho user
			delete[] buf;
			break;
		}
		case SC_PrintString:
		{
			int bufAddr = machine->ReadRegister(4); //doc thanh ghi r4, r4 la` mo^t char*, ta se~ luu* die^*m bat 	 
								//da^`u doc no'
			int i = 0;
			char *buf = new char[LIMIT]; //LIMIT 255
			buf = machine->User2System(bufAddr, LIMIT); //tu*` die^*m bat da^`u ta se~ doc duo*c chuo^~i na`y
			while (buf[i] != 0 && buf[i] != '\n') //ghi chuo^~i va`o thanh ghi
			{
				gSynchConsole->Write(buf+i, 1);
				i++;
			}
			buf[i] = '\n';
			gSynchConsole->Write(buf+i,1);
			delete[] buf;
			break;
		}
		case SC_OpenFileId:
		{
			int bufAddr = machine->ReadRegister(4); // doc thanh ghi r4, te^n file
			int type = machine->ReadRegister(5); //doc thanh ghi r5, kieu file (1, 2, 3, 4)
			char *buf = new char[LIMIT];
			if (fileSystem->index > 10) //Do gioi han 10 file nen qua 10 se bao loi
			{
				machine->WriteRegister(2, -1);
				delete[] buf;
				break;
			}
			buf = machine->User2System(bufAddr, LIMIT); //file name
			if (strcmp(buf,"stdin") == 0) //kiem tra xem no co phai dang stdin khong
			{
				machine->WriteRegister(2, 0);
				break;
			}
			if (strcmp(buf,"stdout") == 0) //kiem tra xem no co phai dang stout khong
			{
				machine->WriteRegister(2, 1);
				break;
			}
			//Tien hanh mo file do 
			if ((fileSystem->openf[fileSystem->index] = fileSystem->Open(buf, type)) != NULL)
			{
				DEBUG('f',"open file successfully");
				machine->WriteRegister(2, fileSystem->index-1);
			} 
			else 
			{
				DEBUG('f',"can not open file");
				machine->WriteRegister(2, -1);
			};
			delete [] buf;
			break;
		}
		case SC_CloseFile:
		{
			int m_index = machine->ReadRegister(4); //Doc thanh ghi r4, Id cu*a file ca^`n do'ng
			if (fileSystem->openf[m_index] == NULL) break;
			delete fileSystem->openf[m_index];
			fileSystem->openf[m_index] = NULL;
			break;
		}
		case SC_CreateFile:
		{
			int bufAddr = machine->ReadRegister(4); //doc thanh ghi r4, te^n file muo^'n tao, chi* luu* con tro*
			char *buf = new char[LIMIT];//LIMIT 255
			buf = machine->User2System(bufAddr, LIMIT);//tao vu`ng de luu* te^n file
			// Tao file 
			if (fileSystem->Create(buf, 0) == false) //goi ha`m tao file cu*a class trong fileSystem.h
			{
				DEBUG('f',"can not create file");
				machine->WriteRegister(2, -1);
			} 
			else 
			{
				DEBUG('f',"create file successfully");
				machine->WriteRegister(2, 0);
			};
			delete [] buf;
			break;
		}
		//bie^'n openf la` OpenFile** duo*c dinh nghi~a trong filesys.h
		case SC_ReadFile:
		{
			int bufAddr = machine->ReadRegister(4); //con tro* te^n file
			int NumBuf = machine->ReadRegister(5); //so^' ki' tu*
			int m_index = machine->ReadRegister(6);	//file ID
			int OldPos;//vi tri' cu~
			int NewPos;//vi tri' mo*'i
			char *buf = new char[NumBuf];
			int i = 0;
			// Check m_index, do quy dinh trong file filesys.cc la` #define NumDirEntries 10, ne^n ta chi* kie^'m 
			// tu*` 0 -> 10
			if (m_index < 0 || m_index > 10)
			{
				machine->WriteRegister(2, -1);//tha^'t bai
				delete[] buf;
				break;
			}
			// check openf[m_index]
			if (fileSystem->openf[m_index] == NULL)
			{
				machine->WriteRegister(2, -1);//tha^'t bai
				delete[] buf;
				break;
			}
			OldPos = fileSystem->openf[m_index]->GetCurrentPos(); //xa'c dinh vi tri' hie^n tai cu*a file
			buf = machine->User2System(bufAddr, NumBuf); //Ten file
			if (fileSystem->openf[m_index]->type == 2) //quy dinh type trong openFile.h 
								   // type 0: only read
			                                           // type 1 : read and write
			                                           // type 2 : stdout
			                                           // type 3 : stdin
			{
				int sz = gSynchConsole->Read(buf, NumBuf); //doc file
				machine->System2User(bufAddr, sz, buf);
				machine->WriteRegister(2, sz);
				break;
			}
					
			if ((fileSystem->openf[m_index]->Read(buf, NumBuf) ) > 0)
			{
				//Chuyen data tu*` System ra User
			        NewPos = fileSystem->openf[m_index]->GetCurrentPos(); //xa'c dinh vi tri' cuo^'i cu*a file
				machine->System2User(bufAddr, NewPos - OldPos +1, buf);
				machine->WriteRegister(2, NewPos - OldPos + 1);
			}
			else
			{
				machine->WriteRegister(2, -1);//tha^'t bai
				delete[] buf;
				break;
			}
			
			delete[] buf;
			break;
		}
		case SC_WriteFile:
		{
			int bufAddr = machine->ReadRegister(4); //con tro* te^n file
			int NumBuf = machine->ReadRegister(5);  //so^' ki' tu*
			int m_index =  machine->ReadRegister(6);//file ID
			int OldPos;
			int NewPos;
			char *buf = new char[NumBuf];
			
			// Check m_index, do quy dinh trong file filesys.cc la` #define NumDirEntries 10, ne^n ta chi* kie^'m 
			// tu*` 0 -> 10
			if (m_index < 0 || m_index > 10)
			{
				machine->WriteRegister(2, -1);
				delete[] buf;
				break;
			}
			// check openf[m_index]
			if (fileSystem->openf[m_index] == NULL)
			{
				machine->WriteRegister(2, -1);
				delete[] buf;
				break;
			}
			OldPos = fileSystem->openf[m_index]->GetCurrentPos();
					
			buf = machine->User2System(bufAddr, NumBuf);
			if (fileSystem->openf[m_index]->type  == 0 || fileSystem->openf[m_index]->type == 3)
								   //quy dinh type trong openFile.h 
								   // type 0: only read
			                                           // type 1 : read and write
			                                           // type 2 : stdout
			                                           // type 3 : stdin
			{	
				if ((fileSystem->openf[m_index]->Write(buf, NumBuf)) > 0) 
				{
					//Chuyen data tu*` System ra User
					printf("%s",buf); //in te^n file
					NewPos = fileSystem->openf[m_index]->GetCurrentPos(); //xa'c dinh vi tri' cuo^'i cu*a file
					machine->WriteRegister(2, NewPos - OldPos + 1);
				}
				else if (fileSystem->openf[m_index]->type == 1)
				{
					machine->WriteRegister(2, -1);
					delete[] buf;
					break;
				}
			}
			// Ghi file va`o console neu type cua file la 3
			if (fileSystem->openf[m_index]->type == 3)
			{
				int i = 0;
				while (buf[i] != 0 && buf[i] != '\n')
				{
					gSynchConsole->Write(buf+i, 1);
					i++;
				}
				buf[i] = '\n';
				gSynchConsole->Write(buf+i,1);
				machine->WriteRegister(2, i-1);
			}
			delete[] buf;
			break;
		}
		case SC_SeekFile: //ti`m vi tri trong file
		{
			int pos = machine->ReadRegister(4);
			int m_index = machine->ReadRegister(5);
			
			// Check m_index, do quy dinh trong file filesys.cc la` #define NumDirEntries 10, ne^n ta chi* kie^'m 
			// tu*` 0 -> 10
			if (m_index < 0 || m_index > 10) 
			{
				machine->WriteRegister(2, -1);
				break;
			}
			// check openf[m_index]
			if (fileSystem->openf[m_index] == NULL)
			{
				printf("Kho^ng the^* ti`m duoc \n");
				machine->WriteRegister(2, -1);
				break;
			}
			pos = (pos == -1) ? fileSystem->openf[m_index]->Length() : pos;
			if (pos > fileSystem->openf[m_index]->Length() || pos < 0) 
			{
				machine->WriteRegister(2, -1);//kho^ng nam trong pham vi do dai cua file
			} 
			else 
			{
				fileSystem->openf[m_index]->Seek(pos); //di chuye^*n vi tri con tro* trong file
				machine->WriteRegister(2, pos);
			}
			break;
		}
	 }

	 machine->registers[PrevPCReg] = machine->registers[PCReg];
	 machine->registers[PCReg] = machine->registers[NextPCReg];
	 machine->registers[NextPCReg] += 4;  
	 break;
    }
}
