// PTABLE.h 

#ifndef PTABLE_H
#define PTABLE_H

#include "bitmap.h"
#include "bool.h"
#include "PCB.h"
#include "semaphore.h"
#include "system.h"

#define MAX_PROCESS 10

class PCB;
class PTable
{
private:
	BitMap *bm;
	PCB **pcb;
	int psize;
	Semaphore *bmsem; // Dung de ngan chan truong hop nap 2 tien trinh cung luc

public:
	PTable(int size); // Khoi tao size doi tuong pcb de luu size process. Gan gia tri ban dau la null. Khoi tao *bm va *bmsem de su dung
	~PTable(); // Huy cac doi tuong da tao

	int ExecUpdate(char *name); // Return PID
	int ExitUpdate(int ec);
	int JoinUpdate(int id);
	int GetFreeSlot(); // Tim free slot de luu thong tin cho tien trinh moi
	bool IsExist(int pid); // Kiem tra co ton tai process ID nay khong
	void Remove(int pid); // Xoa mot process ID ra khoi mang quan ly no, khi ma tien trinh nay da ket thuc
	char* GetFileName(int id);
};

#endif // PCB_H
