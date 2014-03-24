#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "person.h"

PersonRef Person_alloc(void)
{
	return (PersonRef)malloc(sizeof(Person));
}

PersonRef Person_init(PersonRef self)
{
	return Person_initWithName(self, NULL);
}

PersonRef Person_initWithName(PersonRef self,char *fullname)
{
	self->fullname = NULL;
	self->firstname = NULL;
	self->lastname = NULL;
	Person_setFullName(self,fullname);
	return self;
}

PersonRef Person_personWithName(char *fullname)
{
	return Person_initWithName(Person_alloc(),fullname);
}

void Person_dealloc(PersonRef self)
{
	if (self) {
		free(self->fullname);
		free(self->firstname);
		free(self->lastname);
		free(self);
	}
}

static void setfullname(PersonRef self,char *fullname)
{
	free(self->fullname);
	self->fullname = NULL;
	if (fullname) {
		self->fullname = (char*)malloc(strlen(fullname)+1);
		strcpy(self->fullname,fullname);
	}
}

static void setfirstname(PersonRef self,char *firstname)
{
	free(self->firstname);
	self->firstname = NULL;
	if (firstname) {
		self->firstname = (char*)malloc(strlen(firstname)+1);
		strcpy(self->firstname,firstname);
	}
}

static void setlastname(PersonRef self,char *lastname)
{
	free(self->lastname);
	self->lastname = NULL;
	if (lastname) {
		self->lastname = (char*)malloc(strlen(lastname)+1);
		strcpy(self->lastname,lastname);
	}
}

static void makefullname(PersonRef self)
{
	size_t fl,ll,fulllen,pos;
	fl = ll = fulllen = pos = 0;
	if (self->firstname) {
		fl = strlen(self->firstname);
	}
	if (self->lastname) {
		ll = strlen(self->lastname);
	}
	if (fl) {
		fulllen = fl + 1;	/* + space */
	}
	if (ll) {
		fulllen = fulllen + ll + 1;	/* + null */
	}
	if (fulllen) {
		self->fullname = (char*)malloc(fulllen);
		if (fl && ll) {
			sprintf(self->fullname,"%s %s",self->firstname,self->lastname);
		} else {
			if (fl) {
				strcpy(self->fullname,self->firstname);
			}
			if (ll) {
				strcpy(self->fullname,self->lastname);
			}
		}
	}
}

static void makefirstname(PersonRef self)
{
	if (self->fullname) {
		char *p;
		int len;
		p = strchr(self->fullname, ' ');
		if (p) {
			len = (int)(p - self->fullname);
			p = (char*)malloc(len + 1);
			strncpy(p,self->fullname,len);
			p[len] = '\0';
			setfirstname(self,p);
			free(p);
		} else {
			setfirstname(self,self->fullname);
		}
	}
}

static void makelastname(PersonRef self)
{
	if (self->fullname) {
		char *p = strchr(self->fullname, ' ');
		if (p) {
			setlastname(self,p+1);
		} else {
			setlastname(self,"");
		}
	}
}

char* Person_fullName(PersonRef self)
{
	if (self->fullname == NULL) {
		makefullname(self);
	}
	return self->fullname;
}

char* Person_firstName(PersonRef self)
{
	if (self->firstname == NULL) {
		makefirstname(self);
	}
	return self->firstname;
}

char* Person_lastName(PersonRef self)
{
	if (self->lastname == NULL) {
		makelastname(self);
	}
	return self->lastname;
}

void Person_setFullName(PersonRef self,char *fullname)
{
	setfullname(self,fullname);
	setfirstname(self,NULL);
	setlastname(self,NULL);
}

void Person_setFirstName(PersonRef self,char *firstname)
{
	if (self->lastname == NULL) {
		makelastname(self);
	}
	setfirstname(self,firstname);
	setfullname(self,NULL);
}

void Person_setLastName(PersonRef self,char *lastname)
{
	if (self->firstname == NULL) {
		makefirstname(self);
	}
	setlastname(self,lastname);
	setfullname(self,NULL);
}
