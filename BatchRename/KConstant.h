#pragma once

#ifndef _K_CONSTANT_VARIABLE_MODULE_H_
#define _K_CONSTANT_VARIABLE_MODULE_H_

#define LF_CHAR '\n'
#define PARTIAL_SYNC_FOLDER_SEPARATOR ';'

#define DA_PARAMETER_SEPARATOR '\b'
#define DA_NAME_SEPARATOR '='
#define DA_VALUE_SEPARATOR '|'
#define DA_SLASH_SEPARATOR '/'
#define DA_RESULT_SEPARATOR ':'
#define DA_COMMA_SEPARATOR ','

extern const char LF_STR[];
extern const char CRLF_STR[];
extern const char ASTERISK_STR[];

extern const TCHAR L_LF_STR[];
extern const TCHAR L_CRLF_STR[];
extern const TCHAR L_ASTERISK_STR[];

extern TCHAR gModulePathName[];

#endif
