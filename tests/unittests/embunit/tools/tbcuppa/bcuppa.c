#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strvec.h"

int main(int argc, char* argv[]) {
  strvec* groups;
  char*   name;
  FILE*   file;
  char    path[256];
  int     i;

  groups = strvec_new(4);
  name   = 0;

  for ( i = 1; i < argc; ++i ) {
    char arg[256];
    strcpy(arg,argv[i]);
    if ( strchr(arg,'.') ) {
      *strchr(arg,'.') = '\0';
      strvec_push_back(groups,arg);
    } else if ( name ) {
      strvec_push_back(groups,arg);
    } else {
      name = argv[i];
    }
  }

  if ( !name ) {
    return 0;
  }

  strcpy(path, name);
  strcat(path, ".c");

  file = fopen(path, "wt");
  if ( !file ) {
    fprintf(stderr, "%s open failure.\n", path);
    return 1;
  }

  fprintf(file, "#include <embUnit/embUnit.h>\n\n");

  fprintf(file,	"/*embunit:extern=+ */\n");
  for ( i = 0; i < strvec_size(groups); ++i ) {
  fprintf(file, "extern TestRef %s_tests(void);\n"
				,strvec_get(groups,i));
  }
  fprintf(file, "/*embunit:extern=- */\n\n");

  fprintf(file, "int main(int argc,char *argv[])\n"
				"{\n"
                "	TestRunner_start();\n"
  );

  fprintf(file, "	/*embunit:run=+ */\n");
  for ( i = 0; i < strvec_size(groups); ++i ) {
  fprintf(file,	"		TestRunner_runTest(%s_tests());\n", strvec_get(groups,i));
  }
  fprintf(file, "	/*embunit:run=- */\n");
  
  fprintf(file, "	TestRunner_end();\n"
                "	return 0;\n"
                "}\n"
  );

  fclose(file);

  strvec_del(groups);

  return 0;

}
