/*
Copyright (c) 2017. The YARA Authors. All Rights Reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdint.h>
#include <stddef.h>

#include <yara.h>

const char* ELF_RULES = "import \"elf\"";

YR_RULES* elf_rules = NULL;

extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
  YR_COMPILER* compiler;

  int result = yr_initialize();

  if (result != ERROR_SUCCESS)
    return 0;

  result = yr_compiler_create(&compiler);

  if (result != ERROR_SUCCESS)
    return 0;

  result = yr_compiler_add_string(compiler, ELF_RULES, NULL);

  if (result != ERROR_SUCCESS)
    return 0;

  result = yr_compiler_get_rules(compiler, &elf_rules);

  if (result != ERROR_SUCCESS)
    return 0;

  yr_compiler_destroy(compiler);

  return 0;
}


int callback(int message, void* message_data, void* user_data)
{
  return CALLBACK_CONTINUE;
}


extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
  if (elf_rules == NULL)
    return 1;

  yr_rules_scan_mem(
      elf_rules,
      (uint8_t*) data, // TODO: remove typecase after changing API
      size,
      SCAN_FLAGS_NO_TRYCATCH,
      callback,
      NULL,
      0);

  return 0;
}