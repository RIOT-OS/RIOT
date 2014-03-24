#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strvec.h"

int main(int argc, char* argv[]) {
  strvec* includes;
  strvec* cases;
  char*   group;
  FILE*   file;
  char    path[256];
  int     i;

  includes = strvec_new(4);
  cases    = strvec_new(4);
  group    = 0;

  for ( i = 1; i < argc; ++i ) {
    char* arg = argv[i];
    if ( strchr(arg,'.') ) {
      strvec_push_back(includes,arg);
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
  if ( file ) {
    fclose(file);
    fprintf(stderr, "%s already exists. (skip)\n", path);
    return 0;
  }

  file = fopen(path, "wt");
  if ( !file ) {
    fprintf(stderr, "%s open failure.\n", path);
    return 1;
  }

  fprintf(file, "#include <embUnit/embUnit.h>\n\n");

  fprintf(file, "/*embunit:include=+ */\n");
  for ( i = 0; i < strvec_size(includes); ++i ) {
  fprintf(file,	"#include \"%s\"\n",strvec_get(includes,i));
  }
  fprintf(file, "/*embunit:include=- */\n\n");

  fprintf(file, "static void setUp(void)\n"
             	"{\n"
              	"\t/* initialize */\n"
              	"}\n\n"
              	"static void tearDown(void)\n"
              	"{\n"
              	"\t/* terminate */\n"
              	"}\n\n"
  );

  fprintf(file, "/*embunit:impl=+ */\n");
  for ( i = 0; i < strvec_size(cases); ++i ) {
  fprintf(file,	"static void %s(void)\n"
    			"{\n"
                "	TEST_FAIL(\"no implementation\");\n"
                "}\n\n"
				,strvec_get(cases, i)
  );
  }
  fprintf(file, "/*embunit:impl=- */\n");

  fprintf(file, "TestRef %s_tests(void)\n"
  				"{\n"
 			 	"	EMB_UNIT_TESTFIXTURES(fixtures) {\n"
                ,group
  );

  fprintf(file, "	/*embunit:fixtures=+ */\n");
  for ( i = 0; i < strvec_size(cases); ++i ) {
  fprintf(file,	"		new_TestFixture(\x22%s\x22,%s),\n"
                  ,strvec_get(cases, i), strvec_get(cases, i)
  );
  }
  fprintf(file, "	/*embunit:fixtures=- */\n");

  fprintf(file,	"	};\n"
				"	EMB_UNIT_TESTCALLER(%s,\x22%s\x22,setUp,tearDown,fixtures);\n"
				"	return (TestRef)&%s;\n"
				"};\n"
				,group,group,group
  );

  fclose(file);

  strvec_del(includes);
  strvec_del(cases);

  return 0;

}
