/*
 * =====================================================================================
 *
 *       Filename:  test_vector.c
 *
 *    Description:  the sample vector test
 *
 *        Version:  1.0
 *        Created:  2013/12/10 17时07分44秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */


#include <stdlib.h>
#include <string.h>
#include "vector.h"
#include <assert.h>
#include <stdio.h>

typedef struct _student
{
	char name[50];
	int age;

} student;

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
	int
main ( int argc, char *argv[] )
{
	student * stu1 = (student*) malloc(sizeof(student));
	student * stu2 = (student*) malloc(sizeof(student));
	student * stu3 = (student*) malloc(sizeof(student));

	bzero(stu1,sizeof(student));
	bzero(stu2,sizeof(student));
	bzero(stu3,sizeof(student));

	strcpy(stu1->name,"stu1");
	stu1->age = 21;

	strcpy(stu1->name,"stu2");
	stu2->age = 22;

	strcpy(stu1->name,"stu3");
	stu3->age = 23;



	vector * vec = (vector *) malloc(sizeof(vector));
	vector_init(vec);

	vector_add(vec,stu1);
	vector_add(vec,stu2);
	vector_add(vec,stu3);

	assert(vector_count(vec) == 3);

	student * stu = (student *) vector_get(vec,1);

	assert(stu->age ==  22);


	printf(">>stu age: %d\n",stu->age);

	vector_delete(vec,1);
	assert(vector_count(vec) == 2);
	

	free(stu1);
	free(stu2);
	free(stu3);


	return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */
