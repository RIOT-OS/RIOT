#ifndef ZHE_PACKAGE_H
#define ZHE_PACKAGE_H

#define MAKE_PACKAGE_COUNT_ARGS__(z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,cnt,...) cnt
#define MAKE_PACKAGE_COUNT_ARGS_(z,...) MAKE_PACKAGE_COUNT_ARGS__(z,__VA_ARGS__,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0)
#define MAKE_PACKAGE_COUNT_ARGS MAKE_PACKAGE_COUNT_ARGS_

#define MAKE_PACKAGE_SPEC_1(pkg,z,a) MAKE_##pkg##_SPEC_##a z
#define MAKE_PACKAGE_SPEC_2(pkg,z,a,...) MAKE_##pkg##_SPEC_##a z MAKE_PACKAGE_SPEC_1(pkg,z,__VA_ARGS__)
#define MAKE_PACKAGE_SPEC_3(pkg,z,a,...) MAKE_##pkg##_SPEC_##a z MAKE_PACKAGE_SPEC_2(pkg,z,__VA_ARGS__)
#define MAKE_PACKAGE_SPEC_4(pkg,z,a,...) MAKE_##pkg##_SPEC_##a z MAKE_PACKAGE_SPEC_3(pkg,z,__VA_ARGS__)
#define MAKE_PACKAGE_SPEC_5(pkg,z,a,...) MAKE_##pkg##_SPEC_##a z MAKE_PACKAGE_SPEC_4(pkg,z,__VA_ARGS__)
#define MAKE_PACKAGE_SPEC_6(pkg,z,a,...) MAKE_##pkg##_SPEC_##a z MAKE_PACKAGE_SPEC_5(pkg,z,__VA_ARGS__)
#define MAKE_PACKAGE_SPEC_7(pkg,z,a,...) MAKE_##pkg##_SPEC_##a z MAKE_PACKAGE_SPEC_6(pkg,z,__VA_ARGS__)
#define MAKE_PACKAGE_SPEC_8(pkg,z,a,...) MAKE_##pkg##_SPEC_##a z MAKE_PACKAGE_SPEC_7(pkg,z,__VA_ARGS__)
#define MAKE_PACKAGE_SPEC_9(pkg,z,a,...) MAKE_##pkg##_SPEC_##a z MAKE_PACKAGE_SPEC_8(pkg,z,__VA_ARGS__)
#define MAKE_PACKAGE_SPEC_10(pkg,z,a,...) MAKE_##pkg##_SPEC_##a z MAKE_PACKAGE_SPEC_9(pkg,z,__VA_ARGS__)

#define MAKE_PACKAGE_BODY_1(pkg,z,a) MAKE_##pkg##_BODY_##a z
#define MAKE_PACKAGE_BODY_2(pkg,z,a,...) MAKE_##pkg##_BODY_##a z MAKE_PACKAGE_BODY_1(pkg,z,__VA_ARGS__)
#define MAKE_PACKAGE_BODY_3(pkg,z,a,...) MAKE_##pkg##_BODY_##a z MAKE_PACKAGE_BODY_2(pkg,z,__VA_ARGS__)
#define MAKE_PACKAGE_BODY_4(pkg,z,a,...) MAKE_##pkg##_BODY_##a z MAKE_PACKAGE_BODY_3(pkg,z,__VA_ARGS__)
#define MAKE_PACKAGE_BODY_5(pkg,z,a,...) MAKE_##pkg##_BODY_##a z MAKE_PACKAGE_BODY_4(pkg,z,__VA_ARGS__)
#define MAKE_PACKAGE_BODY_6(pkg,z,a,...) MAKE_##pkg##_BODY_##a z MAKE_PACKAGE_BODY_5(pkg,z,__VA_ARGS__)
#define MAKE_PACKAGE_BODY_7(pkg,z,a,...) MAKE_##pkg##_BODY_##a z MAKE_PACKAGE_BODY_6(pkg,z,__VA_ARGS__)
#define MAKE_PACKAGE_BODY_8(pkg,z,a,...) MAKE_##pkg##_BODY_##a z MAKE_PACKAGE_BODY_7(pkg,z,__VA_ARGS__)
#define MAKE_PACKAGE_BODY_9(pkg,z,a,...) MAKE_##pkg##_BODY_##a z MAKE_PACKAGE_BODY_8(pkg,z,__VA_ARGS__)
#define MAKE_PACKAGE_BODY_10(pkg,z,a,...) MAKE_##pkg##_BODY_##a z MAKE_PACKAGE_BODY_9(pkg,z,__VA_ARGS__)

#define MAKE_PACKAGE_PASTE_(x,y) x ## y
#define MAKE_PACKAGE_PASTE(x,y) MAKE_PACKAGE_PASTE_(x,y)
#define MAKE_PACKAGE_SPEC(pkg_, props_, ...) MAKE_PACKAGE_PASTE(MAKE_PACKAGE_SPEC_,MAKE_PACKAGE_COUNT_ARGS(props_,__VA_ARGS__)) (pkg_, props_, __VA_ARGS__)
#define MAKE_PACKAGE_BODY(pkg_, props_, ...) MAKE_PACKAGE_PASTE(MAKE_PACKAGE_BODY_,MAKE_PACKAGE_COUNT_ARGS(props_,__VA_ARGS__)) (pkg_, props_, __VA_ARGS__)

#endif
