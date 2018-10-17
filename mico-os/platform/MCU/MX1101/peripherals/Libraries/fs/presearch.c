/////////////////////////////////////////////////////////////////////////
//                  Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc.,ShangHai,China
//                      All rights  reserved
//  Filename:presearch.c
/////////////////////////////////////////////////////////////////////////
//#include "app_config.h"
#include "dev_detect_driver.h"
#include "fs_config.h"
#include "folder.h"
#include "fat_file.h"
#include "fsinfo.h"
#include "dir.h"
#include "fsmount_buffermanager.h"
#include "fs_mount.h"
#include "fs_sync.h"
#include "fs_filesort.h"
#include "partition.h"
#include "breakpoint.h"

#define INDEPEND_FREERTOS

#ifndef INDEPEND_FREERTOS
#include "os.h"
#endif
#define FUNC_SPI_UPDATE_EN

#ifdef FUNC_SPI_UPDATE_EN
uint8_t UpgradeFileFound = 0xFF;
#endif// FUNC_SPI_UPDATE_EN

static bool fs_inited_flag = 0;
#ifndef INDEPEND_FREERTOS
SEMAPHORE	Sync;	// Identifier of sync object
#endif

uint8_t FsSyncInit(void* FsContext)
{
#ifndef INDEPEND_FREERTOS
	if(FsContext)
	{
		SEMA_INIT(Sync, 1)
		return 1;
	}
#endif
	return 0;
}

void FsLock(void* FsContext)
{
#ifndef INDEPEND_FREERTOS
	if(FsContext)
	{
		OSDOWN(Sync);
	}
#endif
}

void FsUnLock(void* FsContext)
{
#ifndef INDEPEND_FREERTOS
	if(FsContext)
	{
		OSUP(Sync);
	}
#endif
}
////////////////////////////////////////////////////////////////
//
//Ѱ�����ض���չ�����ļ�
//
void FindSpecialFileCallBack(FS_CONTEXT* FsContext)
{
#ifdef FUNC_SPI_UPDATE_EN   //ע�������ļ���أ����
	//����Ƿ�ΪSPI FLASH�����ļ���*.MVA
	//ֻ����Ŀ¼
	if((FsContext->gFsInfo.FolderSum == 1) && (UpgradeFileFound != 1)) //ֻ������Ŀ¼�Ƿ��������ļ���ע�������ȫ������
	{
		if(memcmp(FsContext->gCurrentEntry->ExtName, "MVA", 3) == 0)
		{
			UpgradeFileFound = 1;
		}
	}

#endif

//����Ϊdemo,�ɸġ�������ÿ���ļ�����Callһ��
#if 0 //demo
	// ������ȡ�ص��ļ�Handle ���趨��ȫ�ֱ����磺FAT_FILE SpecialFile��
	if(FsContext->gFsInfo.FolderSum == 1) //ֻ������Ŀ¼�Ƿ����ض���չ�����ļ���ע�������ȫ������
	{
		if(memcmp(FsContext->gCurrentEntry->ExtName, "XYZ", 3) == 0)
		{
			FileGetInfo(&SpecialFile); //�ҵ��ض��ļ�����ȡ����SpecialFile��Ϣ��
		}
	}
#endif

}
////////////////////////////////////////////////////////////////


//�ж��ļ������ͣ����ļ�ϵͳ�ײ����
uint8_t FileGetType(FS_CONTEXT* FsContext)
{
	uint8_t ExtFileName[3];

	ExtFileName[0] = FsContext->gCurrentEntry->ExtName[0];
	ExtFileName[1] = FsContext->gCurrentEntry->ExtName[1];
	ExtFileName[2] = FsContext->gCurrentEntry->ExtName[2];


	if((ExtFileName[0] == 'M') && (ExtFileName[1] == 'P') && (ExtFileName[2] == '3' || ExtFileName[2] == '2'))
	{
		return FILE_TYPE_MP3;
	}
	else if((ExtFileName[0] == 'W') && (ExtFileName[1] == 'M') && (ExtFileName[2] == 'A'))
	{
		return FILE_TYPE_WMA;
	}
	else if((ExtFileName[0] == 'A') && (ExtFileName[1] == 'S') && (ExtFileName[2] == 'F'))
	{
		return FILE_TYPE_WMA;
	}
	else if((ExtFileName[0] == 'W') && (ExtFileName[1] == 'M') && (ExtFileName[2] == 'V'))
	{
		return FILE_TYPE_WMA;
	}
	else if((ExtFileName[0] == 'A') && (ExtFileName[1] == 'S') && (ExtFileName[2] == 'X'))
	{
		return FILE_TYPE_WMA;
	}
	else if((ExtFileName[0] == 'W') && (ExtFileName[1] == 'A') && (ExtFileName[2] == 'V'))
	{
		return FILE_TYPE_WAV;
	}
	else if((ExtFileName[0] == 'S') && (ExtFileName[1] == 'B') && (ExtFileName[2] == 'C'))
	{
		return FILE_TYPE_SBC;
	}
	else if((ExtFileName[0] == 'F') && (ExtFileName[1] == 'L') && (ExtFileName[2] == 'A'))
	{
		return FILE_TYPE_FLAC;
	}
	else if(((ExtFileName[0] == 'A') && (ExtFileName[1] == 'A') && ExtFileName[2] == 'C') 
		|| ((ExtFileName[0] == 'M') && (ExtFileName[1] == 'P') && ExtFileName[2] == '4') 
		|| ((ExtFileName[0] == 'M') && (ExtFileName[1] == '4') && ExtFileName[2] == 'A'))
	{
		return FILE_TYPE_AAC;
	}
	else if((ExtFileName[0] == 'A') && (ExtFileName[1] == 'I') && (ExtFileName[2] == 'F'))
    {
        return FILE_TYPE_AIF;
    }
	else
	{
		return FILE_TYPE_UNKNOWN;
	}
}

// ��ȡFSɨ����벥��ģʽ�ϵ���Ϣ��ƥ����ļ����ļ���ID��
#ifdef FUNC_MATCH_PLAYER_BP

static PLAYER_BP_RET sPlayerBPRet = {-1, 1, 1};

// ��ȡ���Ŷϵ�ƥ����Ϣ��Ӧ����FS��ʼ���ɹ������
PLAYER_BP_RET* GetMatchPlayerBPRet(void)
{
	return &sPlayerBPRet;
}

void SetMatchPlayerBPRet(int8_t BPIndex, uint16_t FileId, uint16_t FolderId)
{
    sPlayerBPRet.BPIndex  = BPIndex;
    sPlayerBPRet.FileId   = FileId;
    sPlayerBPRet.FolderId = FolderId;
}

#endif
//
//ͳ���ļ����ļ�������
//
bool PreSearch(FS_CONTEXT* FsContext)
{
	ENTRY_TYPE EntryType;

#ifdef FUNC_MATCH_PLAYER_BP
	sPlayerBPRet.BPIndex  = -1;
#endif

	mx1101_fs_log("PreSearch start!\n");
#ifdef FUNC_SPI_UPDATE_EN
	UpgradeFileFound = 0;
#endif
	DirSetStartEntry(FsContext, FsContext->gFsInfo.RootStart, 0, TRUE);
	FSINFO_Init(FsContext);

	FsContext->gFolderDirStart = FsContext->gFsInfo.RootStart;

	while(1)
	{

		EntryType = (ENTRY_TYPE)DirGetNextEntry(FsContext);

		//mx1101_fs_log("fcount:%d\n",++fcount);

		if(FsContext->gEntryOffset == 0)
		{
			FSINFO_SaveFdi(FsContext);
		}

		switch(EntryType)
		{
			case ENTRY_FILE: 	//�Ǹ���

				//mx1101_fs_log("fcount:%d\n",++fcount);
				if(FileGetType(FsContext) != FILE_TYPE_UNKNOWN)
				{
					//mx1101_fs_log("�ҵ�������%-.8s%-.3s\n", &FsContext->gCurrentEntry->FileName[0], &FsContext->gCurrentEntry->FileName[8]);
					if(FsContext->gFsInfo.FileSum >= 65535)
					{
						return TRUE;
					}
					FsContext->gFsInfo.FileSum++;
					FsContext->gFsInfo.FileSumInFolder++;
					//mx1101_fs_log("Find a audio file:%d!\n", fcount);
#if FS_FILE_SORT_EN == 1
					FileSortSearchNewFile(FsContext);
#endif
				}

				FindSpecialFileCallBack(FsContext);

				break;

			case ENTRY_FOLDER:	//���ļ��У��´�ѭ�������������ļ�����
				FsContext->gFsInfo.FolderNumInFolder++;
				//mx1101_fs_log("ENTRY_FOLDER: %d\n", FsContext->gFsInfo.FolderNumInFolder);
				break;

			case ENTRY_END:		//������һ���ļ��У�����֮ǰ��¼�Ĳ��ֽڵ�
				FSINFO_ChangeFileNum(FsContext);
				//��¼���ļ����е��ļ�����
				if(FsContext->gFsInfo.FolderSum == 1)
				{
					FsContext->gFsInfo.RootFileSum = FsContext->gFsInfo.FileSum;
				}

				//�ص���ǰ�ļ��еĿ�ͷλ��
				//mx1101_fs_log("��ʼ������һ���ļ��е�λ�ã�\n");
				RewindFolderStart(FsContext);

				//�ӵ�ǰλ�ã��������ļ�ϵͳ��Ѱ����һ���ļ���
				if(!FindNextFolder(FsContext))
				{
					//mx1101_fs_log("PreSearch end! %d ms \n", (uint16_t)PastTimeGet(&Timer));
#if FS_FILE_SORT_EN == 1
					FileSortSearchEnd(FsContext);
#endif
					return TRUE;
				}
				else	//������һ���ļ��гɹ�
				{
					//mx1101_fs_log("Into Next Folder%8s\n", &FsContext->gCurrentEntry->FileName[0]);
					if(FsContext->gFsInfo.FolderSum >= 65535)
					{
#if FS_FILE_SORT_EN == 1
						FileSortSearchEnd(FsContext);
#endif
						return TRUE;
					}
					FsContext->gFsInfo.FolderSum++;
					FsContext->gFsInfo.FolderNumInFolder = 0;
					FsContext->gFsInfo.FileSumInFolder = 0;
					if(!DirEnterSonFolder(FsContext))
					{
						//mx1101_fs_log("�������ļ���ʧ�ܣ�\n");
						FsContext->gFsInfo.FolderSum--;
					}
					//mx1101_fs_log("�������ļ��гɹ���\n");
				}
				break;

			default:
				break;
		}
	}
}

//
//�ļ�ϵͳ��ʼ��������������Ԥ����ͳ���ļ�ϵͳ�й���Ϣ.
//
bool FSInit(uint8_t DeviceID)
{

	//get fs memory
	FS_CONTEXT* Fs_Context;

	Fs_Context = gFSContextBufferManager.pfnAllocFSContextBuffer(&gFSContextBufferManager.FSContextBuffer, DeviceID);

	if(Fs_Context == NULL)
	{
		return FALSE;
	}
		
	ENTER_FAT(Fs_Context);

	if(!CheckAllDiskLinkFlag())
	{
		mx1101_fs_log("disk not ready!\n");
		LEAVE_FAT(Fs_Context, FALSE);
	}

	if(Fs_Context->gPreSearchStatus != PRESEARCH_STATUS_NONE)
	{
		mx1101_fs_log("Mount Already\n");
		LEAVE_FAT(Fs_Context, TRUE);
	}

	Fs_Context->gPreSearchStatus = PRESEARCH_STATUS_SEARCHING;

	if(Fs_Context == 0 || FsGetAvailableLogicId() < 0)
	{
		LEAVE_FAT(Fs_Context, FALSE);
	}

	//mount fs
	FsMount(FsGetAvailableLogicId(), Fs_Context);

	FsGetCurContext()->gFsInfo.DevID = (DEV_ID)DeviceID;
	FsGetCurContext()->gFsInfo.CurrFATSecNum = -1;
	//FsGetCurContext()->gFsInfo.CurrDirSecNum = -1;
	FsGetCurContext()->gCurFreeClusNum = 2;


	if(!PartitionLoad(FsGetCurContext(), 0))
	{
		mx1101_fs_log("PartitionLoad() error!\n");
		//return FALSE;
		FsMount(FsGetLogicIdByDeviceType(DeviceID), 0);
		gFSContextBufferManager.pfnFreeFSContextBuffer(&gFSContextBufferManager.FSContextBuffer, DeviceID);
		Fs_Context->gPreSearchStatus = PRESEARCH_STATUS_NONE;
		LEAVE_FAT(Fs_Context, FALSE);
	}
	mx1101_fs_log("PartitionLoad() OK!\n");

	{
		if(!PreSearch(FsGetCurContext()))
		{
			mx1101_fs_log("PreSearch() error!\n");
			//return FALSE;
			LEAVE_FAT(Fs_Context, FALSE);
		}
		//end = OSSysTickGet();
		//mx1101_fs_log("PreSearch() End:%d\n", end - begin);
	}

	Fs_Context->gPreSearchStatus = PRESEARCH_STATUS_SEARCHED;

	mx1101_fs_log("PreSearch() OK\n");


	mx1101_fs_log("Song sum in disk: %d\n", FsGetCurContext()->gFsInfo.FileSum);
	mx1101_fs_log("Folder sum in disk: %d\n", FsGetCurContext()->gFsInfo.FolderSum);
	mx1101_fs_log("Valid folder sum in disk: %d\n", FsGetCurContext()->gFsInfo.ValidFolderSum);
	mx1101_fs_log("***********************************************\n");
	mx1101_fs_log("\n");

	//return TRUE;
	LEAVE_FAT(Fs_Context, TRUE);

//	fs_inited_flag = TRUE;
}

//
//�ļ�ϵͳȥ��ʼ��
//
bool FSDeInit(uint8_t DeviceID)
{
	bool res = FALSE;

	int8_t LogicId = FsGetLogicIdByDeviceType(DeviceID);

	FS_CONTEXT* Fs_Context = FsGetContextByDeviceType(DeviceID);

#ifdef FUNC_MATCH_PLAYER_BP
//    sPlayerBPRet.BPIndex  = -1;
#endif

	ENTER_FAT(Fs_Context);
	if(LogicId >= 0)
	{
		FsMount(LogicId, 0);

		res = gFSContextBufferManager.pfnFreeFSContextBuffer(&gFSContextBufferManager.FSContextBuffer, DeviceID);
	}
	//return res;
	LEAVE_FAT(Fs_Context, res);

//	fs_inited_flag = FALSE;
}

bool IsFsInited(void)
{
	return fs_inited_flag;
}
