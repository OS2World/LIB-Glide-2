/* 
 * THIS SOFTWARE IS SUBJECT TO COPYRIGHT PROTECTION AND IS OFFERED ONLY
 * PURSUANT TO THE 3DFX GLIDE GENERAL PUBLIC LICENSE. THERE IS NO RIGHT
 * TO USE THE GLIDE TRADEMARK WITHOUT PRIOR WRITTEN PERMISSION OF 3DFX
 * INTERACTIVE, INC. A COPY OF THIS LICENSE MAY BE OBTAINED FROM THE 
 * DISTRIBUTOR OR BY CONTACTING 3DFX INTERACTIVE INC(info@3dfx.com). 
 * THIS PROGRAM IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
 * EXPRESSED OR IMPLIED. SEE THE 3DFX GLIDE GENERAL PUBLIC LICENSE FOR A
 * FULL TEXT OF THE NON-WARRANTY PROVISIONS.  
 * 
 * USE, DUPLICATION OR DISCLOSURE BY THE GOVERNMENT IS SUBJECT TO
 * RESTRICTIONS AS SET FORTH IN SUBDIVISION (C)(1)(II) OF THE RIGHTS IN
 * TECHNICAL DATA AND COMPUTER SOFTWARE CLAUSE AT DFARS 252.227-7013,
 * AND/OR IN SIMILAR OR SUCCESSOR CLAUSES IN THE FAR, DOD OR NASA FAR
 * SUPPLEMENT. UNPUBLISHED RIGHTS RESERVED UNDER THE COPYRIGHT LAWS OF
 * THE UNITED STATES.  
 * 
 * COPYRIGHT 3DFX INTERACTIVE, INC. 1999, ALL RIGHTS RESERVED
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3dfx.h>
#include <fxpci.h>

#define null               0
#define SIZE_SST1_NEEDED   0x100000
#define END_ADDRESS        0x10000000
#define S3_SHIFT           0x400000

struct RangeSTRUCT
{
   FxU32                address;
   FxU32                range;
   struct RangeSTRUCT   *next;
   struct RangeSTRUCT   *prev;
};

typedef struct RangeSTRUCT RangeStruct;

void InitRemap(void);
void CloseRemap(void);
void GetMemoryMap(void);
void FindHole(void);
void RemapVoodoo(void);
void pciGetRange(PciRegister reg,FxU32 device_number,FxU32 *data);
FxBool pciGetAddress(PciRegister reg,FxU32 device_number,FxU32 *data);
FxBool fits_in_hole(RangeStruct *begin,FxU32 end);
void SnapToDecentAddress(RangeStruct *address);
void ForceCleanUp(void);
FxBool FindNecessaryCards(void);
void ProcessCommandLine(char **argv,int argc);
FxBool IsCardVoodoo(long i);
FxBool IsCardS3(long i);
FxBool ReadHex(char *string,FxU32 *num);
void AddMapEntry(FxU32 address,FxU32 range,FxBool S3Card);
void HandleMemoryOverlap(void);
FxBool overlap_map(RangeStruct *begin,FxU32 end);

FxBool switch_S3_flag_ignore=FXFALSE;
FxBool switch_force=FXFALSE;
FxBool switch_C0_bias=FXTRUE;
int switch_voodoo_loc = 0;

RangeStruct test_data[6]=  {{0xF0000000,0x100000,0,0},
                           {0xF3000000,0x200000,0,0},
                           {0xE3000000,0x200000,0,0},
                           {0xF5000000,0x200000,0,0},
                           {0xE6000000,0x200000,0,0},
                           {0xD3000000,0x200000,0,0}};
FxBool      test_card_type[6]=   {{FXFALSE},
                                 {FXTRUE},
                                 {FXFALSE},
                                 {FXFALSE},
                                 {FXFALSE},
                                 {FXFALSE}};
RangeStruct map[60];
RangeStruct hole[60];
long        num_holes=0;
RangeStruct *first_entry;
RangeStruct *last_entry;
long        entries=0;
RangeStruct master_hole;
long        voodoo_loc;

void main(int argc,char **argv)
{
   ProcessCommandLine(argv,argc);
   
   InitRemap();

   if (!FindNecessaryCards())
   {
      printf("This program was only meant to be used with the 3dfx Voodoo chipset\n");
      printf("and the S3 968/868 chipset to correct S3's memory addressing bug\n");
      printf("No 968/868 or Voodoo chipset was detected\n");
      ForceCleanUp();
   }
   if (!switch_force)
   {
      GetMemoryMap();
      FindHole();
   }
   RemapVoodoo();
   CloseRemap();
}

void InitRemap(void)
{
   pciOpen();
}

void CloseRemap(void)
{
   pciClose();
}

void GetMemoryMap(void)
{
   FxU32    temp,temp2;
   long     i;

#ifdef TESTING
   for (i=0;i<6;i++)
   {
      temp=test_data[i].address;
      temp2=~(test_data[i].range - 0x1);
      AddMapEntry(temp,temp2,test_card_type[i]);
   }
#else   
   for (i=0;i<32;i++)
   {
      if (pciDeviceExists(i))
      {
         if (pciGetAddress(PCI_BASE_ADDRESS_0,i,&temp))
         {
            pciGetRange(PCI_BASE_ADDRESS_0,i,&temp2);
            AddMapEntry(temp,temp2,IsCardS3(i));
         }
      }
   }
#endif
   HandleMemoryOverlap();
}

void FindHole(void)
{
   RangeStruct    *cur;
   long           i;
   FxU32          max_range;
   long           max_i;

   if (switch_C0_bias)
   {
      if (first_entry->address>0xD000000)
      {
         master_hole.address=0xC000000;
         master_hole.range=SIZE_SST1_NEEDED;
         return;
      }
   }

   /* find biggest hole and use that */
   cur=last_entry;

   while(cur)
   {
      if (cur==last_entry)
      {
         if (fits_in_hole(cur,END_ADDRESS))
         {
            hole[num_holes].address=cur->address+cur->range;
            hole[num_holes].range=SIZE_SST1_NEEDED;
            SnapToDecentAddress(&hole[num_holes]);
            if (fits_in_hole(&hole[num_holes],END_ADDRESS))
            {
               hole[num_holes].range=END_ADDRESS-hole[num_holes].address;
               num_holes++;
            }
         }
      }
      else
      {
         if (fits_in_hole(cur,cur->next->address))
         {
            hole[num_holes].address=cur->address+cur->range;
            hole[num_holes].range=SIZE_SST1_NEEDED;
            SnapToDecentAddress(&hole[num_holes]);
            if (fits_in_hole(&hole[num_holes],cur->next->address))
            {
               hole[num_holes].range=cur->next->address-hole[num_holes].address;
               num_holes++;
            }
         }
      }
      cur=cur->prev;
   }
   
   /* put one underneath everything */
   /* get a bigger hole than necessary */
   if (((num_holes)&&(hole[0].address>0xB000000))||((!num_holes)&&(first_entry->address>0xB000000)))
   {
      hole[num_holes].address=first_entry->address - (SIZE_SST1_NEEDED<<2);
      hole[num_holes].range=SIZE_SST1_NEEDED;
      SnapToDecentAddress(&hole[num_holes]);
   
      if (fits_in_hole(&hole[num_holes],first_entry->address))
      {
         hole[num_holes].range=first_entry->address - hole[num_holes].address;
         num_holes++;
      }
   }

   if (!num_holes)
   {
      printf("No acceptable memory location to map board!!!\n");
      printf("Please see tech support about the fxremap.exe\n");
      ForceCleanUp();
   }

   for (i=0,max_range=0,max_i=0;i<num_holes;i++)
   {
      if (hole[i].range > max_range)
      {
         max_range=hole[i].range;
         max_i=i;
      }
   }

   master_hole=hole[max_i];
   
   if (master_hole.address<0xA000000)
   {
      printf("No acceptable memory location to map board!!!\n");
      printf("Please see tech support about the fxremap.exe\n");
      ForceCleanUp();
   }
}


FxBool fits_in_hole(RangeStruct *begin,FxU32 end)
{
   if ((begin->address+begin->range+SIZE_SST1_NEEDED)<end)
      return FXTRUE;
   return FXFALSE;
}

void RemapVoodoo(void)
{
   FxU32    address;

#if 1
   address=master_hole.address<<4;
   pciSetConfigData(PCI_BASE_ADDRESS_0,voodoo_loc,&address);
#endif
}

void pciGetRange(PciRegister reg,FxU32 device_number,FxU32 *data)
{
   FxU32    temp=0xFFFFFFFF;
   FxU32    size,save;
   
   pciGetConfigData(PCI_BASE_ADDRESS_0,device_number,&save);
   pciSetConfigData(PCI_BASE_ADDRESS_0,device_number,&temp);
   pciGetConfigData(PCI_BASE_ADDRESS_0,device_number,&size);
   pciSetConfigData(PCI_BASE_ADDRESS_0,device_number,&save);

#ifdef TESTING
   printf("PciGetRange: save %08x \n",save);
   printf("PciGetRange: temp %08x \n",temp);
   printf("PciGetRange: size %08x \n",size);
   printf("PciGetRange: save %08x \n",save);
#endif
   *data=size;
}

FxBool pciGetAddress(PciRegister reg,FxU32 device_number,FxU32 *data)
{
   pciGetConfigData(PCI_BASE_ADDRESS_0,device_number,data);
   if (*data & 0x01)
      return FXFALSE;
   return FXTRUE;
}

void AddMapEntry(FxU32 address,FxU32 range,FxBool S3Card)
{
   RangeStruct *temp,*cur,*next;

#if 0
   static long    test_entry=0;

   address=test_data[test_entry].address;
   range=~(test_data[test_entry++].range - 0x1);
#endif
   /* only if address != 0 */
   if(address)
   {
      map[entries].address=address>>4;
      map[entries].range=((~range)>>4)+0x1;

      /* handle S3 memory addressing bug */
      if (S3Card)
      {
         map[entries].address-=S3_SHIFT;
         map[entries].range=S3_SHIFT<<1;
      }

      temp=&map[entries++];
      if (entries<=1)
      {
         first_entry=temp;
         last_entry=temp;
         temp->next=null;
         temp->prev=null;
         return;
      }

      cur=first_entry;
      next=null;
      while(cur)
      {
         if (temp->address < cur->address)
         {
            next=cur;
            break;
         }
         cur=cur->next;
      }
      if (next)
      {
         temp->next=next;
         temp->prev=next->prev;
         next->prev=temp;
         if (next==first_entry)
            first_entry=temp;
         else
            (temp->prev)->next=temp;
      }
      else
      {
         last_entry->next=temp;
         temp->prev=last_entry;
         last_entry=temp;
         temp->next=null;
      }
   }
}

void SnapToDecentAddress(RangeStruct *address)
{
   if (address->address&0x000FFFFF)
   {
      address->address=((address->address)&0xFFF00000)+0x00100000;
   }
}

void ForceCleanUp(void)
{
   pciClose();
   exit(1);
}

FxBool FindNecessaryCards(void)
{
   FxBool voodoo_found=FXFALSE;
   FxBool S3_968_found=FXFALSE;
   long     i, voodoosFound = 0;

   for (i=0;i<32;i++)
   {
      if (pciDeviceExists(i))
      {
         if(IsCardVoodoo(i))
         {
            if (voodoo_found && !switch_force)
            {
               printf("This program is not intended for use with more than one\n");
               printf("3dfx card\n.  Please contact tech support\n");
               ForceCleanUp();
            }
            if (voodoosFound <= switch_voodoo_loc)
            {
                voodoo_found=FXTRUE;
                voodoo_loc=i;
            }
            voodoosFound++;
         }
         else if(IsCardS3(i))
            S3_968_found=FXTRUE;
      }
   }
   if (!voodoo_found)
   {
      printf("Warning no known voodoo card was found\n");
      return FXFALSE;
   }
   if ((!S3_968_found)&&(!switch_S3_flag_ignore))
   {
      printf("Warning S3 968/868 was not found\n");
      return FXFALSE;
   }
   return FXTRUE;
}

void ProcessCommandLine(char **argv,int argc)
{
   long     i;
   FxU32    temp,temp2;
   FxU32    address,range;
   char     *hex_ptr;
   
   for (i=1;i<argc;i++)
   {
      if (strcmp(argv[i],"/dS3")==0)
      {
         switch_S3_flag_ignore=FXTRUE;
      }
      else if(strcmp(argv[i],"/f")==0)
      {
         if ((i+1)<argc)
         {
            if (ReadHex(argv[i+1],&temp))
            {
               switch_force=FXTRUE;
               master_hole.address=temp>>4;
               i++;
            }
            else
            {
               printf("Command line: improper format\n");
               printf("ex: fxremap.exe /f 0xC0000000\n");
               ForceCleanUp();
            }
         }
         else
         {
            printf("Command line: improper format\n");
            printf("ex: fxremap.exe /f 0xC0000000\n");
            ForceCleanUp();
         }
      }
      else if(strcmp(argv[i],"/x")==0)
      {
         if ((i+1)<argc)
         {
            hex_ptr=strchr(argv[i+1],'-');
            if (!hex_ptr)
            {
               printf("Command line: improper format\n");
               printf("ex: fxremap.exe /x 0xE0000000-0xF0000000\n");
               ForceCleanUp();
            }
            if ((ReadHex(argv[i+1],&temp))&&(ReadHex(hex_ptr+1,&temp2)))
            {
               address=temp;
               range=temp2-temp;
               range=~(range - 0x1);
               i++;
               AddMapEntry(address,range,FXFALSE);
            }
            else
            {
               printf("Command line: improper format\n");
               printf("ex: fxremap.exe /x 0xE0000000-0xF0000000\n");
               ForceCleanUp();
            }
         }
         else
         {
            printf("Command line: improper format\n");
            printf("ex: fxremap.exe /x 0xE0000000-0xF0000000\n");
            ForceCleanUp();
         }
      }
      else if (strcmp(argv[i],"/nb")==0)
      {
         switch_C0_bias=FXFALSE;
      }
      else if (strcmp(argv[i],"/i")==0)
      {
         switch_voodoo_loc = atoi(argv[++i]);
      }
      else
      {
         printf("Command line: improper options specified\n");
         printf("Valid options are /dS3 /f /x /i\n");
      }
   }
}

FxBool IsCardVoodoo(long i)
{
   FxU32    vendor,dev_id;
   
   pciGetConfigData(PCI_VENDOR_ID,i,&vendor);
   pciGetConfigData(PCI_DEVICE_ID,i,&dev_id);
   if ((vendor==0x121a)&&(dev_id==0x0001))
      return FXTRUE;
   
   return FXFALSE;
}

FxBool IsCardS3(long i)
{
   FxU32    vendor,dev_id;
   
   pciGetConfigData(PCI_VENDOR_ID,i,&vendor);
   pciGetConfigData(PCI_DEVICE_ID,i,&dev_id);
   if ((vendor==0x5333)&&((dev_id==0x88f0)||(dev_id==0x8880)))
      return FXTRUE;

   return FXFALSE;
}

FxBool ReadHex(char *string,FxU32 *num)
{
   long  i=0;
   FxU32 temp=0,temp2;
   long  num_count=0;

   /* bypass leading spaces */
   while((string[i])&&(string[i]==' '))
      i++;
   /* verify leading 0x */
   if (string[i]=='0')
      i++;
   else
      return FXFALSE;
   if (string[i]=='x')
      i++;
   else
      return FXFALSE;

   /* read in number */
   while(((string[i]>=0x30)&&(string[i]<0x3A))||((string[i]>=0x41)&&(string[i]<0x47))||((string[i]>=0x61)&&(string[i]<0x67)))
   {
      if ((string[i]>=0x30)&&(string[i]<0x3A))
         temp2=string[i] - 0x30;
      else if ((string[i]>=0x41)&&(string[i]<0x47))
         temp2=string[i] - 0x37;
      else if ((string[i]>=0x61)&&(string[i]<0x67))
         temp2=string[i] - 0x57;
      if (num_count!=0)
         temp=(temp<<4)+temp2;
      else if (num_count<8)
         temp=temp2;
      else
         return FXFALSE;
      num_count++;i++;
   }
   *num=temp;
   return FXTRUE;
}

void HandleMemoryOverlap(void)
{
   RangeStruct *cur;

   cur=first_entry;
   while(cur)
   {
      if (cur!=last_entry)
      {
         if (overlap_map(cur,cur->next->address))
         {
            if (cur->range<cur->next->address+cur->next->range-cur->address)
               cur->range=cur->next->address+cur->next->range-cur->address;
            if (cur->next==last_entry)
            {
               last_entry=cur;
               cur->next=null;
            }
            else
            {
               cur->next=cur->next->next;
               cur->next->prev=cur;
            }
         }
         else
            cur=cur->next;
      }
      else
         cur=cur->next;
   }
}

FxBool overlap_map(RangeStruct *begin,FxU32 end)
{
   if (begin->address+begin->range>end)
      return FXTRUE;
   return FXFALSE;
}

