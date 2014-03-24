#ifndef	__PERSON_H__
#define	__PERSON_H__

typedef struct __Person		Person;
typedef struct __Person*	PersonRef;

struct __Person {
	char *fullname;
	char *firstname;
	char *lastname;
};

PersonRef Person_alloc(void);
PersonRef Person_init(PersonRef);
PersonRef Person_initWithName(PersonRef,char*);
PersonRef Person_personWithName(char*);
void Person_dealloc(PersonRef);
char* Person_fullName(PersonRef);
char* Person_firstName(PersonRef);
char* Person_lastName(PersonRef);
void Person_setFullName(PersonRef,char*);
void Person_setFirstName(PersonRef,char*);
void Person_setLastName(PersonRef,char*);

#endif/*__PERSON_H__*/
