/*
	SWETUL PATEL
	FILE SYSTEM READER

*/

#include "fat32.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>

int command;
FILE *myFile;
int clusValid = 0;

uint32_t readFileAllocTable(uint32_t clusNum, fat32BS image_disk, FILE *startLoc, uint32_t FDS)
{

    uint32_t FOffset = ((clusNum) * 4);
    //printf("FOffset: %u \n", FOffset);
    //printf("RESERV %u", image_disk.BPB_RsvdSecCnt*image_disk.BPB_BytesPerSec);
    uint32_t FATSecNum = (image_disk.BPB_RsvdSecCnt * image_disk.BPB_BytesPerSec) + FOffset ;
    //printf("FSN: %u \n", FATSecNum);
    //uint32_t FATEntOffset = FOffset % image_disk.BPB_BytesPerSec;

    uint32_t readFAT;
    uint32_t jump = FATSecNum;
    //printf("THIS IS BYTE OFFFSET %u \n", FATEntOffset);
    fseek(startLoc, jump, SEEK_SET);
    fread(&readFAT, sizeof(readFAT),1,startLoc);
    uint32_t FAT32ClusEntryVal = readFAT & 0x0FFFFFFF;

    if(FAT32ClusEntryVal >= 0x0FFFFFF8 || FAT32ClusEntryVal == 0x0)
    {
        //printf("END OF CLUSTER! \n");
        return 0;

    }
    else if(FAT32ClusEntryVal == 0x0FFFFFF7)
    {
        return FAT32ClusEntryVal = readFileAllocTable(FAT32ClusEntryVal, image_disk, startLoc,FDS);
    }
    else{
        return FAT32ClusEntryVal;
    }

}

int recursiveDir(FILE * startLoc, uint32_t seekLoc, fat32BS bootInfo, uint32_t FDS, int depth)
{

    uint32_t current = seekLoc;
    fseek(startLoc, ((seekLoc-2  + FDS)*bootInfo.BPB_BytesPerSec), SEEK_SET);
    fat32Dir dirs;
    fat32Dir firstDIR;

    int keepRead = 1;
    uint32_t currentCLUS = seekLoc;
    while(keepRead)
    {
        uint32_t sectors = 0;
        while(sectors < bootInfo.BPB_SecPerClus)
        {
            int dirCount = 0;
            for(int i = 0; i < (bootInfo.BPB_BytesPerSec/32); i++)
            {
                current += 32;
                fread(&dirs, sizeof(struct fat32Dir_struct), 1, startLoc);
                if(i == 0)
                {
                    firstDIR = dirs;
                }

                if(dirs.DIR_Name[0] != 0x00)
                {
                    if (dirs.DIR_Name[0] != 0xE5 && (dirs.DIR_Attr & 2) != 2) {
                        if((dirs.DIR_Attr & 16) == 16)
                        {
                            if(dirCount++ > 2)
                            {
                                for(int z = 0; z < depth; z++)
                                {
                                    printf("-");
                                }
                                printf("Directory name: %s \n", dirs.DIR_Name);

                                FILE* temp = startLoc;
                                //printf("This is a directory \n \n \n");
                                uint32_t seekPos = ((dirs.DIR_FstClusHI + dirs.DIR_FstClusLO));

                                if(recursiveDir(temp, seekPos, bootInfo, FDS, ++depth))

                                fseek(startLoc, current, SEEK_SET);
                            }

                        }else{
                            for(int z = 0; z < depth; z++)
                            {
                                printf("-");
                            }
                            printf("File name: %s \n", dirs.DIR_Name);
                        }

                    }
                }
                else{
                    break;
                }
                printf("attr: %u \n", dirs.DIR_Attr);
                //if this is a directory

                if (dirs.DIR_Name[0] != 0xE5 && (dirs.DIR_Attr & 2) != 2) {
                    if((dirs.DIR_Attr & 16) == 16)
                    {
                        if(dirCount++ > 2)
                        {
                            printf("Directory name: %s \n", dirs.DIR_Name);

                            FILE* temp = startLoc;
                            //printf("This is a directory \n \n \n");
                            uint32_t seekPos = ((dirs.DIR_FstClusHI + dirs.DIR_FstClusLO));

                            if(recursiveDir(temp, seekPos, bootInfo, FDS))

                            fseek(startLoc, current, SEEK_SET);
                        }

                    }else{
                        printf("File name: %s \n", dirs.DIR_Name);
                    }

                }
            }
            sectors++;
        }
            currentCLUS = readFileAllocTable(currentCLUS, bootInfo, startLoc, FDS);

            if (currentCLUS) {
                fseek(startLoc, ((currentCLUS-2 + FDS)*bootInfo.BPB_BytesPerSec), SEEK_SET);
                current = currentCLUS;
            }
            else{
                keepRead = 0;
            }
    }
    return 1;
}



// Function reads a file system image
int readIMAGE(FILE* startLoc, size_t readSizeBytes)
{
    fat32BS image_disk;
    FSInfo disk_info;

    if(fread(&image_disk, readSizeBytes, 1, startLoc))
    {
        //how many root directory sectors
        uint32_t RDS = ((image_disk.BPB_RootEntCnt * 32) + (image_disk.BPB_BytesPerSec - 1)) / image_disk.BPB_BytesPerSec;

        //first directory sector
        uint32_t FDS = image_disk.BPB_RsvdSecCnt + (image_disk.BPB_NumFATs * image_disk.BPB_FATSz32) + RDS;

        //first sector of cluster
        uint32_t FSC = ((image_disk.BPB_RootClus - 2) *image_disk.BPB_SecPerClus) + FDS;


        if(command == 1)        //info command
        {
            //all formulas are derived from the microsoft FAT manual page given on UMLEARN
            fseek(startLoc, readSizeBytes, SEEK_SET);             //seek to get the drive info
            fread(&disk_info, sizeof(struct FSInfo_struct), 1, startLoc);    //get the drive info
            printf("Drive name: %s \n", image_disk.BS_OEMName);
            printf("Free space on drive in KB: %u \n", (image_disk.BPB_SecPerClus * image_disk.BPB_BytesPerSec * disk_info.FSI_Free_Cnt) / 1000);

            //how many usable sectors
            uint32_t usableSec = image_disk.BPB_TotSec32 - ((image_disk.BPB_NumFATs * image_disk.BPB_FATSz32) + image_disk.BPB_RsvdSecCnt + RDS);
            printf("Amount of usable space in Bytes: %u \n", (usableSec*image_disk.BPB_BytesPerSec));

            printf("Size of individual cluster(in sectors: %u) and (in Bytes: %u) \n", image_disk.BPB_SecPerClus, (image_disk.BPB_BytesPerSec * image_disk.BPB_SecPerClus));

            fclose(myFile);
            return EXIT_SUCCESS;
        }// end of info command

        if(command == 2)            //list command
        {
            fseek(startLoc, FSC*image_disk.BPB_BytesPerSec, SEEK_SET);
            //seeked into root directory position
            uint32_t counter = (FSC*image_disk.BPB_BytesPerSec);
            fat32Dir dirs;
            uint32_t sectors = 0;
            int keepRead = 1;
            uint32_t currentCLUS = image_disk.BPB_RootClus;
            while(keepRead)
            {
                while(sectors < image_disk.BPB_SecPerClus)
                {
                    for(int i = 0; i < (image_disk.BPB_BytesPerSec/32); i++)
                    {
                        counter += 32;
                        fread(&dirs, sizeof(struct fat32Dir_struct), 1, startLoc);
                        if ((dirs.DIR_Name[0] != 0x00 || dirs.DIR_Name[0] != 0xE5) && ((dirs.DIR_Attr & 2) != 2 || (dirs.DIR_Attr & 0) != 0)) {

                            //printf("attr: %u \n", dirs.DIR_Attr);
                            //if this is a directory
                            if((dirs.DIR_Attr & 16) == 16)
                            {
                                printf("Directory: %s \n", dirs.DIR_Name);
                                FILE* temp = startLoc;
                                //printf("This is a directory \n \n \n");
                                uint32_t seekPos = (dirs.DIR_FstClusHI + dirs.DIR_FstClusLO);
                                //printf("--GOING TO %u  ------from %u\n", seekPos, counter);
                                int init = 1;
                                if(recursiveDir(temp, seekPos, image_disk, FDS, init))
                                {
                                  //  printf("End of directory! MAIN MAIN MAIN..... \n");
                                }
                                //printf("SEEKING BAK TO %u \n", counter);
                                fseek(startLoc, counter, SEEK_SET);
                            }
                            else
                            {
                                printf("File: %s \n", dirs.DIR_Name);
                            }
                        }
                    }
                    sectors++;
                }
                currentCLUS = readFileAllocTable(currentCLUS, image_disk, startLoc, FDS);
                if (currentCLUS) {
                    fseek(startLoc, currentCLUS*image_disk.BPB_BytesPerSec, SEEK_SET);
                    //printf("NEW CLUSTER!!! %u \n", currentCLUS);
                }
                else{
                    //printf("NO new cluster here!! %u \n", currentCLUS);
                    keepRead = 0;
                }
            }
        }
    }
    return 1;
}


int main(int argc, char** argv)
{
	if(argc < 2|| argc > 4)
	{
		printf("Missing input arguments or too many arguments, Please try again!\n");
		return EXIT_FAILURE;
	}
    if (argc < 3) {
        printf("Missing Command! \n");
        return EXIT_FAILURE;
    }

	myFile = fopen(argv[1], "rb");

    if(!myFile)
	{
		printf("Error opening image/drive! \n");
		return EXIT_FAILURE;
	}




    // check command type
	if(!strcmp(argv[2],"info"))
	{
		command = 1;
        printf("got here \n");
		return readIMAGE(myFile, sizeof(struct fat32BS_struct));
		//one block
	}
	else if(!strcmp(argv[2],"list"))
	{
		command = 2;
        //calculate location of directory first instance.
        return readIMAGE(myFile, 512);

	}
	else if(!strcmp(argv[2],"get"))
	{
		command = 3;
	}
    else{
        printf("Incorrect Command! choose between info, list and get! \n");
        return EXIT_FAILURE;
    }
	return EXIT_SUCCESS;
}
