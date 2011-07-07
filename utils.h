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

#endif /* UTILS_H_ */
