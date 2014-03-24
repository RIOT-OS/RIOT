#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strvec.h"

int main(int argc, char* argv[]) {
  strvec* includes;
  strvec* cases;
  strvec* target;
  char*   group;
  FILE*   file;
  char    path[256];
  char    line[1024];
  int     i;
  int     incl_pos;
  int     impl_pos;
  int     suite_pos;
  int     in_incl;
  int     in_suite;

  includes     = strvec_new(4);
  cases        = strvec_new(4);
  target       = strvec_new(40);
  group    = 0;

  for ( i = 1; i < argc; ++i ) {
    char* arg = argv[i];
    if ( strchr(arg,'.') ) {
      strvec_push_back(includes, arg);
    } else if ( group ) {
      strvec_push_back(cases,arg);
    } else {
      group = arg;
    }
  }

  if ( !group ) {
    return 0;
  }

  strcpy(path, group);
  strcat(path, ".c");
  file = fopen(path, "rt");
  if ( !file ) {
    fprintf(stderr, "can't open %s\n", path);
    return 1;
  }

  in_incl = 0;
  in_suite = 0;
  for ( i = 0; fgets(line, 1023, file); ++i ) {
    char* token;
    strvec_push_back(target, line);
    if ( strstr(line,"embunit:include=+") ) in_incl = 1;
    if ( strstr(line,"embunit:fixtures=+"  ) ) in_suite = 1;
    if ( strstr(line,"embunit:include=-") ) { incl_pos  = i; in_incl = 0; }
    if ( strstr(line,"embunit:impl=-"   ) ) impl_pos  = i;
    if ( strstr(line,"embunit:fixtures=-"  ) ) { suite_pos = i; in_suite = 0; }
    else {
      if ( in_incl ) {
        strtok(line,"\"<>"); token = strtok(0, "\"<>");
        if ( token ) {
          int i = strvec_find(includes, token);
          if ( i >= 0 ) {
            strvec_erase(includes,i);
            break;
          }
        }
      }
      else if ( in_suite ) {
        strtok(line,", \t"); token = strtok(0, ", \t"); token = strtok(0, ", \t)");
        if ( token ) {
          int i = strvec_find(cases, token);
          if ( i >= 0 ) {
            strvec_erase(cases,i);
            break;
          }
        }
      }
    }
  }

  fclose(file);

  for ( i = 0; i < strvec_size(cases); ++i ) {
    const char* name = strvec_get(cases,i);
    sprintf(line,"		new_TestFixture(\x22%s\x22,%s),\n", name, name);
    strvec_insert_before(target, suite_pos++, line);
  }

  for ( i = 0; i < strvec_size(cases); ++i ) {
    const char* name = strvec_get(cases,i);
    sprintf(line,"static void %s(void)\n{\n\tTEST_FAIL(\"no implementation\");\n}\n\n", name);
    strvec_insert_before(target, impl_pos++, line);
  }

  for ( i = 0; i < strvec_size(includes); ++i ) {
    const char* name = strvec_get(includes,i);
    sprintf(line,"#include \"%s\"\n", name);
    strvec_insert_before(target, incl_pos++, line);
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
  strvec_del(includes);
  strvec_del(cases);

  return 0;

}
