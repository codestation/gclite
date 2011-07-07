/*
 * utils.h
 *
 *  Created on: 06/07/2011
 *      Author: code
 */

#ifndef UTILS_H_
#define UTILS_H_

void *redir2stub(u32 address, u32 stub, void *redir);
void *redir_call(u32 address, void *func);

void gc_utf8_to_unicode(wchar_t *dest, char *src);
void fix_text_padding(wchar_t *fake, wchar_t *real, wchar_t first, wchar_t last);

#endif /* UTILS_H_ */
