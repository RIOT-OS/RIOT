#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strvec.h"

int main(int argc, char* argv[]) {
  strvec* groups;
  strvec* target;
  char*   runner;
  FILE*   file;
  char    path[256];
  char    line[1024];
  int     i;
  int     decl_pos;
  int     reg_pos;

  groups = strvec_new(4);
  target   = strvec_new(40);
  runner    = 0;

  for ( i = 1; i < argc; ++i ) {
    char* arg = argv[i];
    if ( runner ) {
      strvec_push_back(groups,arg);
    } else {
      runner = arg;
    }
  }

  if ( !runner ) {
    return 0;
  }

  strcpy(path, runner);
  strcat(path, ".c");
  file = fopen(path, "rt");
  if ( !file ) {
    fprintf(stderr, "can't open %s\n", path);
    return 1;
  }

  for ( i = 0; fgets(line, 1023, file); ++i ) {
    strvec_push_back(target, line);
    if ( strstr(line,"embunit:extern=-") ) decl_pos = i;
    if ( strstr(line,"embunit:run=-" ) ) reg_pos  = i;
  }

  fclose(file);

  for ( i = 0; i < strvec_size(groups); ++i ) {
    const char* name = strvec_get(groups,i);
    sprintf(line,"		TestRunner_runTest(%s_tests());\n", name);
    strvec_insert_before(target, reg_pos++, line);
  }

  for ( i = 0; i < strvec_size(groups); ++i ) {
    const char* name = strvec_get(groups,i);
    sprintf(line,"extern TestRef %s_tests(void);\n", name);
    strvec_insert_before(target, decl_pos++, line);
  }

  file = fopen(path, "wt");
  if ( !file ) {
    fprintf(stderr, "can't open %s\n", path);
    return 1;
  }

  for ( i = 0; i < strvec_size(target); ++i ) {
    fprintf(file,"%s", strvec_get(target,i));
  }
  fclose(file);

  strvec_del(target);
  strvec_del(groups);

  return 0;

}
