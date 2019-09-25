
#include "verno.h"

char build_date_time_str[] = "2019/05/07 16:52:55 GMT +08:00";
char sw_verno_str[] = MTK_FW_VERSION;
char hw_verno_str[] = "mt7682_hdk";
char md_commit_id_str[] = "";

#ifdef MTK_SYSLOG_VERSION_2
#include "memory_attribute.h"
ATTR_LOG_TIMESTAMP log_bin_build_date_time_str[] = "2019/05/07 16:52:55 GMT +08:00";
ATTR_LOG_VERSION log_bin_sw_verno_str[] = MTK_FW_VERSION;
#endif

