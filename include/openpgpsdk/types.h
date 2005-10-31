/** \file
 */

#ifndef OPS_TYPES_H
#define OPS_TYPES_H

/** Special type for intermediate function casting, avoids warnings on
    some platforms
*/
typedef void (*ops_void_fptr)(void);
#define ops_fcast(f) ((ops_void_fptr)f)

/** ops_map_t
 */
typedef struct 
    {
    int type;
    char *string;
    } ops_map_t;

/** Boolean type */
typedef unsigned ops_boolean_t;

/** ops_content_tag_t */
typedef enum ops_content_tag_t ops_content_tag_t;

/** 
   keep both ops_content_tag_t and ops_packet_tag_t because we might
   want to introduce some bounds checking i.e. is this really a valid value
   for a packet tag? 
*/
typedef enum ops_content_tag_t ops_packet_tag_t;
/** SS types are a subset of all content types.
 \todo may be want to separate these out?
*/
typedef enum ops_content_tag_t ops_ss_type_t;
/* typedef enum ops_sig_type_t ops_sig_type_t; */

/** Revocation Reason type */
typedef unsigned char ops_ss_rr_code_t;

/** ops_parse_type_t */
typedef enum ops_parse_type_t ops_parse_type_t;

/** ops_parser_content_t */
typedef struct ops_parser_content_t ops_parser_content_t;

/** Reader Flags */
typedef enum
    {
    OPS_RETURN_LENGTH=1,
    } ops_reader_flags_t;
typedef enum ops_reader_ret_t ops_reader_ret_t;

/** Writer flags */
typedef enum
    {
    OPS_WF_DUMMY,
    } ops_writer_flags_t;
/** ops_writer_ret_t */
typedef enum ops_writer_ret_t ops_writer_ret_t;


#endif
