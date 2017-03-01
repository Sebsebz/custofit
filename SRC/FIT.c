#include "stdio.h"
#include "string.h"
#include <stdint.h>

enum en_bool_enum
{
   FALSE = 0,
   TRUE
} __attribute__ ((aligned (1))) __attribute__ ((packed)); /* Alignment to get boolean on 1 Byte */

/** Base boolean type                     */
typedef enum en_bool_enum  en_bool;

/* Base character type                    */
typedef char               t_char;

/* Base unsigned integer types            */
typedef uint8_t  t_uint8;      /*  8-bits */
typedef uint16_t t_uint16;     /* 16-bits */
typedef uint32_t t_uint32;     /* 32-bits */
#ifndef OS_VXWORKS_5
typedef uint64_t t_uint64;     /* 64-bits */
#else /* OS_VXWORKS_5 */
typedef unsigned long long int t_uint64;     /* 64-bits */
#endif /* OS_VXWORKS_5 */

/* Base signed integer types              */
typedef int8_t   t_int8;       /*  8-bits */
typedef int16_t  t_int16;      /* 16-bits */
typedef int32_t  t_int32;      /* 32-bits */
#ifndef OS_VXWORKS_5
typedef int64_t  t_int64;      /* 64-bits */
#else /* OS_VXWORKS_5 */
typedef long long int t_int64;     /* 64-bits */
#endif /* OS_VXWORKS_5 */

/* Base float types                       */
typedef float    t_float32;    /* 32-bits */
typedef double   t_float64;    /* 64 bits */

/* Base bitfield type.
    Take care: bitfield are implementation dependent
    Structure can be filled from left to right or right to left
    Alignment is machine dependent
*/
typedef uint32_t t_bitfield32; /* 32-bits */

#pragma pack(1)
typedef struct 
{
	t_uint8    header_size;        // 0
	t_uint8    protocol_version;   // 1
	t_uint16   profile_version;    // 2-3
	t_uint32   data_size;          // 4-7
	t_char     data_type[4];       // 8-11
	t_uint16   crc;                // 12-13
} st_fit_header_file_packed;
#pragma pack()
#pragma pack(1)
typedef struct 
{
	t_uint8    reserved;
	t_uint8    architecture;
	t_uint16   global_msg_idx;
	t_uint8    field_nb;
} st_fit_definition_header;
#pragma pack()
#pragma pack(1)
typedef struct 
{
	t_uint8    definition_number;
	t_uint8    size;
	t_uint8    base_type;
} st_fit_field;
#pragma pack()

typedef enum {
	RHM_DATA = 0,
	RHM_DEFINITION
} en_record_header_message;

typedef enum {
	RHM_SPECIFIC_FLAG_OFF = 0,
	RHM_SPECIFIC_FLAG_ON
} en_record_header_specific_flag;

int main(int argc, char* argv[])
{
	int ret = 0;
	FILE *file;
	t_uint8 buffer[64];
	t_char str_type[5];
	st_fit_header_file_packed *p_header_file_packed = NULL;
	t_uint8 header_record;
	t_uint32 record_cpt = 0;
	en_record_header_message record_header_msg;
	st_fit_definition_header *p_definition_header_msg = NULL;
	en_record_header_specific_flag record_header_specific_flag;
	st_fit_field *p_fit_field = NULL;

   if((file = fopen(argv[1], "rb")) == NULL)
   {
      printf("Error opening file %s.\n", argv[1]);
      ret = -1;
   }

   if(ret == 0)
   {
   	  t_int32 indice_buffer = 0;

      memset(buffer,0x00, sizeof(buffer));
      for(indice_buffer = 0 ; indice_buffer < 64 && indice_buffer < sizeof(st_fit_header_file_packed) ; indice_buffer++)
      {
      	buffer[indice_buffer] = (t_uint8) getc(file);
      }

      p_header_file_packed = (st_fit_header_file_packed *) buffer;

      printf("===================== \n");
      printf("Header Size      : %d\n",p_header_file_packed->header_size);
      printf("Protocol Version : %d\n",p_header_file_packed->protocol_version);
      printf("Profile Version  : %d\n",p_header_file_packed->profile_version);
      printf("Date Size        : %u\n",p_header_file_packed->data_size);
      memset(str_type,0x00,5);
      memcpy(str_type,p_header_file_packed->data_type,4);
      printf("Data Type        : %s\n",str_type);
      printf("CRC              : 0x%04X\n",p_header_file_packed->crc);
      printf("==========================\n");

      /* Read Next Header record */
      header_record =  (t_uint8) getc(file);
      printf("Record %ld : 0x%02X\n",++record_cpt,header_record);
      printf("Test 0x%02x => 0x%02X\n",0x40,(header_record & 0x40) );
      printf("Test 0x%02x => 0x%02X\n",0x20,(header_record & 0x20) );
      printf("Test 0x%02x => 0x%02X\n",0x0F,(header_record & 0x0F) );

      if(header_record & 0x40)
      {
      	printf("- Is Definition Record");
      	record_header_msg = RHM_DEFINITION;
      }
      else
      {
      	printf("- Is Data Record");
      	record_header_msg = RHM_DATA;
      }
      record_header_specific_flag = RHM_SPECIFIC_FLAG_OFF;
      if(header_record & 0x20)
      {
      	printf(" with spcific type");
      	record_header_specific_flag = RHM_SPECIFIC_FLAG_ON;
      }

      if(header_record & 0x0F)
      {
      	printf(" : local type %d",header_record & 0x0F);
      }
      printf("\n");
      if(record_header_msg == RHM_DEFINITION)
      {
	      memset(buffer,0x00, sizeof(buffer));
	      for(indice_buffer = 0 ; indice_buffer < 64 && indice_buffer < sizeof(st_fit_definition_header) ; indice_buffer++)
	      {
	      	buffer[indice_buffer] = (t_uint8) getc(file);
	      }
	      p_definition_header_msg = (st_fit_definition_header *) buffer;
	      printf("Record %d : Definition.reserved       = %d\n",record_cpt,p_definition_header_msg->reserved);
	      printf("Record %d : Definition.architecture   = %d\n",record_cpt,p_definition_header_msg->architecture);
	      printf("Record %d : Definition.global_msg_idx = %d\n",record_cpt,p_definition_header_msg->global_msg_idx);
	      printf("Record %d : Definition.field_nb       = %d\n",record_cpt,p_definition_header_msg->field_nb);
	      t_uint32 field_idx;
	      for(field_idx = 0; field_idx < p_definition_header_msg->field_nb; ++field_idx)
	      {
		      t_uint8 buffer_field[64];
		      memset(buffer_field,0x00, sizeof(buffer_field));
		      for(indice_buffer = 0 ; indice_buffer < 64 && indice_buffer < sizeof(st_fit_field) ; indice_buffer++)
		      {
		      	buffer_field[indice_buffer] = (t_uint8) getc(file);
		      }
		      p_fit_field = (st_fit_field *) buffer_field;
     	      printf("Record %d : Field %d : definition_number = 0x%02X\n",record_cpt,field_idx,p_fit_field->definition_number);
     	      printf("Record %d : Field %d : size              = %d\n",record_cpt,field_idx,p_fit_field->size);
     	      printf("Record %d : Field %d : base_type         = 0x%02X\n",record_cpt,field_idx,p_fit_field->base_type);
	      }
      }

      /* Decode Next record */
	  fclose(file);
   }

	return ret;
}