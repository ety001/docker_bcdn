//============================================================
// Base64.h : base64 function declaration
//			(source from INTERNET)
//                          
//============================================================

#ifndef __BASE64_H_20061208_16
#define  __BASE64_H_20061208_16

void to64frombits(uint8_t *out, const uint8_t *in, int inlen);
 int from64tobits(char *out, const char *in);

#endif // #ifndef __BASE64_H_20061208_16

